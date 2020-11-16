#ifndef WINDOWHOOK_H
#define WINDOWHOOK_H

#include <QObject>
#include "qhook.h"
#include "browser/QCefView.h"

//enum HookEnum{
//    Mouse,
//    Keyboard
//};

class WindowHook : public QHook
{
    Q_OBJECT
private:
    /*鼠标移动事件*/
    virtual bool mouseMoveEvent(QHookMouseEvent *event) override;
    /*鼠标点击事件*/
    virtual bool mousePressEvent(QHookMouseEvent *event) override;
    /*鼠标释放事件*/
    virtual bool mouseReleaseEvent(QHookMouseEvent *event) override;
    /*按键按压事件*/
    virtual bool keyPressEvent(QHookKeyEvent *event) override;
    /*按键释放事件*/
    virtual bool keyReleaseEvent(QHookKeyEvent *event) override;
public:
    static WindowHook& getInstance(){
       static WindowHook instance;
       return instance;
     }
    void moveWindow(CefClientStruct* cefClient);


private:
    WindowHook();
    WindowHook(const WindowHook &) = delete;
    WindowHook &operator =(const WindowHook &) = delete;

    QHook* m_hook;
    CefClientStruct* m_cefClient=nullptr;

signals:

};

#endif // WINDOWHOOK_H
