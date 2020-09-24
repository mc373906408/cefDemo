#ifndef QCEFCONTEXT_H
#define QCEFCONTEXT_H

#include <include/internal/cef_types_wrappers.h>
#include "QCefApp/QCefApp.h"
#include "QCefRenderHandler/QCefRenderHandler.h"
class QCefContext
{
public:
    QCefContext(CefSettings* settings);
    ~QCefContext(){}

    //初始化 Cef
    int initCef(int argc, char *argv[]);

public:
    CefRefPtr<QCefApp> cefApp() const{return m_cefApp;}
private:
    int initCef(CefMainArgs& mainArgs);
private:
    CefSettings* m_settings;
    CefRefPtr<QCefApp> m_cefApp;
    CefRefPtr<CefCommandLine> m_cmdLine;
    CefRefPtr<QCefRenderHandler> m_cefRenderer;
};

#endif //QCEFCONTEXT_H
