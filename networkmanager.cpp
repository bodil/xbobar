#include "dbus.h"
#include "networkmanager.h"

NetworkManager::NetworkManager(QObject* parent)
  : QObject(parent)
{
  setObjectName("NetworkManager");
}

QVariantMap getAccessPointInfo(const QDBusObjectPath& path) {
  QVariantMap m = getProperties(path, "org.freedesktop.NetworkManager.AccessPoint");
  if (m.contains("Ssid"))
    m.insert("Ssid", QString(m["Ssid"].toByteArray()));
  return m;
}

QVariant getAccessPoints(const QDBusObjectPath& path) {
  QDBusInterface i("org.freedesktop.NetworkManager", path.path(),
                   "org.freedesktop.NetworkManager.Device.Wireless",
                   QDBusConnection::systemBus());
  QDBusReply<QList<QDBusObjectPath>> r = i.call("GetAccessPoints");
  if (r.isValid()) {
    QVariantList l;
    QListIterator<QDBusObjectPath> i(r.value());
    while (i.hasNext())
      // l << i.next().path();
      l << getAccessPointInfo(i.next());
    return l;
  } else {
    return dbusError(r.error());
  }
}

QVariant getDeviceInfo(const QDBusObjectPath& path) {
  QVariantMap m(getProperties(path, "org.freedesktop.NetworkManager.Device"));
  m = mergeProperties(m, path, "org.freedesktop.NetworkManager.Device.Wired");
  m = mergeProperties(m, path, "org.freedesktop.NetworkManager.Device.Wireless");
  m = mergeProperties(m, path, "org.freedesktop.NetworkManager.Device.Cdma");
  m = mergeProperties(m, path, "org.freedesktop.NetworkManager.Device.Gsm");
  m = mergeProperties(m, path, "org.freedesktop.NetworkManager.Device.Serial");
  m = mergeProperties(m, path, "org.freedesktop.NetworkManager.Device.Bluetooth");
  m = mergeProperties(m, path, "org.freedesktop.NetworkManager.Device.OlpcMesh");
  m = mergeProperties(m, path, "org.freedesktop.NetworkManager.Device.Modem");

  QVariant acc(getAccessPoints(path));
  if (!acc.toMap().contains("error"))
    m.insert("AccessPoints", acc);
  return m;
}

QVariant NetworkManager::getDevices() {
  QDBusInterface i("org.freedesktop.NetworkManager",
                   "/org/freedesktop/NetworkManager",
                   "org.freedesktop.NetworkManager",
                   QDBusConnection::systemBus());
  QDBusReply<QList<QDBusObjectPath>> r = i.call("GetDevices");
  if (r.isValid()) {
    QVariantList l;
    QListIterator<QDBusObjectPath> i(r.value());
    while (i.hasNext())
      l << getDeviceInfo(i.next());
    return l;
  } else {
    return dbusError(r.error());
  }
}
