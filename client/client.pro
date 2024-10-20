QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# ���� ���� �̸� ����
TARGET = Client

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    chat.cpp \
    loginwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    networkmanager.cpp \
    joinwindow.cpp \
    rtpclient.cpp

HEADERS += \
    chat.h \
    loginwindow.h \
    mainwindow.h \
    networkmanager.h \
    joinwindow.h \
    rtpclient.h

FORMS += \
    chat.ui \
    loginwindow.ui \
    mainwindow.ui \
    joinwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
