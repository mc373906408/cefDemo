#include "QCefRenderHandler/QCefRenderHandler.h"
#include <QDebug>


CefRefPtr<CefRenderProcessHandler> QCefRenderHandler::GetRenderProcessHandler()
{
    return this;
}

void QCefRenderHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    // 取回V8上下文的window对象
    CefRefPtr<CefV8Value> object = context->GetGlobal();

    // 创建"sendMessage"函数，作为消息通道使用.
    m_v8Handler = CefRefPtr<QCefV8Handler>(new QCefV8Handler(browser));
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("sendMessage", m_v8Handler);

    // 将消息通道注册到window对象
    bool b=object->SetValue("sendMessage", func, V8_PROPERTY_ATTRIBUTE_NONE);
}
