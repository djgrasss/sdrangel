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

#ifndef INCLUDE_BLADERFINPUT_H
#define INCLUDE_BLADERFINPUT_H

#include <dsp/devicesamplesource.h>
#include "bladerf/devicebladerf.h"
#include "bladerf/devicebladerfparam.h"

#include <libbladeRF.h>
#include <QString>

#include "bladerfinputsettings.h"

class DeviceSourceAPI;
class BladerfInputThread;

class BladerfInput : public DeviceSampleSource {
public:
	class MsgConfigureBladerf : public Message {
		MESSAGE_CLASS_DECLARATION

	public:
		const BladeRFInputSettings& getSettings() const { return m_settings; }

		static MsgConfigureBladerf* create(const BladeRFInputSettings& settings)
		{
			return new MsgConfigureBladerf(settings);
		}

	private:
		BladeRFInputSettings m_settings;

		MsgConfigureBladerf(const BladeRFInputSettings& settings) :
			Message(),
			m_settings(settings)
		{ }
	};

	class MsgReportBladerf : public Message {
		MESSAGE_CLASS_DECLARATION

	public:

		static MsgReportBladerf* create()
		{
			return new MsgReportBladerf();
		}

	protected:

		MsgReportBladerf() :
			Message()
		{ }
	};

	BladerfInput(DeviceSourceAPI *deviceAPI);
	virtual ~BladerfInput();

	virtual bool init(const Message& message);
	virtual bool start(int device);
	virtual void stop();

	virtual const QString& getDeviceDescription() const;
	virtual int getSampleRate() const;
	virtual quint64 getCenterFrequency() const;

	virtual bool handleMessage(const Message& message);

private:
	bool applySettings(const BladeRFInputSettings& settings, bool force);
	bladerf_lna_gain getLnaGain(int lnaGain);
//	struct bladerf *open_bladerf_from_serial(const char *serial);

	DeviceSourceAPI *m_deviceAPI;
	QMutex m_mutex;
	BladeRFInputSettings m_settings;
	struct bladerf* m_dev;
	BladerfInputThread* m_bladerfThread;
	QString m_deviceDescription;
	DeviceBladeRFParams m_sharedParams;
};

#endif // INCLUDE_BLADERFINPUT_H
