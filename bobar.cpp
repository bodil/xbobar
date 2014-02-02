#include <QApplication>
#include <QCommandLineParser>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QMetaEnum>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUuid>
#include <QWebFrame>
#include <QX11Info>

#include <iostream>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <signal.h>

#include "bobar.h"
#include "networkmanager.h"

void x11SetProp(const QWidget* win, const QString& type, int value) {
  Atom atom(XInternAtom(QX11Info::display(), type.toUtf8().constData(), False));
  XChangeProperty(QX11Info::display(), win->winId(), atom, XA_CARDINAL, 32,
                  PropModeReplace, (unsigned char*)&value, 1);
}

void x11SetProp(const QWidget* win, const QString& type, const QString& value) {
  Atom atom(XInternAtom(QX11Info::display(), type.toUtf8().constData(), False));
  Atom val(XInternAtom(QX11Info::display(), value.toUtf8().constData(), False));
  XChangeProperty(QX11Info::display(), win->winId(), atom, XA_ATOM, 32,
                  PropModeReplace, (unsigned char*)&val, 1);
}

void x11SetProp(const QWidget* win, const QString& type, const QVector<long>& value) {
  Atom atom(XInternAtom(QX11Info::display(), type.toUtf8().constData(), False));
  XChangeProperty(QX11Info::display(), win->winId(), atom, XA_CARDINAL, 32,
                  PropModeReplace, (unsigned char*)value.constData(),
                  value.count());
}

QString readFile(const QString& path) {
  QFile file(path);
  if (!file.exists()) qFatal("ERROR: file not found: %s", path.toUtf8().data());
  if (!file.open(QIODevice::ReadOnly)) {
    qFatal("ERROR: %s", file.errorString().toUtf8().data());
  }
  QByteArray result(file.readAll());
  return QString(result);
}

API::API(Page* parent, BobarWindow* window)
  : QObject(parent)
  , window(window)
  , page(parent)
{
  setObjectName("bobar");
  new NetworkManager(this);
}

QVariant API::searchRequirePath(const QString& base, const QString& file) {
  QFileInfo info(file);
  if (info.fileName() == file) {
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
  QString uuid(QUuid().toString());
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

Page::Page(BobarWindow* parent, const QString& path)
  : QWebPage(parent)
  , programPath(path)
  , window(parent)
  , api(new API(this, parent))
{
  connect(mainFrame(), &QWebFrame::javaScriptWindowObjectCleared,
          this, &Page::onReloaded);
  connect(mainFrame(), &QWebFrame::loadFinished,
          this, &Page::onLoadFinished);
  mainFrame()->setScrollBarPolicy(Qt::Vertical, Qt::ScrollBarAlwaysOff);
  mainFrame()->setScrollBarPolicy(Qt::Horizontal, Qt::ScrollBarAlwaysOff);
  restart();
}

void Page::restart() {
  mainFrame()->setUrl(QUrl("qrc:///lib/index.html"));
}

void Page::onLoadFinished() {
  qDebug("onLoadFinished");
  mainFrame()->evaluateJavaScript(readFile(":/lib/prelude.js"));
  mainFrame()->evaluateJavaScript(readFile(":/lib/react.js"));
  mainFrame()->evaluateJavaScript(QString("window.requireFile(\"%1\");").arg(programPath));
}

void Page::onReloaded() {
  qDebug("onReloaded");
  mainFrame()->addToJavaScriptWindowObject("__bobar", api);
}

BobarWindow::BobarWindow(const QString& path, QWidget* parent)
  : QWidget(parent,
            Qt::NoDropShadowWindowHint |
            Qt::WindowDoesNotAcceptFocus |
            Qt::FramelessWindowHint)
  , page(new Page(this, path))
  , webView(new QWebView(this))
{
  webView->setPage(page);
  QHBoxLayout* l = new QHBoxLayout;
  l->setContentsMargins(0, 0, 0, 0);
  l->addWidget(webView);
  setLayout(l);

  x11SetProp(this, "_NET_WM_DESKTOP", -1);
  x11SetProp(this, "_NET_WM_WINDOW_TYPE", "_NET_WM_WINDOW_TYPE_DOCK");
}

BobarWindow::~BobarWindow() {
  x11SetProp(this, "_NET_WM_STRUT_PARTIAL", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
}

void BobarWindow::resize(int height, Qt::Edge edge) {
  QDesktopWidget* desktop(QApplication::desktop());
  QRect dg(desktop->screenGeometry());
  QRect g;

  switch (edge) {
  case Qt::LeftEdge:
    g = QRect(0, 0, height, dg.height());
    x11SetProp(this, "_NET_WM_STRUT_PARTIAL",
               {(uint)height, 0, 0, 0, 0, (uint)dg.height(), 0, 0, 0, 0, 0, 0});
    break;
  case Qt::RightEdge:
    g = QRect(dg.width() - height, 0, height, dg.height());
    x11SetProp(this, "_NET_WM_STRUT_PARTIAL",
               {0, (uint)height, 0, 0, 0, 0, 0, (uint)dg.height(), 0, 0, 0, 0});
    break;
  case Qt::TopEdge:
    g = QRect(0, 0, dg.width(), height);
    x11SetProp(this, "_NET_WM_STRUT_PARTIAL",
               {0, 0, (uint)height, 0, 0, 0, 0, 0, 0, (uint)dg.width(), 0, 0});
    break;
  case Qt::BottomEdge:
    g = QRect(0, dg.height() - height, dg.width(), height);
    x11SetProp(this, "_NET_WM_STRUT_PARTIAL",
               {0, 0, 0, (uint)height, 0, 0, 0, 0, 0, 0, 0, (uint)dg.width()});
    break;
  }

  setGeometry(g);
  show();
}

void BobarWindow::openDebugger() {
  debugger = new QWebInspector;
  debugger->setPage(page);
  debugger->show();
}

static void sigint_handler(int) {
  QApplication::instance()->exit(0);
}

static void mysignal(int sig, void (*handler)(int)) {
  struct sigaction sa;
  sa.sa_handler = handler;
  sa.sa_flags = 0;
  sigaction(sig, &sa, 0);
}

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);
  QApplication::setApplicationName("Bobar");
  QApplication::setApplicationVersion("0.1");
  mysignal(SIGINT, sigint_handler);

  QWebSettings* web = QWebSettings::globalSettings();
  web->setAttribute(QWebSettings::LocalStorageEnabled, true);
  web->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
  web->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
  web->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
  web->setAttribute(QWebSettings::JavascriptCanCloseWindows, true);
  web->setAttribute(QWebSettings::JavascriptCanAccessClipboard, true);
  web->setAttribute(QWebSettings::CSSGridLayoutEnabled, true);

  QCommandLineParser opts;
  opts.setApplicationDescription("A Webkit enabled system status bar.");
  opts.addHelpOption();
  opts.addVersionOption();
  QCommandLineOption debugOption({"d", "debug"}, "Opens the Web Inspector.");
  opts.addOption(debugOption);
  opts.addPositionalArgument("program-file", "A Javascript program file to evaluate.");
  opts.process(a);

  QStringList args(opts.positionalArguments());
  if (!args.count()) {
    std::cout << "ERROR: No program file specified.\n\n";
    std::cout << opts.helpText().toUtf8().data();
    return 1;
  }

  BobarWindow b(args.at(0));

  if (opts.isSet(debugOption)) {
    web->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
    b.openDebugger();
  }

  b.page->restart();

  return a.exec();
}
