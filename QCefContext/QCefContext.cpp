#include "QCefContext/QCefContext.h"
#include <QDebug>

QCefContext::QCefContext(CefSettings* settings)
{
    m_settings=settings;
}

int QCefContext::initCef(int argc, char *argv[])
{
    // 创建CEF默认命令行参数.
    m_cmdLine = CefCommandLine::CreateCommandLine();
#if defined(OS_WIN)
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
    CefMainArgs mainArgs(hInstance);

    m_cmdLine->InitFromString(::GetCommandLineW());
    //    m_cmdLine->InitFromString("--force-device-scale-factor 1");
#endif
    //    m_cmdLine->AppendSwitch("type");
    //    m_cmdLine->AppendSwitchWithValue("type","renderer");
    return initCef(mainArgs);
}

int QCefContext::initCef(CefMainArgs& mainArgs)
{
    qDebug()<<"11111";
    CefRefPtr<CefApp> app;
    // 创建一个正确类型的App Client
    if (!m_cmdLine->HasSwitch("type"))
    {
        app = new QCefApp();
        m_cefApp = CefRefPtr<QCefApp>((QCefApp*)app.get());
    }
    else{
        /*创建渲染进程处理器，用于H5通讯*/
        CefString procType = m_cmdLine->GetSwitchValue("type");
        if (procType == "renderer")
        {
            qDebug()<<"111111";
            app = new QCefRenderHandler();
            m_cefRenderer = CefRefPtr<QCefRenderHandler>((QCefRenderHandler*)app.get());
        }
    }


    int result = CefExecuteProcess(mainArgs, app, NULL);
    if (result >= 0)
    {
        return result;
    }



    // 初始化CEF
    CefInitialize(mainArgs, *m_settings, app.get(), NULL);

    return -1;
}
