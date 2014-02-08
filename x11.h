#include <QWidget>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

namespace X11 {
  void setProp (const QWidget* win, const QString& type, int value);
  void setProp (const QWidget* win, const QString& type, const QString& value);
  void setProp (const QWidget* win, const QString& type, const QVector<long>& value);
};
