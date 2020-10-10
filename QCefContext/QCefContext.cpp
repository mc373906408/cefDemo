#include "QCefContext/QCefContext.h"
#include <QDebug>
#include <QCoreApplication>
#include <direct.h>

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
//    qDebug()<<"11111";
    CefRefPtr<CefApp> app(new QCefApp);
    // 创建一个正确类型的App Client

//        app = new QCefApp();
    m_cefApp = CefRefPtr<QCefApp>((QCefApp*)app.get());

    CefSettings settings;
    settings.log_severity = LOGSEVERITY_DISABLE;
    settings.remote_debugging_port=7777;

    QString dir(_getcwd(NULL,0));
    CefString(&settings.browser_subprocess_path).FromASCII(QString(dir+"/release/subprocess").toStdString().c_str());

//    else{
//        /*创建渲染进程处理器，用于H5通讯*/
//        CefString procType = m_cmdLine->GetSwitchValue("type");
//        if (procType == "renderer")
//        {
//            qDebug()<<"111111";
//            app = new QCefRenderHandler();
//            m_cefRenderer = CefRefPtr<QCefRenderHandler>((QCefRenderHandler*)app.get());
//        }
//    }


//    int result = CefExecuteProcess(mainArgs, app, NULL);
//    if (result >= 0)
//    {
//        return result;
//    }



    // 初始化CEF
    CefInitialize(mainArgs, settings, app.get(), NULL);

    return -1;
}
