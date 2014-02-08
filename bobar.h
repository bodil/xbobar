#include <QWebInspector>

class API;
class Page;

class BobarWindow : public QWidget {
  Q_OBJECT

public:
  explicit BobarWindow(const QString& path, QWidget* parent = 0);
  ~BobarWindow();

  void resize(int height, Qt::Edge edge);
  void openDebugger();

  Page* page;
  QWebView* webView;
  QWebInspector* debugger;
};

class Page : public QWebPage {
  Q_OBJECT

public:
  explicit Page(BobarWindow* parent, const QString& path);

  void loadFile(const QString& path);

  QString programPath;
  BobarWindow* window;
  API* api;

public slots:
  void restart();
  void onReloaded();
  void onLoadFinished();
};
