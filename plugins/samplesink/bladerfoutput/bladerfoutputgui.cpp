///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 Edouard Griffiths, F4EXB                                   //
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

#include <QDebug>
#include <QMessageBox>

#include <libbladeRF.h>

#include "ui_bladerfoutputgui.h"
#include "gui/colormapper.h"
#include "gui/glspectrum.h"
#include "dsp/dspengine.h"
#include "dsp/dspcommands.h"
#include "device/devicesinkapi.h"
#include "dsp/filerecord.h"
#include "bladerfoutputgui.h"
#include "bladerf/devicebladerfvalues.h"

BladerfOutputGui::BladerfOutputGui(DeviceSinkAPI *deviceAPI, QWidget* parent) :
	QWidget(parent),
	ui(new Ui::BladerfOutputGui),
	m_deviceAPI(deviceAPI),
	m_settings(),
	m_deviceSampleSink(NULL),
	m_sampleRate(0),
	m_lastEngineState((DSPDeviceSinkEngine::State)-1)
{
	ui->setupUi(this);
	ui->centerFrequency->setColorMapper(ColorMapper(ColorMapper::ReverseGold));
	ui->centerFrequency->setValueRange(7, BLADERF_FREQUENCY_MIN_XB200/1000, BLADERF_FREQUENCY_MAX/1000);

	ui->samplerate->clear();
	for (int i = 0; i < BladerfSampleRates::getNbRates(); i++)
	{
		ui->samplerate->addItem(QString::number(BladerfSampleRates::getRate(i)/1000));
	}

	ui->bandwidth->clear();
	for (int i = 0; i < BladerfBandwidths::getNbBandwidths(); i++)
	{
		ui->bandwidth->addItem(QString::number(BladerfBandwidths::getBandwidth(i)));
	}

	connect(&m_updateTimer, SIGNAL(timeout()), this, SLOT(updateHardware()));
	connect(&m_statusTimer, SIGNAL(timeout()), this, SLOT(updateStatus()));
	m_statusTimer.start(500);

	displaySettings();

	m_deviceSampleSink = new BladerfOutput(m_deviceAPI);
	m_deviceAPI->setSink(m_deviceSampleSink);

	char recFileNameCStr[30];
	sprintf(recFileNameCStr, "test_%d.sdriq", m_deviceAPI->getDeviceUID());
    m_fileSink = new FileRecord(std::string(recFileNameCStr));
//    m_deviceAPI->addSink(m_fileSink);

    connect(m_deviceAPI->getDeviceOutputMessageQueue(), SIGNAL(messageEnqueued()), this, SLOT(handleDSPMessages()), Qt::QueuedConnection);
}

BladerfOutputGui::~BladerfOutputGui()
{
//    m_deviceAPI->removeSink(m_fileSink);
    delete m_fileSink;
	delete m_deviceSampleSink; // Valgrind memcheck
	delete ui;
}

void BladerfOutputGui::destroy()
{
	delete this;
}

void BladerfOutputGui::setName(const QString& name)
{
	setObjectName(name);
}

QString BladerfOutputGui::getName() const
{
	return objectName();
}

void BladerfOutputGui::resetToDefaults()
{
	m_settings.resetToDefaults();
	displaySettings();
	sendSettings();
}

qint64 BladerfOutputGui::getCenterFrequency() const
{
	return m_settings.m_centerFrequency;
}

void BladerfOutputGui::setCenterFrequency(qint64 centerFrequency)
{
	m_settings.m_centerFrequency = centerFrequency;
	displaySettings();
	sendSettings();
}

QByteArray BladerfOutputGui::serialize() const
{
	return m_settings.serialize();
}

bool BladerfOutputGui::deserialize(const QByteArray& data)
{
	if(m_settings.deserialize(data)) {
		displaySettings();
		sendSettings();
		return true;
	} else {
		resetToDefaults();
		return false;
	}
}

bool BladerfOutputGui::handleMessage(const Message& message)
{
	if (BladerfOutput::MsgReportBladerf::match(message))
	{
		displaySettings();
		return true;
	}
	else
	{
		return false;
	}
}

