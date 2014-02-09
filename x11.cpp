#include <QX11Info>

#include "x11.h"

namespace X11 {

  Atom internAtom(const QString& name) {
    return XInternAtom(QX11Info::display(), name.toUtf8().constData(), False);
  }

  void setProp(const QWidget* win, const QString& type, int value) {
    XChangeProperty(QX11Info::display(), win->winId(), internAtom(type), XA_CARDINAL,
                    32, PropModeReplace, (unsigned char*)&value, 1);
  }

  void setProp(const QWidget* win, const QString& type, const QString& value) {
    Atom val(internAtom(value));
    XChangeProperty(QX11Info::display(), win->winId(), internAtom(type), XA_ATOM,
                    32, PropModeReplace, (unsigned char*)&val, 1);
  }

  void setProp(const QWidget* win, const QString& type, const QVector<long>& value) {
    XChangeProperty(QX11Info::display(), win->winId(), internAtom(type), XA_CARDINAL,
                    32, PropModeReplace, (unsigned char*)value.constData(),
                    value.count());
  }

  void setSelectionOwner(const QString& selection, const QWindow* owner) {
    XSetSelectionOwner(QX11Info::display(), internAtom(selection), owner->winId(),
                       QX11Info::appTime());
  }

  QWindow* getSelectionOwner(const QString& selection) {
    return QWindow::fromWinId(XGetSelectionOwner(QX11Info::display(),
                                                 internAtom(selection)));
  }

  bool isSelectionOwner(const QString& selection, const QWindow* window) {
    return getSelectionOwner(selection)->winId() == window->winId();
  }

};
