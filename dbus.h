#include <QObject>
#include <QVariant>

#include "dbus/dbus.h"

namespace DBus {

  class ByteBuffer : public QObject {
    Q_OBJECT

  public:
    ByteBuffer(const QByteArray& array, QObject* parent = 0);

    const char* const* constData() const;

  private:
    QByteArray array;
    const char* data;
  };

  class MethodCall : public QObject {
    Q_OBJECT

  public:
    MethodCall(DBusConnection* connection,
               const QString& dest,
               const QString& path,
               const QString& iface,
               const QString& method,
               const QVariantList& args,
               QObject* parent = 0);
    ~MethodCall();

    bool invoke();
    void onComplete();

  signals:
    void done(QVariant error, QVariant result);

  private:
    DBusConnection* connection;
    DBusMessage* msg;
    DBusPendingCall* pending;
    DBusMessage* reply;

    void marshalIter(DBusMessageIter* iter, const QVariant& arg);
    void marshalArgs(DBusMessage* msg, const QVariantList& args);
  };

  class Interface : public QObject {
    Q_OBJECT

  public:
    Interface(DBusBusType bus, const QString& name, QObject* parent);
    ~Interface();

    Q_INVOKABLE QObject* call(const QString& dest,
                              const QString& path,
                              const QString& iface,
                              const QString& method,
                              const QVariantList& args);

    // Q_INVOKABLE QObject* call(const QString& service, const QString& path,
    //                           const QString& interface, const QString& method,
    //                           const QList<QVariant>& args);

  protected:
    void timerEvent(QTimerEvent*);

  private:
    DBusConnection* connection;
  };

};
