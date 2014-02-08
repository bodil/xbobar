#include <QDir>
#include <QMetaEnum>

#include "api.h"
#include "bobar.h"
#include "uuid.h"
#include "dbus.h"

API::API(Page* parent, BobarWindow* window)
  : QObject(parent)
  , window(window)
  , page(parent)
{
  setObjectName("bobar");
  QObject* dbus = new QObject(this);
  dbus->setObjectName("DBus");
  new DBus::Interface(DBUS_BUS_SYSTEM, "systemBus", dbus);
  new DBus::Interface(DBUS_BUS_SESSION, "sessionBus", dbus);
}

bool isFilePath(const QString& path) {
  return path.startsWith("./")
    || path.startsWith("../")
    || path.startsWith("/");
}

QVariant API::searchRequirePath(const QString& base, const QString& file) {
  if (!isFilePath(file)) {
    QDir res(":/lib");
    if (res.exists(file)) return res.absoluteFilePath(file);
    if (res.exists(file + ".js")) return res.absoluteFilePath(file + ".js");
    return QVariant();
  }
  QDir baseDir(base);
  if (baseDir.exists(file)) return baseDir.absoluteFilePath(file);
  if (baseDir.exists(file + ".js")) return baseDir.absoluteFilePath(file + ".js");
  return QVariant();
}

QVariant API::readFileSync(const QString& path) {
  QFile file(path);
  if (!file.exists())
    return QVariantMap({{"error", QString("file not found: %1").arg(path)}});
  if (!file.open(QIODevice::ReadOnly))
    return QVariantMap({{"error", file.errorString()}});
  return QVariantMap({{"error", QVariant()},
        {"result", QString(file.readAll())},
          {"path", QFileInfo(file).absoluteFilePath()},
            {"dir", QFileInfo(file).absolutePath()}});
}

QString API::loadUrl(const QString& url) {
  QString uuid(UUID::uuid());
  QNetworkReply* reply = page->networkAccessManager()->get(QNetworkRequest(QUrl(url)));
  reply->setProperty("x-uuid", uuid);
  connect(page->networkAccessManager(), &QNetworkAccessManager::finished,
          this, &API::onNetworkRequestFinished);
  return uuid;
}

void API::onNetworkRequestFinished(QNetworkReply* reply) {
  QString uuid(reply->property("x-uuid").toString());
  if (reply->error() != QNetworkReply::NoError) {
    QMetaObject mo(QNetworkReply::staticMetaObject);
    QMetaEnum e(mo.enumerator(mo.indexOfEnumerator("NetworkError")));
    QString error(e.valueToKey(reply->error()));
    emit completedLoadUrl(uuid, QVariantMap({{"error", error}}));
  } else {
    QString data(reply->readAll());
    emit completedLoadUrl(uuid, QVariantMap({{"error", QVariant()},
            {"result", data}}));
  }
}

void API::showFrame() {
  window->resize(0, Qt::TopEdge);
}

void API::resizeFrame(int height, const QString& edgeS) {
  Qt::Edge edge;
  if (edgeS == "top") edge = Qt::TopEdge;
  else if (edgeS == "bottom") edge = Qt::BottomEdge;
  else if (edgeS == "left") edge = Qt::LeftEdge;
  else if (edgeS == "right") edge = Qt::RightEdge;
  window->resize(height, edge);
}

void API::restart() {
  page->restart();
}
