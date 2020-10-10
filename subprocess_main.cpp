﻿#include "QCefRenderHandler/QCefRenderHandler.h"

int main(int argc, char *argv[])
{
    SetProcessDPIAware();
    HINSTANCE hInstance = (HINSTANCE)GetModuleHandle(NULL);
    CefMainArgs main_args(hInstance);
//    CefRefPtr<CefApp> app(new QCefApp);
    CefRefPtr<CefApp> app(new QCefRenderHandler);
    return CefExecuteProcess(main_args, app.get(),NULL);
}
