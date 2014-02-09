#include <QObject>
#include <QWindow>
#include <QVariant>

#include "x11.h"

class Tray : public QObject {
  Q_OBJECT

public:
  Tray(QObject* parent = 0);
  ~Tray();

  Q_INVOKABLE QVariant grab();
  Q_INVOKABLE QVariant isTray();

private:
  QWindow* window;

};
