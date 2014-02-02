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

class API : public QObject {
  Q_OBJECT

public:
  explicit API(Page* parent, BobarWindow* window);

  Q_INVOKABLE QVariant readFileSync(const QString& path);
  Q_INVOKABLE QVariant searchRequirePath(const QString& base, const QString& file);
  Q_INVOKABLE QString loadUrl(const QString& url);
  Q_INVOKABLE void showFrame();
  Q_INVOKABLE void resizeFrame(int height, const QString& edge);
  Q_INVOKABLE void restart();

  BobarWindow* window;
  Page* page;

public slots:
  void onNetworkRequestFinished(QNetworkReply* reply);

signals:
  void completedLoadUrl(const QString& id, const QVariant& data);
};
