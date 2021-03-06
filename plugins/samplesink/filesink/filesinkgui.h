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

#ifndef INCLUDE_FILESINKGUI_H
#define INCLUDE_FILESINKGUI_H

#include <QTimer>

#include "filesinkoutput.h"
#include "filesinksettings.h"
#include "plugin/plugingui.h"


class DeviceSinkAPI;
class DeviceSampleSink;

namespace Ui {
	class FileSinkGui;
}

class FileSinkGui : public QWidget, public PluginGUI {
	Q_OBJECT

public:
	explicit FileSinkGui(DeviceSinkAPI *deviceAPI, QWidget* parent = NULL);
	virtual ~FileSinkGui();
	void destroy();

	void setName(const QString& name);
	QString getName() const;

	void resetToDefaults();
	virtual qint64 getCenterFrequency() const;
	virtual void setCenterFrequency(qint64 centerFrequency);
	QByteArray serialize() const;
	bool deserialize(const QByteArray& data);
	virtual bool handleMessage(const Message& message);

private:
	Ui::FileSinkGui* ui;

	DeviceSinkAPI* m_deviceAPI;
	FileSinkSettings m_settings;
    QString m_fileName;
	QTimer m_updateTimer;
    QTimer m_statusTimer;
	DeviceSampleSink* m_deviceSampleSink;
    int m_sampleRate;
    quint64 m_deviceCenterFrequency; //!< Center frequency in device
    bool m_generation;
	std::time_t m_startingTimeStamp;
	int m_samplesCount;
	std::size_t m_tickCount;
	int m_lastEngineState;

	void displaySettings();
	void displayTime();
	void sendSettings();
	void configureFileName();
	void updateWithGeneration();
	void updateWithStreamTime();
	void updateSampleRateAndFrequency();

private slots:
    void handleDSPMessages();
    void handleSinkMessages();
    void on_centerFrequency_changed(quint64 value);
	void on_startStop_toggled(bool checked);
	void on_showFileDialog_clicked(bool checked);
	void on_interp_currentIndexChanged(int index);
    void on_sampleRate_currentIndexChanged(int index);
    void updateHardware();
    void updateStatus();
	void tick();
};

class FileSinkSampleRates {
public:
	static unsigned int getRate(unsigned int rate_index);
	static unsigned int getRateIndex(unsigned int rate);
	static unsigned int getNbRates();
	static const unsigned int m_nb_rates;
	static const unsigned int m_rates[];
};

#endif // INCLUDE_FILESINKGUI_H
