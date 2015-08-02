#-------------------------------------------------
#
# Project created by QtCreator 2014-03-24T18:47:18
#
#-------------------------------------------------

QT       += core gui
QT       += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LANUdpExperiment
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    friendproperty.cpp \
    msgrecsocket.cpp \
    msgsendsocket.cpp \
    dialogrecfile.cpp \
    dialogsendfile.cpp \
    filesendsocket.cpp \
    filerecsocket.cpp \
    udpsocket.cpp

HEADERS  += mainwindow.h \
    msg.h \
    friendproperty.h \
    msgrecsocket.h \
    msgsendsocket.h \
    dialogrecfile.h \
    dialogsendfile.h \
    filesendsocket.h \
    filerecsocket.h \
    udpsocket.h

FORMS    += mainwindow.ui \
    dialogrecfile.ui \
    dialogsendfile.ui
LIBS += -lWs2_32

RESOURCES += \
    ico.qrc

RC_FILE+= ico.rc
