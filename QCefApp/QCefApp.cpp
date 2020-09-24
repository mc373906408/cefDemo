#include "QCefApp/QCefApp.h"
#include <QCoreApplication>
#include <QDebug>


CefRefPtr<CefBrowserProcessHandler> QCefApp::GetBrowserProcessHandler()
{
    return this;
}
void QCefApp::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();
    m_contextReady=true;
}

void QCefApp::OnBeforeCommandLineProcessing(const CefString &process_type, CefRefPtr<CefCommandLine> command_line)
{
    /*浏览器设置*/
//    command_line->AppendSwitchWithValue("--force-device-scale-factor","2");
//    command_line->AppendSwitch();
//    command_line->AppendSwitchWithValue("type","renderer");
}

CefRefPtr<QCefClient> QCefApp::addBrowser(QList<QSslCertificate> caCerts)
{
    if(m_contextReady)
    {
        // 创建本地窗口所需的信息
        CefWindowInfo windowInfo;

#if defined(OS_WIN)
        // 针对Windows系统，需要指定特殊的标识，
        // 这个标识会被传递给CreateWindowEx()方法
        windowInfo.SetAsPopup(NULL, "QCefView");
        /*设置窗口风格，初始值不可见，指定父窗口后设置可见*/
        windowInfo.style=WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
#endif
        // 初始化cef client方法
        CefRefPtr<QCefClient> client(new QCefClient());
//        client->setCaCerts(caCerts);
        // 指定CEF浏览器设置
        CefBrowserSettings browserSettings;
//        std::string url = "html/";
        QString url=QCoreApplication::applicationDirPath()+"/res/html/404.html";
        qDebug()<<url;
        // 创建浏览器窗口
        CefBrowserHost::CreateBrowser(windowInfo, client.get(), url.toStdString(), browserSettings, NULL,CefRequestContext::GetGlobalContext());
        // 将浏览器引用添加到浏览器队列
        m_clients.enqueue(client);
        return client;
    }
    return nullptr;
}

void QCefApp::closeAllBrowser()
{
    while (!m_clients.empty())
    {
        m_clients.dequeue()->browser()->GetHost()->CloseBrowser(true);
    }
}
