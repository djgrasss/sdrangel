///////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 Edouard Griffiths, F4EXB                                   //
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

#include <QtPlugin>
#include <QAction>
#include "plugin/pluginapi.h"
#include "device/deviceapi.h"
#include "util/simpleserializer.h"

#include "sdrdaemonfecplugin.h"

#include "sdrdaemonfecgui.h"

const PluginDescriptor SDRdaemonFECPlugin::m_pluginDescriptor = {
	QString("SDRdaemon input"),
	QString("2.1.0"),
	QString("(c) Edouard Griffiths, F4EXB"),
	QString("https://github.com/f4exb/sdrangel"),
	true,
	QString("https://github.com/f4exb/sdrangel")
};

const QString SDRdaemonFECPlugin::m_deviceTypeID = SDRDAEMONFEC_DEVICE_TYPE_ID;

SDRdaemonFECPlugin::SDRdaemonFECPlugin(QObject* parent) :
	QObject(parent)
{
}

const PluginDescriptor& SDRdaemonFECPlugin::getPluginDescriptor() const
{
	return m_pluginDescriptor;
}

void SDRdaemonFECPlugin::initPlugin(PluginAPI* pluginAPI)
{
	pluginAPI->registerSampleSource(m_deviceTypeID, this);
}

PluginInterface::SampleSourceDevices SDRdaemonFECPlugin::enumSampleSources()
{
	SampleSourceDevices result;
	int count = 1;

	for(int i = 0; i < count; i++)
	{
		QString displayedName(QString("SDRdaemon[%1]").arg(i));

		result.append(SampleSourceDevice(displayedName,
				m_deviceTypeID,
				QString::null,
				i));
	}

	return result;
}

PluginGUI* SDRdaemonFECPlugin::createSampleSourcePluginGUI(const QString& sourceId, QWidget **widget, DeviceAPI *deviceAPI)
{
	if(sourceId == m_deviceTypeID)
	{
		SDRdaemonFECGui* gui = new SDRdaemonFECGui(deviceAPI);
		*widget = gui;
		return gui;
	}
	else
	{
		return NULL;
	}
}