void BladerfOutputGui::handleDSPMessages()
{
    Message* message;

    while ((message = m_deviceAPI->getDeviceOutputMessageQueue()->pop()) != 0)
    {
        qDebug("BladerfOutputGui::handleDSPMessages: message: %s", message->getIdentifier());

        if (DSPSignalNotification::match(*message))
        {
            DSPSignalNotification* notif = (DSPSignalNotification*) message;
            m_sampleRate = notif->getSampleRate();
            m_deviceCenterFrequency = notif->getCenterFrequency();
            qDebug("BladerfOutputGui::handleDSPMessages: SampleRate:%d, CenterFrequency:%llu", notif->getSampleRate(), notif->getCenterFrequency());
            updateSampleRateAndFrequency();
            m_fileSink->handleMessage(*notif); // forward to file sink

            delete message;
        }
    }
}

void BladerfOutputGui::updateSampleRateAndFrequency()
{
    m_deviceAPI->getSpectrum()->setSampleRate(m_sampleRate);
    m_deviceAPI->getSpectrum()->setCenterFrequency(m_deviceCenterFrequency);
    ui->deviceRateLabel->setText(tr("%1k").arg((float)m_sampleRate / 1000));
}

void BladerfOutputGui::displaySettings()
{
	ui->centerFrequency->setValue(m_settings.m_centerFrequency / 1000);

	unsigned int sampleRateIndex = BladerfSampleRates::getRateIndex(m_settings.m_devSampleRate);
	ui->samplerate->setCurrentIndex(sampleRateIndex);

	unsigned int bandwidthIndex = BladerfBandwidths::getBandwidthIndex(m_settings.m_bandwidth);
	ui->bandwidth->setCurrentIndex(bandwidthIndex);

	ui->interp->setCurrentIndex(m_settings.m_log2Interp);

	ui->vga1Text->setText(tr("%1dB").arg(m_settings.m_vga1));
	ui->vga1->setValue(m_settings.m_vga1);

	ui->vga2Text->setText(tr("%1dB").arg(m_settings.m_vga2));
	ui->vga2->setValue(m_settings.m_vga2);

	ui->xb200->setCurrentIndex(getXb200Index(m_settings.m_xb200, m_settings.m_xb200Path, m_settings.m_xb200Filter));
}

void BladerfOutputGui::sendSettings()
{
	if(!m_updateTimer.isActive())
		m_updateTimer.start(100);
}

void BladerfOutputGui::on_centerFrequency_changed(quint64 value)
{
	m_settings.m_centerFrequency = value * 1000;
	sendSettings();
}

void BladerfOutputGui::on_samplerate_currentIndexChanged(int index)
{
	int newrate = BladerfSampleRates::getRate(index);
	m_settings.m_devSampleRate = newrate;
	sendSettings();
}

void BladerfOutputGui::on_bandwidth_currentIndexChanged(int index)
{
	int newbw = BladerfBandwidths::getBandwidth(index);
	m_settings.m_bandwidth = newbw * 1000;
	sendSettings();
}

void BladerfOutputGui::on_interp_currentIndexChanged(int index)
{
	if ((index <0) || (index > 6))
		return;
	m_settings.m_log2Interp = index;
	sendSettings();
}

void BladerfOutputGui::on_vga1_valueChanged(int value)
{
	if ((value < BLADERF_TXVGA1_GAIN_MIN) || (value > BLADERF_TXVGA1_GAIN_MAX))
		return;

	ui->vga1Text->setText(tr("%1dB").arg(value));
	m_settings.m_vga1 = value;
	sendSettings();
}

void BladerfOutputGui::on_vga2_valueChanged(int value)
{
	if ((value < BLADERF_TXVGA2_GAIN_MIN) || (value > BLADERF_TXVGA2_GAIN_MAX))
		return;

	ui->vga2Text->setText(tr("%1dB").arg(value));
	m_settings.m_vga2 = value;
	sendSettings();
}

