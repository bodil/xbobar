#include <QObject>
#include <QVariant>

class NetworkManager : public QObject {
  Q_OBJECT

public:
  NetworkManager(QObject* parent = 0);

  Q_INVOKABLE QVariant getDevices();
};
