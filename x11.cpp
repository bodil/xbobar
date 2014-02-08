#include <QX11Info>

#include "x11.h"

namespace X11 {

  void setProp(const QWidget* win, const QString& type, int value) {
    Atom atom(XInternAtom(QX11Info::display(), type.toUtf8().constData(), False));
    XChangeProperty(QX11Info::display(), win->winId(), atom, XA_CARDINAL, 32,
                    PropModeReplace, (unsigned char*)&value, 1);
  }

  void setProp(const QWidget* win, const QString& type, const QString& value) {
    Atom atom(XInternAtom(QX11Info::display(), type.toUtf8().constData(), False));
    Atom val(XInternAtom(QX11Info::display(), value.toUtf8().constData(), False));
    XChangeProperty(QX11Info::display(), win->winId(), atom, XA_ATOM, 32,
                    PropModeReplace, (unsigned char*)&val, 1);
  }

  void setProp(const QWidget* win, const QString& type, const QVector<long>& value) {
    Atom atom(XInternAtom(QX11Info::display(), type.toUtf8().constData(), False));
    XChangeProperty(QX11Info::display(), win->winId(), atom, XA_CARDINAL, 32,
                    PropModeReplace, (unsigned char*)value.constData(),
                    value.count());
  }

};
