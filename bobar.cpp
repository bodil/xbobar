#include <QApplication>
#include <QCommandLineParser>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QWebFrame>
#include <QFile>

#include <iostream>

#include <signal.h>

#include "bobar.h"
#include "api.h"
#include "x11.h"

QString readFile(const QString& path) {
  QFile file(path);
  if (!file.exists()) qFatal("ERROR: file not found: %s", path.toUtf8().data());
  if (!file.open(QIODevice::ReadOnly)) {
    qFatal("ERROR: %s", file.errorString().toUtf8().data());
  }
  QByteArray result(file.readAll());
  return QString(result);
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

  X11::setProp(this, "_NET_WM_DESKTOP", -1);
  X11::setProp(this, "_NET_WM_WINDOW_TYPE", "_NET_WM_WINDOW_TYPE_DOCK");
}

BobarWindow::~BobarWindow() {
  X11::setProp(this, "_NET_WM_STRUT_PARTIAL", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
}

void BobarWindow::resize(int height, Qt::Edge edge) {
  QDesktopWidget* desktop(QApplication::desktop());
  QRect dg(desktop->screenGeometry());
  QRect g;

  switch (edge) {
  case Qt::LeftEdge:
    g = QRect(0, 0, height, dg.height());
    X11::setProp(this, "_NET_WM_STRUT_PARTIAL",
               {(uint)height, 0, 0, 0, 0, (uint)dg.height(), 0, 0, 0, 0, 0, 0});
    break;
  case Qt::RightEdge:
    g = QRect(dg.width() - height, 0, height, dg.height());
    X11::setProp(this, "_NET_WM_STRUT_PARTIAL",
               {0, (uint)height, 0, 0, 0, 0, 0, (uint)dg.height(), 0, 0, 0, 0});
    break;
  case Qt::TopEdge:
    g = QRect(0, 0, dg.width(), height);
    X11::setProp(this, "_NET_WM_STRUT_PARTIAL",
               {0, 0, (uint)height, 0, 0, 0, 0, 0, 0, (uint)dg.width(), 0, 0});
    break;
  case Qt::BottomEdge:
    g = QRect(0, dg.height() - height, dg.width(), height);
    X11::setProp(this, "_NET_WM_STRUT_PARTIAL",
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
  QApplication::setOrganizationName("bodil.org");
  QApplication::setOrganizationDomain("bodil.org");
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
