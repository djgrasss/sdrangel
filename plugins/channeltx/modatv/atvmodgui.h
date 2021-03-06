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

#ifndef PLUGINS_CHANNELTX_MODTV_ATVMODGUI_H_
#define PLUGINS_CHANNELTX_MODTV_ATVMODGUI_H_

#include "gui/rollupwidget.h"
#include "plugin/plugingui.h"
#include "dsp/channelmarker.h"
#include "dsp/movingaverage.h"
#include "atvmod.h"

class PluginAPI;
class DeviceSinkAPI;

class ThreadedBasebandSampleSource;
class UpChannelizer;
class ATVMod;
class QMessageBox;

namespace Ui {
    class ATVModGUI;
}

class ATVModGUI : public RollupWidget, public PluginGUI {
    Q_OBJECT

public:
    static ATVModGUI* create(PluginAPI* pluginAPI, DeviceSinkAPI *deviceAPI);
    void destroy();

    void setName(const QString& name);
    QString getName() const;
    virtual qint64 getCenterFrequency() const;
    virtual void setCenterFrequency(qint64 centerFrequency);

    void resetToDefaults();
    QByteArray serialize() const;
    bool deserialize(const QByteArray& data);

    virtual bool handleMessage(const Message& message);

    static const QString m_channelID;

private slots:
    void viewChanged();
    void channelizerOutputSampleRateChanged();
    void handleSourceMessages();

    void on_deltaFrequency_changed(quint64 value);
    void on_deltaMinus_toggled(bool minus);
    void on_channelMute_toggled(bool checked);
    void on_forceDecimator_toggled(bool checked);
    void on_modulation_currentIndexChanged(int index);
    void on_rfScaling_valueChanged(int value);
    void on_fmExcursion_valueChanged(int value);
    void on_rfBW_valueChanged(int value);
    void on_rfOppBW_valueChanged(int value);
    void on_nbLines_currentIndexChanged(int index);
    void on_fps_currentIndexChanged(int index);
    void on_standard_currentIndexChanged(int index);
    void on_invertVideo_clicked();
    void on_uniformLevel_valueChanged(int value);
    void on_inputSelect_currentIndexChanged(int index);
    void on_imageFileDialog_clicked(bool checked);
    void on_videoFileDialog_clicked(bool checked);

    void on_playVideo_toggled(bool checked);
    void on_playLoop_toggled(bool checked);
    void on_navTimeSlider_valueChanged(int value);

    void on_playCamera_toggled(bool checked);
    void on_camSelect_currentIndexChanged(int index);
    void on_cameraManualFPSEnable_toggled(bool checked);
    void on_cameraManualFPS_valueChanged(int value);

    void on_overlayTextShow_toggled(bool checked);
    void on_overlayText_textEdited(const QString& arg1);

    void onWidgetRolled(QWidget* widget, bool rollDown);
    void onMenuDoubleClicked();

    void configureImageFileName();
    void configureVideoFileName();
    void tick();

private:
    Ui::ATVModGUI* ui;
    PluginAPI* m_pluginAPI;
    DeviceSinkAPI* m_deviceAPI;
    ChannelMarker m_channelMarker;
    bool m_basicSettingsShown;
    bool m_doApplySettings;

    ThreadedBasebandSampleSource* m_threadedChannelizer;
    UpChannelizer* m_channelizer;
    ATVMod* m_atvMod;
    MovingAverage<Real> m_channelPowerDbAvg;

    QString m_imageFileName;
    QString m_videoFileName;
    quint32 m_videoLength;   //!< video file length in seconds
    float m_videoFrameRate;  //!< video file frame rate
    int m_frameCount;
    std::size_t m_tickCount;
    bool m_enableNavTime;
    QMessageBox *m_camBusyFPSMessageBox;
    int m_rfSliderDivisor;

    explicit ATVModGUI(PluginAPI* pluginAPI, DeviceSinkAPI *deviceAPI, QWidget* parent = NULL);
    virtual ~ATVModGUI();

    void blockApplySettings(bool block);
    void applySettings();
    void updateWithStreamData();
    void updateWithStreamTime();
    void setRFFiltersSlidersRange(int sampleRate);
    void setChannelMarkerBandwidth();
    int getNbLines();
    int getFPS();

    void leaveEvent(QEvent*);
    void enterEvent(QEvent*);
};

#endif /* PLUGINS_CHANNELTX_MODAM_AMMODGUI_H_ */
