///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 F4EXB                                                      //
// written by Edouard Griffiths                                                  //
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


#ifndef PLUGINS_CHANNEL_BFM_RDSDEMOD_H_
#define PLUGINS_CHANNEL_BFM_RDSDEMOD_H_

#include <QObject>
#include "util/udpsink.h"

#include "dsp/dsptypes.h"

class RDSDemod : public QObject
{
    Q_OBJECT
public:
	RDSDemod();
	~RDSDemod();

	void setSampleRate(int srate);
	void process(Real rdsSample, Real pilotSample);

protected:
	void biphase(Real acc, Real d_cphi);
	Real filter_lp_2400_iq(Real in, int iqIndex);
	Real filter_lp_pll(Real input);
	int sign(Real a);

private:
	struct
	{
		double subcarr_phi;
		Real subcarr_bb[2];
		double clock_offset;
		double clock_phi;
		double prev_clock_phi;
		Real lo_clock;
		Real prev_lo_clock;
		Real prev_bb;
		double d_cphi;
		Real acc;
		int numsamples;
		Real prev_acc;
		int counter;
		int reading_frame;
		int tot_errs[2];
	} m_parms;

	Real m_xv[2][2+1];
	Real m_yv[2][2+1];
	Real m_xw[1+1];
	Real m_yw[1+1];
	Real m_prev;

	int m_srate;

	UDPSink<Real> m_udpDebug;

	static const int m_udpSize;
	static const Real m_pllBeta;
	static const Real m_fsc;
};

#endif /* PLUGINS_CHANNEL_BFM_RDSDEMOD_H_ */
