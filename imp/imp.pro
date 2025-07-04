#-------------------------------------------------
#
# Project created by QtCreator 2017-02-28T11:51:18
#
#-------------------------------------------------

QT       += core gui serialport qml quick quickwidgets quickcontrols2 printsupport
QT       += serialbus
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = imp
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ./about
INCLUDEPATH += ./HelpBrowser
INCLUDEPATH += ./checkInputNumberIF
INCLUDEPATH += ./Detects

SOURCES += main.cpp\
    Detects/abstractsettingscontroller.cpp \
  Detects/bepvtdetect.cpp \
  Detects/bepvtsettings.cpp \
  Detects/detectfactory.cpp \
  Detects/emvtdetect.cpp \
  Detects/emvtsettings.cpp \
    Detects/impabstractdetect.cpp \
    Detects/mbtcplocator.cpp \
    Detects/measserverdetect.cpp \
  Detects/modbusvtdetect.cpp \
  Detects/serialportlocator.cpp \
  Detects/vt21detect.cpp \
  Detects/vtdetect.cpp \
    ImpMessage/impmessagecreator.cpp \
  Logger/logger.cpp \
  UtilLib/modbus.cpp \
    UtilLib/utillib.cpp \
    formulatree/formulafactory.cpp \
    formulatree/formulanode.cpp \
        imp.cpp \
    impsettings.cpp \
    indicator.cpp \
    checkInputNumberIF/checkInputNumberIF.cpp \
  indsettings.cpp \
  postmessagesender.cpp \
    settingseditordialog.cpp \
    workplacesmodel.cpp

HEADERS  += imp.h \
    Detects/abstractsettingscontroller.h \
    Detects/bepvtdetect.h \
    Detects/bepvtsettings.h \
    Detects/detectfactory.h \
    Detects/emvtdetect.h \
    Detects/emvtsettings.h \
    Detects/impabstractdetect.h \
    Detects/mbtcplocator.h \
    Detects/measserverdetect.h \
    Detects/measservermap.h \
    Detects/modbusvtdetect.h \
    Detects/serialportlocator.h \
    Detects/vt21detect.h \
    Detects/vtdetect.h \
    ImpMessage/impmessagecreator.h \
    Logger/logger.h \
    UtilLib/modbus.h \
    UtilLib/utillib.h \
    WidgetUtil/DetectModelCommands.h \
    formulatree/formulafactory.h \
    formulatree/formulanode.h \
    impdef.h \
    impsettings.h \
    indicator.h \
    checkInputNumberIF/checkInputNumberIF.h \
    indsettings.h \
    postmessagesender.h \
    settingseditordialog.h \
    versionNo.h \
    workplacesmodel.h

RESOURCES += \
    indicator.qrc \
    imp.qrc

DISTFILES += \
    Detects/bepvtsettings.qml \
    HelpBrowser/doc/logo.jpg \
    HelpBrowser/doc/ico-out-mini.png \
    HelpBrowser/doc/about.htm \
    HelpBrowser/doc/charts.htm \
    HelpBrowser/doc/index.htm \
    HelpBrowser/doc/contacts.htm \
    HelpBrowser/doc/indicators.htm \
    HelpBrowser/doc/main.htm \
    HelpBrowser/doc/settings.htm \
    HelpBrowser/doc/start.htm \
    HelpBrowser/doc/help.htm \
    logo.bmp \
    description.txt \
    indicator.qml \
    InputIndicator.qml \
    imp.rc

RC_FILE = imp.rc

include($$PWD/../qBreakpad/qBreakpad.pri)
QMAKE_LIBDIR += $$PWD/../qBreakpad/handler
LIBS += -lqBreakpad

FORMS += \
    settingseditordialog.ui
