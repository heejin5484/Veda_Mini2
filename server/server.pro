QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network sql

CONFIG += c++17

# execute file name
TARGET = Server

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chatroom.cpp \
    chatserver.cpp \
    databasewindow.cpp \
    main.cpp \
    mainwindow.cpp \
    rtpprocess.cpp \
    databasemanager.cpp

HEADERS += \
    chatroom.h \
    chatserver.h \
    constants.h \
    databasewindow.h \
    mainwindow.h \
    rtpprocess.h \
    databasemanager.h

FORMS += \
    chatroom.ui \
    databasewindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
