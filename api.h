#include <QObject>
#include <QNetworkReply>

class BobarWindow;
class Page;

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
