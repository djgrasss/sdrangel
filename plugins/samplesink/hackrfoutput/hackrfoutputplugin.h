///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 Edouard Griffiths, F4EXB                                   //
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

#ifndef INCLUDE_HACKRFOUTPUTPLUGIN_H
#define INCLUDE_HACKRFOUTPUTPLUGIN_H

#include <QObject>
#include "plugin/plugininterface.h"

#define HACKRFOUTPUT_DEVICE_TYPE_ID "sdrangel.samplesource.hackrfoutput"

class PluginAPI;

class HackRFOutputPlugin : public QObject, public PluginInterface {
	Q_OBJECT
	Q_INTERFACES(PluginInterface)
	Q_PLUGIN_METADATA(IID HACKRFOUTPUT_DEVICE_TYPE_ID)

public:
	explicit HackRFOutputPlugin(QObject* parent = NULL);

	const PluginDescriptor& getPluginDescriptor() const;
	void initPlugin(PluginAPI* pluginAPI);

	virtual SamplingDevices enumSampleSinks();
	virtual PluginGUI* createSampleSinkPluginGUI(const QString& sinkId, QWidget **widget, DeviceSinkAPI *deviceAPI);

	static const QString m_hardwareID;
    static const QString m_deviceTypeID;

private:
	static const PluginDescriptor m_pluginDescriptor;
};

#endif // INCLUDE_HACKRFOUTPUTPLUGIN_H
