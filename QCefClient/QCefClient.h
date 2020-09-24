#ifndef QCEFCLIENT_H
#define QCEFCLIENT_H

#include <QObject>
#include <include/cef_client.h>
#include <include/cef_life_span_handler.h>
#include <include/wrapper/cef_helpers.h>



class QCefClient:
        public QObject,
        public CefClient,
        public CefLifeSpanHandler,
        public CefLoadHandler

{
    Q_OBJECT
public:
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, TransitionType transition_type) override;
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString &target_url, const CefString &target_frame_name, WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures &popupFeatures, CefWindowInfo &windowInfo, CefRefPtr<CefClient> &client, CefBrowserSettings &settings, CefRefPtr<CefDictionaryValue> &extra_info, bool *no_javascript_access) override;
    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;
public:

    CefRefPtr<CefBrowser> browser();
    CefWindowHandle browserWinId();


public slots:
    void load(CefString url);
    void reload();

signals:
//    void loadStarted(bool isMainFrame);
//    void loadFinished(bool ok, bool isMainFrame);
//    void loadError(QString errorStr);
    void webMsgReceived(QString msg);

private:
    CefRefPtr<CefBrowser> m_browser;
    bool m_created=false;
    CefString m_url;

    // Include the default reference counting implementation.
    IMPLEMENT_REFCOUNTING(QCefClient);
};

#endif //QCEFCLIENT_H
