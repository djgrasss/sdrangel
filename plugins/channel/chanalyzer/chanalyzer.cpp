///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 maintech GmbH, Otto-Hahn-Str. 15, 97204 Hoechberg, Germany //
// written by Christian Daniel                                                   //
// (c) 2014 Modified by John Greb
//                                                                               //
// This program is free software; you can redistribute it and/or modify          //
// it under the terms of the GNU General Public License as published by          //
// the Free Software Foundation as version 3 of the License, or                  //
//                                                                               //
// This program is distributed in the hope that it will be useful,               //
// but WITHOUT ANY WARRANTY; without even the implied warranty of                //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                  //
// GNU General Public License V3 for more details.                               //
//                                                                               //
// You should have received a copy of the GNU General Public License             //
// along with this program. If not, see <http://www.gnu.org/licenses/>.          //
///////////////////////////////////////////////////////////////////////////////////

#include <QTime>
#include <stdio.h>
#include "audio/audiooutput.h"
#include "dsp/dspcommands.h"

#include <iostream>
#include "chanalyzer.h"

MESSAGE_CLASS_DEFINITION(ChannelAnalyzer::MsgConfigureChannelAnalyzer, Message)

ChannelAnalyzer::ChannelAnalyzer(SampleSink* sampleSink) :
	m_sampleSink(sampleSink)
{
	m_Bandwidth = 5000;
	m_LowCutoff = 300;
	//m_volume = 2.0;
	m_spanLog2 = 3;
	m_sampleRate = 96000;
	m_frequency = 0;
	m_nco.setFreq(m_frequency, m_sampleRate);
	m_nco_test.setFreq(m_frequency, m_sampleRate);
	m_interpolator.create(16, m_sampleRate, 5000);
	m_sampleDistanceRemain = (Real)m_sampleRate / 48000.0;

	//m_audioBuffer.resize(512);
	//m_audioBufferFill = 0;
	m_undersampleCount = 0;

	m_usb = true;
	m_ssb = true;
	SSBFilter = new fftfilt(m_LowCutoff / 48000.0, m_Bandwidth / 48000.0, ssbFftLen);
	DSBFilter = new fftfilt(m_Bandwidth / 48000.0, 2*ssbFftLen);
	// if (!USBFilter) segfault;
}

ChannelAnalyzer::~ChannelAnalyzer()
{
	if (SSBFilter) delete SSBFilter;
	if (DSBFilter) delete DSBFilter;
}

void ChannelAnalyzer::configure(MessageQueue* messageQueue,
		Real Bandwidth,
		Real LowCutoff,
		int  spanLog2,
		bool ssb)
{
	Message* cmd = MsgConfigureChannelAnalyzer::create(Bandwidth, LowCutoff, spanLog2, ssb);
	cmd->submit(messageQueue, this);
}

void ChannelAnalyzer::feed(SampleVector::const_iterator begin, SampleVector::const_iterator end, bool positiveOnly)
{
	Complex ci;
	fftfilt::cmplx *sideband, sum;
	int n_out;
	int decim = 1<<(m_spanLog2 - 1);
	unsigned char decim_mask = decim - 1; // counter LSB bit mask for decimation by 2^(m_scaleLog2 - 1)

	for(SampleVector::const_iterator it = begin; it < end; ++it) {
		Complex c(it->real() / 32768.0, it->imag() / 32768.0);
		c *= m_nco.nextIQ();

		/*
		ci = c;
		if (m_ssb) {
			n_out = SSBFilter->runSSB(ci, &sideband, m_usb);
		} else {
			n_out = DSBFilter->noFilt(ci, &sideband);
		}
		*/

		if(m_interpolator.interpolate(&m_sampleDistanceRemain, c, &ci))
		{
			if (m_ssb)
			{
				n_out = SSBFilter->runSSB(ci, &sideband, m_usb);
			}
			else
			{
				n_out = DSBFilter->runDSB(ci, &sideband);
			}
			m_sampleDistanceRemain += (Real)m_sampleRate / 48000.0;
		}
		else
		{
			n_out = 0;
		}

		for (int i = 0; i < n_out; i++)
		{
			// Downsample by 2^(m_scaleLog2 - 1) for SSB band spectrum display
			// smart decimation with bit gain using float arithmetic (23 bits significand)

			sum += sideband[i];

			if (!(m_undersampleCount++ & decim_mask))
			{
				sum /= decim;
				m_sampleBuffer.push_back(Sample(sum.real() * 32768.0, sum.imag() * 32768.0));
				sum = 0;
			}

		}
	}

	if(m_sampleSink != NULL)
	{
		m_sampleSink->feed(m_sampleBuffer.begin(), m_sampleBuffer.end(), m_ssb); // m_ssb = positive only
	}

	m_sampleBuffer.clear();
}

void ChannelAnalyzer::start()
{
}

void ChannelAnalyzer::stop()
{
}

bool ChannelAnalyzer::handleMessage(Message* cmd)
{
	float band, lowCutoff;

	if(DSPSignalNotification::match(cmd)) {
		DSPSignalNotification* signal = (DSPSignalNotification*)cmd;
		//fprintf(stderr, "%d samples/sec, %lld Hz offset", signal->getSampleRate(), signal->getFrequencyOffset());
		m_sampleRate = signal->getSampleRate();
		m_nco.setFreq(-signal->getFrequencyOffset(), m_sampleRate);
		m_interpolator.create(16, m_sampleRate, m_Bandwidth);
		m_sampleDistanceRemain = m_sampleRate / 48000.0;
		cmd->completed();
		return true;
	} else if(MsgConfigureChannelAnalyzer::match(cmd)) {
		MsgConfigureChannelAnalyzer* cfg = (MsgConfigureChannelAnalyzer*)cmd;

		band = cfg->getBandwidth();
		lowCutoff = cfg->getLoCutoff();

		if (band < 0) {
			band = -band;
			lowCutoff = -lowCutoff;
			m_usb = false;
		} else
			m_usb = true;

		if (band < 100.0f)
		{
			band = 100.0f;
			lowCutoff = 0;
		}

		m_Bandwidth = band;
		m_LowCutoff = lowCutoff;

		m_interpolator.create(16, m_sampleRate, band * 2.0f);
		SSBFilter->create_filter(m_LowCutoff / 48000.0f, m_Bandwidth / 48000.0f);
		DSBFilter->create_dsb_filter(m_Bandwidth / 48000.0f);

		//m_volume = cfg->getVolume();
		//m_volume *= m_volume * 0.1;

		m_spanLog2 = cfg->getSpanLog2();
		m_ssb = cfg->getSSB();

		cmd->completed();
		return true;
	} else {
		if(m_sampleSink != NULL)
		   return m_sampleSink->handleMessage(cmd);
		else return false;
	}
}