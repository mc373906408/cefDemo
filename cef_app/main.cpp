#include <QApplication>
#include <Windows.h>
#include <log/TSpdlog.h>
#include <CPP/DirHelper.h>
#include "CPP/FDJ_Macro.h"
#include <QDir>
#include <QDateTime>
#include <QFont>
#include <QTranslator>
#include <QQuickView>
#include <QQmlContext>
#include <browser/QCefApp.h>
#include <browser/QCefView.h>
#include <CPP/Tools.h>
#include <QQuickItem>
#include <CPP/WindowHook.h>
#include <CPP/FDJConfig.h>
#include <QStack>

bool rmDir(const QString &path)
{
    QString m_path=path;
#ifdef Q_OS_WIN
    m_path.replace("\\","/");
    if (!m_path.endsWith("/"))
        m_path += "/";
#endif

    QStack<QString> paths;
    QStack<QString> rmPaths;
    paths.push(m_path);
    rmPaths.push(m_path);

    while (!paths.isEmpty()) {
        QDir dir(paths.pop());
        dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Hidden);

        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.isDir()) {
                paths.push(fileInfo.canonicalFilePath());
                rmPaths.push(fileInfo.canonicalFilePath());
            } else {
                dir.remove(fileInfo.fileName());
            }
        }
    }

    bool flag = true;
    while (!rmPaths.isEmpty()) {
        QDir dir(rmPaths.pop());
        if (dir.exists()){
            if(dir.rmdir(dir.absolutePath()) == false)
                flag = false;
        }
    }

    return flag;
}

void initFDJLog()
{
    QString path = DirHelper::getInstance()->logPath();
    path = path + QDir::separator() + "cefApp_" + QDateTime::currentDateTime().toString("yyyyMMdd") + ".log";
    TSpdlog::registerLog(path.toStdString().c_str(),path.toStdString().c_str());
    TSpdlog::getInstance();

}

void initClearCache(){
    if(FDJConfig::getInstance()->getInstance()){
        rmDir(QCoreApplication::applicationDirPath()+"/cache/Cache");
        FDJConfig::getInstance()->setIsCache(false);
    }
}


int main(int argc,char *argv[])
{
    /*修复高dpi下显示异常*/
    SetProcessDPIAware();


    QApplication::setOrganizationName(FDJ_COMPANY_NAME);
    QApplication::setOrganizationDomain(FDJ_COMPANY_URL);
    QApplication::setApplicationName(FDJ_COMPANY_APP_NAME);

    /* 实例化全局日志 */
    initFDJLog();
    TSpdlog::getInstance()->otherInfo(QStringLiteral("APP启动,version:") + FDJ_COMPANY_APP_VERSION + " id:" + FDJ_PUBLISH_GIT_ID);

    TSpdlog::getInstance()->otherInfo(QStringLiteral("启动：创建程序实例"));
    QApplication app(argc,argv);

    TSpdlog::getInstance()->otherInfo(QStringLiteral("启动：设置全局字体"));
    QFont font = app.font();
    font.setFamily(QStringLiteral("黑体"));
    app.setFont(font);

    TSpdlog::getInstance()->otherInfo(QStringLiteral("启动：加载全局键鼠钩子"));
    WindowHook::getInstance();

    TSpdlog::getInstance()->otherInfo(QStringLiteral("启动：初始化qml对象"));
    QQuickView qmlEngine;

    QObject::connect(&qmlEngine,&QQuickWindow::sceneGraphInitialized,[](){
        TSpdlog::getInstance()->otherInfo(QStringLiteral("启动：qml对象启动成功"));
    });

    /*设置窗口属性*/
    qmlEngine.setFlags(Qt::Dialog|Qt::FramelessWindowHint|Qt::WindowCloseButtonHint|Qt::MSWindowsFixedSizeDialogHint|Qt::WindowMinimizeButtonHint);
//    qmlEngine.setColor(QColor("transparent"));

    /*将自身注册到qml中*/
    qmlEngine.rootContext()->setContextProperty("cppWindow",&qmlEngine);
    /*初始化Tools*/
    qmlEngine.rootContext()->setContextProperty("cppTool",&Tools::getInstance());
    /* 注册日志 */
    qmlEngine.rootContext()->setContextProperty("cppLog",TSpdlog::getInstance(nullptr));
    /*初始化配置文件*/
    qmlEngine.rootContext()->setContextProperty("cppConfig",FDJConfig::getInstance());
    /*注册DIR*/
    qmlEngine.rootContext()->setContextProperty("cppDir",DirHelper::getInstance());
    /*清理缓存*/
    initClearCache();

    /*将Cef注册到qml中*/
    qmlEngine.rootContext()->setContextProperty("cppCef",&QCefView::getInstance());


    TSpdlog::getInstance()->otherInfo(QStringLiteral("启动：加载qml"));
    qmlEngine.setSource(QUrl("qrc:/main.qml"));

    int rv=0;
    TSpdlog::getInstance()->otherInfo(QStringLiteral("启动：初始化cef"));
    QCefContext* cef =new QCefContext();
    rv=cef->initCef(argc,argv);
    if (rv>=0){
        TSpdlog::getInstance()->otherError(QStringLiteral("启动：初始化cef失败"));
        return 0;
    }


    /*初始化cefview*/
    QCefView::getInstance().init(&qmlEngine,cef);

    qmlEngine.show();

    QObject::connect(&app,&QApplication::aboutToQuit,[](){
        /*关闭浏览器标签*/
        QCefView::getInstance().closeAllBrowser();
        TSpdlog::getInstance()->otherInfo("app quit");
    });
    QObject::connect(&app,&QApplication::focusWindowChanged,[](QWindow * focusWindow){
        if(focusWindow){
            QCefView::getInstance().setLostFocusHide(focusWindow->objectName());
        }
    });

    TSpdlog::getInstance()->otherInfo(QStringLiteral("启动：启动完成"));
    rv=app.exec();

    return rv;
}
