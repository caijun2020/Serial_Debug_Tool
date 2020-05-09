#-------------------------------------------------
#
# Project created by QtCreator 2020-04-23T14:34:22
#
#-------------------------------------------------

QT       += core gui

TARGET = CSerialDebug
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    SerialDebugWidget.cpp \
    qextserialbase.cpp \
    QSerialPort.cpp \
    LoopBuffer.cpp \
    QUtilityBox.cpp

HEADERS  += mainwindow.h \
    SerialDebugWidget.h \
    qextserialbase.h \
    QSerialPort.h \
    ComInitData.h \
    LoopBuffer.h \
    QtBaseType.h \
    QUtilityBox.h

win32{
    HEADERS += win_qextserialport.h
    SOURCES += win_qextserialport.cpp
}

unix{
    HEADERS += posix_qextserialport.h
    SOURCES += posix_qextserialport.cpp
}

FORMS    += mainwindow.ui \
    SerialDebugWidget.ui

RC_FILE = icon.rc
