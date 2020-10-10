//#include <QGuiApplication>
//#include <QQmlApplicationEngine>

#include <MainDlg/MainDlg.h>
#include <QCefContext/QCefContext.h>
#include <QApplication>
#include <QQuickView>
#include <QQmlContext>
#include <QDebug>
#include <QObject>
#include "test.h"

int main(int argc, char *argv[])
{
//    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

//    QGuiApplication app(argc, argv);

//    QQmlApplicationEngine engine;
//    const QUrl url(QStringLiteral("qrc:/main.qml"));
//    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
//                     &app, [url](QObject *obj, const QUrl &objUrl) {
//        if (!obj && url == objUrl)
//            QCoreApplication::exit(-1);
//    }, Qt::QueuedConnection);
//    engine.load(url);

//    return app.exec();

//    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    /*修复windows下高dpi显示异常*/
    SetProcessDPIAware();
    int result = 0;

//    CefSettings settings;
//    // 禁用日志
//    settings.log_severity = LOGSEVERITY_DISABLE;
//    // 设置CEF资源路径(cef.pak和/或devtools_resources.pak)
//    CefString(&settings.resources_dir_path) = CefString();
//    // 本地化资源路径
//    CefString(&settings.locales_dir_path) = CefString();
//    settings.remote_debugging_port=7777;

    QCefContext* cef = new QCefContext();
    result = cef->initCef(argc, argv);
    if (result >= 0)
        return result;

    QApplication a(argc, argv);
    QApplication::addLibraryPath(".");

    QQuickView qmlEngine;
    QObject::connect(&qmlEngine,&QQuickWindow::sceneGraphInitialized,[](){
//        TSpdlog::getInstance()->otherInfo(QStringLiteral("启动：qml对象启动成功"));
    });
    qmlEngine.rootContext()->setContextProperty("cppWindow",&qmlEngine);
//    Hello *hello=new Hello;
    QCefView* m_webview = new QCefView(cef->cefApp()->addBrowser(), &qmlEngine);
    test *m_test=new test(m_webview);
    qmlEngine.rootContext()->setContextProperty("cpphello",m_test);
    qmlEngine.setSource(QUrl("qrc:/main.qml"));

    qmlEngine.show();



//    QObject* object= qmlEngine.focusObject();
//    QObject::connect(object,SIGNAL(sgbutton()),&hello,SLOT(qsbutton()));
//    MainDlg* browser = new MainDlg(cef->cefApp());
//    QObject* webview=qmlEngine.findChild<QObject*>("webview");


//    m_webview->load(QUrl("http://www.baidu.com"));

    result = a.exec();

//    delete browser;
    delete cef;

    CefShutdown();

    return result;
}
