#include "QCefClient/QCefClient.h"
#include <QDebug>
#include <QFile>

CefRefPtr<CefLifeSpanHandler> QCefClient::GetLifeSpanHandler()
{
    return this;
}

void QCefClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    CEF_REQUIRE_UI_THREAD();
    m_browser=browser;
    m_created=true;
}

void QCefClient::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type)
{
    CEF_REQUIRE_UI_THREAD();
}

bool QCefClient::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString &target_url, const CefString &target_frame_name, WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures &popupFeatures, CefWindowInfo &windowInfo, CefRefPtr<CefClient> &client, CefBrowserSettings &settings, CefRefPtr<CefDictionaryValue> &extra_info, bool *no_javascript_access)
{
    /*禁止弹出浏览器，都在当页显示*/
    switch (target_disposition) {
    case WOD_NEW_FOREGROUND_TAB:
    case WOD_NEW_BACKGROUND_TAB:
    case WOD_NEW_POPUP:
    case WOD_NEW_WINDOW:
        browser->GetMainFrame()->LoadURL(target_url);
        return true;
    }
    return false;
}

bool QCefClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    //收到来自Render进程发过来的信息
    emit webMsgReceived(QString(message->GetName().ToString().c_str()));
    return true;
}

CefRefPtr<CefBrowser> QCefClient::browser()
{
    return m_browser;
}

CefWindowHandle QCefClient::browserWinId()
{
    if(m_created)
    {
        return browser()->GetHost()->GetWindowHandle();
    }
    return (CefWindowHandle)-1;
}

void QCefClient::load(CefString url)
{
    browser()->GetMainFrame()->LoadURL(url);
    m_url=url;
}

void QCefClient::reload()
{
    browser()->ReloadIgnoreCache();
}
