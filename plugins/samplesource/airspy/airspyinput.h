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

#ifndef INCLUDE_AIRSPYINPUT_H
#define INCLUDE_AIRSPYINPUT_H

#include <dsp/devicesamplesource.h>

#include "airspysettings.h"
#include <libairspy/airspy.h>
#include <QString>

class DeviceSourceAPI;
class AirspyThread;

class AirspyInput : public DeviceSampleSource {
public:
	class MsgConfigureAirspy : public Message {
		MESSAGE_CLASS_DECLARATION

	public:
		const AirspySettings& getSettings() const { return m_settings; }

		static MsgConfigureAirspy* create(const AirspySettings& settings)
		{
			return new MsgConfigureAirspy(settings);
		}

	private:
		AirspySettings m_settings;

		MsgConfigureAirspy(const AirspySettings& settings) :
			Message(),
			m_settings(settings)
		{ }
	};

	class MsgReportAirspy : public Message {
		MESSAGE_CLASS_DECLARATION

	public:
		const std::vector<uint32_t>& getSampleRates() const { return m_sampleRates; }

		static MsgReportAirspy* create(const std::vector<uint32_t>& sampleRates)
		{
			return new MsgReportAirspy(sampleRates);
		}

	protected:
		std::vector<uint32_t> m_sampleRates;

		MsgReportAirspy(const std::vector<uint32_t>& sampleRates) :
			Message(),
			m_sampleRates(sampleRates)
		{ }
	};

	AirspyInput(DeviceSourceAPI *deviceAPI);
	virtual ~AirspyInput();

	virtual bool init(const Message& message);
	virtual bool start(int device);
	virtual void stop();

	virtual const QString& getDeviceDescription() const;
	virtual int getSampleRate() const;
	virtual quint64 getCenterFrequency() const;
	const std::vector<uint32_t>& getSampleRates() const { return m_sampleRates; }

	virtual bool handleMessage(const Message& message);

private:
	bool applySettings(const AirspySettings& settings, bool force);
	struct airspy_device *open_airspy_from_sequence(int sequence);
	void setCenterFrequency(quint64 freq);

	DeviceSourceAPI *m_deviceAPI;
	QMutex m_mutex;
	AirspySettings m_settings;
	struct airspy_device* m_dev;
	AirspyThread* m_airspyThread;
	QString m_deviceDescription;
	std::vector<uint32_t> m_sampleRates;
};

#endif // INCLUDE_AIRSPYINPUT_H
