#include "dbus.h"
#include "uuid.h"

namespace DBus {

  QVariant unpack(const QDBusMessage& m) {
    QString sig(m.signature());
    qDebug() << "DBus::unpack: converting signature " << sig;
    if (sig == "a{sv}") {
      QDBusReply<QVariantMap> r(m);
      return r.value();
    } else if (sig == "ao") {
      QDBusReply<QList<QDBusObjectPath>> r(m);
      QListIterator<QDBusObjectPath> i(r.value());
      QVariantList out;
      while (i.hasNext())
        out << i.next().path();
      return out;
    } else {
      qFatal("DBus::unpack: unhandled signature \'%s\'", qPrintable(sig));
    }
  }

  QVariantMap dbusError(const QDBusError& e) {
    return QVariantMap({{"type", QDBusError::errorString(e.type())},
          {"name", e.name()},
            {"message", e.message()}});
  }

  // QVariantMap getProperties(const QDBusObjectPath& path, const QString& interface) {
  //   QDBusInterface i("org.freedesktop.NetworkManager",
  //                    path.path(),
  //                    "org.freedesktop.DBus.Properties",
  //                    QDBusConnection::systemBus());
  //   QDBusReply<QVariantMap> r = i.call("GetAll", interface);
  //   if (r.isValid()) {
  //     QVariantMap m(r.value());
  //     m.insert("ObjectPath", path.path());
  //     return m;
  //   } else {
  //     return dbusError(r.error());
  //   }
  // }

  // QVariantMap mergeProperties(const QVariantMap& m, const QDBusObjectPath& path, const QString& interface) {
  //   QVariantMap nm(getProperties(path, interface));
  //   return !nm.contains("error") ? nm.unite(m) : m;
  // }

  Interface::Interface(const QDBusConnection& connection, const QString& name,
                       QObject* parent)
    : QObject(parent)
    , connection(connection)
  {
    setObjectName(name);
  }

  QObject* Interface::call(const QString& service, const QString& path,
                           const QString& interface, const QString& method,
                           const QList<QVariant>& args)
  {
    QDBusInterface i(service, path, interface, connection);
    CallReceiver* r = new CallReceiver(this);
    i.callWithCallback(method, args, r, SLOT(onSuccess(const QDBusMessage&)),
                       SLOT(onError(const QDBusError&)));
    return r;
  }

  CallReceiver::CallReceiver(QObject* parent)
    : QObject(parent)
  {}

  void CallReceiver::onSuccess(const QDBusMessage& result) {
    emit done(QVariant(), unpack(result));
    deleteLater();
  }

  void CallReceiver::onError(const QDBusError& error) {
    emit done(dbusError(error), QVariant());
    deleteLater();
  }

};
