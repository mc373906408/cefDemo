#ifndef QCEFVIEW_H
#define QCEFVIEW_H

#include <QTimer>
#include <QWindow>
#include <include/internal/cef_ptr.h>

#include "QCefClient/QCefClient.h"


class QCefView:public QObject
{
    Q_OBJECT
public:
    QCefView(CefRefPtr<QCefClient> cefClient,QWindow *window);
    virtual ~QCefView(){}

    void load(QUrl url);
    void setresize();
    void sendToWeb(QString msg);

signals:
    void cefEmbedded();
    void sgLoad(CefString url);
    void webMsgReceived(QString msg);

private:
    void runJavaScript(QString script);
protected slots:
    virtual void onCefTimer();

private:
    bool m_cefEmbedded=false;
    CefRefPtr<QCefClient> m_cefClient;
    QTimer *m_cefTimer;
    QWindow *m_window;
    QWindow* subW;
};

#endif //QCEFVIEW_H
