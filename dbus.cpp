#include <QtDBus>

#include "networkmanager.h"

QVariantMap dbusError(QDBusError e) {
  QVariantMap error({{"type", QDBusError::errorString(e.type())},
        {"name", e.name()},
          {"message", e.message()}});
  return QVariantMap({{"error", error}});
}

QVariantMap getProperties(const QDBusObjectPath& path, const QString& interface) {
  QDBusInterface i("org.freedesktop.NetworkManager",
                   path.path(),
                   "org.freedesktop.DBus.Properties",
                   QDBusConnection::systemBus());
  QDBusReply<QVariantMap> r = i.call("GetAll", interface);
  if (r.isValid()) {
    QVariantMap m(r.value());
    m.insert("ObjectPath", path.path());
    return m;
  } else {
    return dbusError(r.error());
  }
}

QVariantMap mergeProperties(const QVariantMap& m, const QDBusObjectPath& path, const QString& interface) {
  QVariantMap nm(getProperties(path, interface));
  return !nm.contains("error") ? nm.unite(m) : m;
}
