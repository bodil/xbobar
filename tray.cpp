#include <QX11Info>

#include "tray.h"

Tray::Tray(QObject* parent)
  : QObject(parent)
{
  window = new QWindow();
}

Tray::~Tray() {
  window->deleteLater();
}

QVariant Tray::grab() {
  QString selection(QString("_NET_SYSTEM_TRAY_S%1").arg(QX11Info::appScreen()));
  X11::setSelectionOwner(selection, window);
  return isTray();
}

QVariant Tray::isTray() {
  QString selection(QString("_NET_SYSTEM_TRAY_S%1").arg(QX11Info::appScreen()));
  return X11::isSelectionOwner(selection, window);
}
