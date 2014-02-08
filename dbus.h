#include <QObject>
#include <QtDBus>

namespace DBus {

  class CallReceiver : public QObject {
    Q_OBJECT

  public:
    CallReceiver(QObject* parent = 0);

  public slots:
    void onSuccess(const QDBusMessage& result);
    void onError(const QDBusError& error);

  signals:
    void done(QVariant error, QVariant result);
  };



  class Interface : public QObject {
    Q_OBJECT

  public:
    Interface(const QDBusConnection& connection, const QString& name, QObject* parent);

    Q_INVOKABLE QObject* call(const QString& service, const QString& path,
                              const QString& interface, const QString& method,
                              const QList<QVariant>& args);

  private:
    QDBusConnection connection;
  };

};
