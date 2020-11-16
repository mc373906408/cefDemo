#ifndef QCEFVIEW_H
#define QCEFVIEW_H

#include <QTimer>
#include <QWindow>
#include <include/cef_app.h>
#include "QCefContext.h"
#include "QCefClient.h"
#include <iebrowser/QIE.h>


struct CefClientStruct{
    QString objectName;
    CefRefPtr<QCefClient> client;  //cef智能指针
    int identifier;  //cef唯一id
    QWindow* root;  //本窗口指针
    QWindow* cefWnd;  //cef子窗口指针
    QIE* ieWnd;  //ie子窗口指针
    int width;
    int height;
    int beginWidth; //初始宽度，用于计算子窗口位置
    int beginHeight; //初始高度，用于计算子窗口位置
    QString parent; //父窗口objectName
    int parent_left; //与父窗口的左边距
    int parent_top; //与父窗口的上边距
    int parent_right; //与父窗口的右边距
    int parent_bottom; //与父窗口的下边距
    bool ie;  //是否是ie
    QString url;
    QPoint clickPos; //位移开始时鼠标位置
    QStringList children; //子窗口列表
    bool needHide=false;  //切换标签隐藏
    bool lostFocusHide=false; //失去焦点隐藏
    int win_x;
    int win_y;
    int win_width;
    int win_height;
    bool isMax=false;
    bool isFront=false;
};

class QCefView:public QObject
{
    Q_OBJECT
    enum Metamorphosis{
        X,
        Y,
        Width,
        Height
    };

public:

    static QCefView& getInstance(){
        static QCefView instance;
        return instance;
    }

    /*初始化*/
    void init(QWindow *window,QCefContext *cef);

    /*关闭所有网页*/
    Q_INVOKABLE void closeAllBrowser();

    /*给指定H5发请求*/
    Q_INVOKABLE void sendToWeb(QString objectName,QString msg);

    /*修改CEF窗口状态*/
    Q_INVOKABLE void setWindowStatus(QString objectName,QString status);

    /*还原全屏*/
    void setWindowFillScreen(int identifier);

    /*设置隐藏*/
    void setWindowHidden();

    /*关闭指定窗口*/
    Q_INVOKABLE void closeWindow(QString objectName);

    /*打开新窗口*/
    Q_INVOKABLE void openWindow(QString objectName,QString fatherName,QString ie,QString url,QString rect,QString align,QString valign,QString needHide,QString lostFocusHide,QString front);

    /*切换窗口*/
    Q_INVOKABLE void switchWindow(QString objectName,QString needHide);

    /*移动指定窗口*/
    Q_INVOKABLE void moveWindow(QString objectName);

    /*获取当前窗口的同级窗口，如果是顶级窗口，返回子窗口列表*/
    Q_INVOKABLE QString getCefClients(QString objectName);

    /*获取父窗口name*/
    Q_INVOKABLE QString getFatherName(QString objectName);

    /*测试接口*/
    Q_INVOKABLE void test();

    /*关闭菜单栏等*/
    void setLostFocusHide(QString objectName);

    /*获取窗口指针*/
    CefClientStruct *getCefClientStruct(QString objectName);

    /*设置拦截网址*/
    Q_INVOKABLE void setHookPopup(QString objectName);

    /*清理IE缓存*/
    Q_INVOKABLE void clearIECache();

    /*打开外部浏览器*/
    Q_INVOKABLE void openUrl(QString url);

    /*强制刷新*/
    Q_INVOKABLE void reload(QString objectName);

    /*重定向*/
    Q_INVOKABLE void load(QString objectName,QString url);

    /*置顶窗口*/
    void frontWindow(int identifier);

signals:
    void sgWebMsgReceived(QString msg);

    void sgOpenNewWindow(QObject* father,const QString &json);
    void sgCloseWindow(const QString &name);

public slots:
    void onCloseBrowser(CefRefPtr<CefBrowser> browser);
    void onIECloseBrowser(QString objectName);
    void onSendCookie(QString cookieName,QString cookie);

protected slots:
    virtual void onCefTimer();


public:
    bool ismove=false;

    QString m_HookPopupName;
    QStringList m_HookPopup;
private:
    QCefView(){}

    QCefView(const QCefView &) = delete;
    QCefView &operator =(const QCefView &) = delete;

    void windowMetamorphosis(CefClientStruct *cefClient,Metamorphosis mode);
private:
    QTimer *m_cefTimer;
    QWindow *m_window;
    QCefContext *m_cef;
    QHash<QString,CefClientStruct> m_cefClients;
    QStringList m_cefClientsOpenList;
    CefClientStruct *m_lostFocusHideClient;
    qint64 m_Flock=0;
    QHash<QString,QString> cookies;
};


#endif //QCEFVIEW_H
