#include "QCefClient.h"
#include "QCefView.h"
#include <QDebug>
#include <QFile>
#include "log/TSpdlog.h"




QCefClient::QCefClient(QCefView *cefApp)
{
    if(cefApp){
        QObject::connect(this,&QCefClient::sgCloseBrowser,cefApp,&QCefView::onCloseBrowser);
        QObject::connect(this,&QCefClient::sgSendCookie,cefApp,&QCefView::onSendCookie);
    }
}

CefRefPtr<CefLifeSpanHandler> QCefClient::GetLifeSpanHandler()
{
    return this;
}

CefRefPtr<CefKeyboardHandler> QCefClient::GetKeyboardHandler()
{
    return this;
}

CefRefPtr<CefLoadHandler> QCefClient::GetLoadHandler()
{
    return this;
}

CefRefPtr<CefContextMenuHandler> QCefClient::GetContextMenuHandler()
{
    return this;
}

CefRefPtr<CefDownloadHandler> QCefClient::GetDownloadHandler()
{
    return this;
}

CefRefPtr<CefFocusHandler> QCefClient::GetFocusHandler()
{
    return this;
}


void QCefClient::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    m_browser=browser;
    m_created=true;
}



void QCefClient::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    emit sgCloseBrowser(browser);
}

void QCefClient::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, ErrorCode errorCode, const CefString &errorText, const CefString &failedUrl)
{
    if(!m_closeLoadError)
    {
        TSpdlog::getInstance()->netError(QStringLiteral("网页加载出错 code:%1 message:%2 url:%3").arg(errorCode).arg(errorText.ToString().c_str()).arg(failedUrl.ToString().c_str()));
    }
}

void QCefClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    if(httpStatusCode==200)
        m_closeLoadError=true;
}

void QCefClient::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model)
{
    if((params->GetTypeFlags()&(CM_TYPEFLAG_PAGE|CM_TYPEFLAG_FRAME))!=0){
        if(model->GetCount()>0){
            model->Clear();
        }
    }
}

void QCefClient::OnBeforeDownload(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDownloadItem> download_item, const CefString &suggested_name, CefRefPtr<CefBeforeDownloadCallback> callback)
{
    callback->Continue(suggested_name,true);
}


bool QCefClient::Visit(const CefCookie &cookie, int count, int total, bool &deleteCookie)
{
    emit sgSendCookie(QString::fromWCharArray(cookie.name.str),QString::fromWCharArray(cookie.value.str));
    return true;
}

bool QCefClient::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString &target_url, const CefString &target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures &popupFeatures, CefWindowInfo &windowInfo, CefRefPtr<CefClient> &client, CefBrowserSettings &settings, CefRefPtr<CefDictionaryValue> &extra_info, bool *no_javascript_access)
{
    switch (target_disposition)
    {
    case WOD_NEW_FOREGROUND_TAB:
    case WOD_NEW_BACKGROUND_TAB:
    case WOD_NEW_POPUP:
    case WOD_NEW_WINDOW:
        QString msg;
        msg+="qtSlots({\"HookPopup\":\"";
        msg+=target_url.ToString().c_str();
        msg+="\"})";
        QCefView::getInstance().sendToWeb(QCefView::getInstance().m_HookPopupName,msg);
        return true;
    }

    return false;
}

void QCefClient::OnGotFocus(CefRefPtr<CefBrowser> browser)
{
    QCefView::getInstance().frontWindow(browser->GetIdentifier());
}



bool QCefClient::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    //收到来自Render进程发过来的信息
    emit sgWebMsgReceived(QString(message->GetName().ToString().c_str()));
    return true;
}

bool QCefClient::OnPreKeyEvent(CefRefPtr<CefBrowser> browser, const CefKeyEvent &event, MSG *os_event, bool *is_keyboard_shortcut)
{
    if(event.windows_key_code==VK_F5){
        browser->ReloadIgnoreCache();
    }else if(event.windows_key_code==VK_F11||event.windows_key_code==VK_ESCAPE){
        QCefView::getInstance().setWindowFillScreen(browser->GetIdentifier());
    }
    return false;
}



CefRefPtr<CefBrowser> QCefClient::browser()
{
    return m_browser;
}

#ifdef Q_OS_WIN
CefWindowHandle QCefClient::browserWinId()
{
    if(m_created)
    {
        return browser()->GetHost()->GetWindowHandle();
    }
    return (CefWindowHandle)-1;
}
#endif
