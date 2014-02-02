#include <QtDBus>

QVariantMap dbusError(QDBusError e);
QVariantMap getProperties(const QDBusObjectPath& path, const QString& interface);
QVariantMap mergeProperties(const QVariantMap& m, const QDBusObjectPath& path, const QString& interface);
