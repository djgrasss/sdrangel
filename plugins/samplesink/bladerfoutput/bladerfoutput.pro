#--------------------------------------------
#
# Pro file for Windows builds with Qt Creator
#
#--------------------------------------------

TEMPLATE = lib
CONFIG += plugin

QT += core gui widgets multimedia opengl

TARGET = outputbladerf

DEFINES += USE_SSE2=1
QMAKE_CXXFLAGS += -msse2
DEFINES += USE_SSE4_1=1
QMAKE_CXXFLAGS += -msse4.1

CONFIG(MINGW32):LIBBLADERFSRC = "D:\softs\bladeRF\host\libraries\libbladeRF\include"
CONFIG(MINGW64):LIBBLADERFSRC = "D:\softs\bladeRF\host\libraries\libbladeRF\include"
INCLUDEPATH += $$PWD
INCLUDEPATH += ../../../sdrbase
INCLUDEPATH += ../../../devices
INCLUDEPATH += $$LIBBLADERFSRC

CONFIG(Release):build_subdir = release
CONFIG(Debug):build_subdir = debug

SOURCES += bladerfoutputgui.cpp\
    bladerfoutput.cpp\
    bladerfoutputplugin.cpp\
    bladerfoutputsettings.cpp\
    bladerfoutputthread.cpp

HEADERS += bladerfoutputgui.h\
    bladerfoutput.h\
    bladerfoutputplugin.h\
    bladerfoutputsettings.h\
    bladerfoutputthread.h

FORMS += bladerfoutputgui.ui

LIBS += -L../../../sdrbase/$${build_subdir} -lsdrbase
LIBS += -L../../../libbladerf/$${build_subdir} -llibbladerf
LIBS += -L../../../devices/$${build_subdir} -ldevices

RESOURCES = ../../../sdrbase/resources/res.qrc
