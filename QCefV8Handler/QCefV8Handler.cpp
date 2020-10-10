#include "QCefV8Handler/QCefV8Handler.h"
#include <QDebug>
#include <QFile>

QCefV8Handler::QCefV8Handler(CefRefPtr<CefBrowser> browser)
{
    m_browser=browser;
}

bool QCefV8Handler::Execute(const CefString &name, CefRefPtr<CefV8Value> object, const CefV8ValueList &arguments, CefRefPtr<CefV8Value> &retval, CefString &exception)
{
    if (name=="sendMessage")
    {
        if (arguments.size() == 1)
        {
            //这里收到了H5的请求
            CefString msgStr = arguments.at(0)->GetStringValue();
//            QFile file("D:/test.txt");
//            file.open(QIODevice::WriteOnly|QIODevice::Text);
//            QTextStream out(&file);
//            out<<msgStr.ToString().c_str();
            //消息会被发送到CefClient的OnProcessMessageReceived接口方法
            m_browser->GetMainFrame()->SendProcessMessage(PID_BROWSER, CefProcessMessage::Create(msgStr));
            retval = CefV8Value::CreateInt(0);
        }
        return true;
    }
    return false;
}