void BladerfOutputGui::on_xb200_currentIndexChanged(int index)
{
	if (index == 1) // bypass
	{
		m_settings.m_xb200 = true;
		m_settings.m_xb200Path = BLADERF_XB200_BYPASS;
	}
	else if (index == 2) // Auto 1dB
	{
		m_settings.m_xb200 = true;
		m_settings.m_xb200Path = BLADERF_XB200_MIX;
		m_settings.m_xb200Filter = BLADERF_XB200_AUTO_1DB;
	}
	else if (index == 3) // Auto 3dB
	{
		m_settings.m_xb200 = true;
		m_settings.m_xb200Path = BLADERF_XB200_MIX;
		m_settings.m_xb200Filter = BLADERF_XB200_AUTO_3DB;
	}
	else if (index == 4) // Custom
	{
		m_settings.m_xb200 = true;
		m_settings.m_xb200Path = BLADERF_XB200_MIX;
		m_settings.m_xb200Filter = BLADERF_XB200_CUSTOM;
	}
	else if (index == 5) // 50 MHz
	{
		m_settings.m_xb200 = true;
		m_settings.m_xb200Path = BLADERF_XB200_MIX;
		m_settings.m_xb200Filter = BLADERF_XB200_50M;
	}
	else if (index == 6) // 144 MHz
	{
		m_settings.m_xb200 = true;
		m_settings.m_xb200Path = BLADERF_XB200_MIX;
		m_settings.m_xb200Filter = BLADERF_XB200_144M;
	}
	else if (index == 7) // 222 MHz
	{
		m_settings.m_xb200 = true;
		m_settings.m_xb200Path = BLADERF_XB200_MIX;
		m_settings.m_xb200Filter = BLADERF_XB200_222M;
	}
	else // no xb200
	{
		m_settings.m_xb200 = false;
	}

	if (m_settings.m_xb200)
	{
		ui->centerFrequency->setValueRange(7, BLADERF_FREQUENCY_MIN_XB200/1000, BLADERF_FREQUENCY_MAX/1000);
	}
	else
	{
		ui->centerFrequency->setValueRange(7, BLADERF_FREQUENCY_MIN/1000, BLADERF_FREQUENCY_MAX/1000);
	}

	sendSettings();
}

void BladerfOutputGui::on_startStop_toggled(bool checked)
{
    if (checked)
    {
        if (m_deviceAPI->initGeneration())
        {
            m_deviceAPI->startGeneration();
            DSPEngine::instance()->startAudioInput();
        }
    }
    else
    {
        m_deviceAPI->stopGeneration();
        DSPEngine::instance()->stopAudioInput();
    }
}

void BladerfOutputGui::updateHardware()
{
	qDebug() << "BladerfGui::updateHardware";
	BladerfOutput::MsgConfigureBladerf* message = BladerfOutput::MsgConfigureBladerf::create( m_settings);
	m_deviceSampleSink->getInputMessageQueue()->push(message);
	m_updateTimer.stop();
}

void BladerfOutputGui::updateStatus()
{
    int state = m_deviceAPI->state();

    if(m_lastEngineState != state)
    {
        switch(state)
        {
            case DSPDeviceSinkEngine::StNotStarted:
                ui->startStop->setStyleSheet("QToolButton { background:rgb(79,79,79); }");
                break;
            case DSPDeviceSinkEngine::StIdle:
                ui->startStop->setStyleSheet("QToolButton { background-color : blue; }");
                break;
            case DSPDeviceSinkEngine::StRunning:
                ui->startStop->setStyleSheet("QToolButton { background-color : red; }");
                break;
            case DSPDeviceSinkEngine::StError:
                ui->startStop->setStyleSheet("QToolButton { background-color : magenta; }");
                QMessageBox::information(this, tr("Message"), m_deviceAPI->errorMessage());
                break;
            default:
                break;
        }

        m_lastEngineState = state;
    }
}

unsigned int BladerfOutputGui::getXb200Index(bool xb_200, bladerf_xb200_path xb200Path, bladerf_xb200_filter xb200Filter)
{
	if (xb_200)
	{
		if (xb200Path == BLADERF_XB200_BYPASS)
		{
			return 1;
		}
		else
		{
			if (xb200Filter == BLADERF_XB200_AUTO_1DB)
			{
				return 2;
			}
			else if (xb200Filter == BLADERF_XB200_AUTO_3DB)
			{
				return 3;
			}
			else if (xb200Filter == BLADERF_XB200_CUSTOM)
			{
				return 4;
			}
			else if (xb200Filter == BLADERF_XB200_50M)
			{
				return 5;
			}
			else if (xb200Filter == BLADERF_XB200_144M)
			{
				return 6;
			}
			else if (xb200Filter == BLADERF_XB200_222M)
			{
				return 7;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
		return 0;
	}
}
