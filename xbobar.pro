
QT += core widgets webkitwidgets x11extras dbus
CONFIG += debug link_pkgconfig

DESTDIR = build
TARGET = xbobar
HEADERS += bobar.h dbus.h networkmanager.h
SOURCES += bobar.cpp dbus.cpp networkmanager.cpp
RESOURCES += xbobar.qrc
LIBS += -lX11
# PKGCONFIG += NetworkManagerQt
QMAKE_CXXFLAGS += -std=gnu++11
