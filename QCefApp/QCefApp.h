#ifndef QCEFAPP_H
#define QCEFAPP_H

#include "QCefClient/QCefClient.h"
#include <include/cef_app.h>
#include <QList>
#include <QSslCertificate>
#include <QQueue>


class QCefApp:public CefApp,public CefBrowserProcessHandler
{
public:
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override;
    virtual void OnContextInitialized() override;
    virtual void OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line) override;
public:
    QCefApp(){}
    virtual ~QCefApp(){}

    /*创建浏览器进程的工厂方法*/
    CefRefPtr<QCefClient> addBrowser(QList<QSslCertificate> caCerts = QList<QSslCertificate>());

    /*关闭所有浏览器进程*/
    void closeAllBrowser();

private:
    bool m_contextReady;
    QQueue<CefRefPtr<QCefClient>> m_clients;
    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(QCefApp);
};

#endif //QCEFAPP_H
