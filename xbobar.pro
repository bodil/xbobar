
QT += core widgets webkitwidgets x11extras dbus
CONFIG += debug link_pkgconfig

DESTDIR = build
TARGET = xbobar
JSSOURCES = lib/fs.js lib/dom.js lib/dbus.js lib/networkmanager.js lib/prelude.js
HEADERS += api.h bobar.h dbus.h uuid.h x11.h
SOURCES += api.cpp bobar.cpp dbus.cpp uuid.cpp x11.cpp
RESOURCES += xbobar.qrc
LIBS += -lX11
PKGCONFIG += dbus-1
QMAKE_CXXFLAGS += -std=gnu++11

jsdoc.depends = $${JSSOURCES}
jsdoc.commands = jsdoc -d doc $${JSSOURCES}
QMAKE_EXTRA_TARGETS += jsdoc
