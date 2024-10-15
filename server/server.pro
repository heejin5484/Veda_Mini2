QT       += core gui network multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

# execute file name
TARGET = Server

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camerathread.cpp \
    chatroom.cpp \
    chatserver.cpp \
    main.cpp \
    mainwindow.cpp \

HEADERS += \
    camerathread.h \
    chatroom.h \
    chatserver.h \
    mainwindow.h

FORMS += \
    chatroom.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
