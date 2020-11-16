#include "WindowHook.h"


bool WindowHook::mouseMoveEvent(QHookMouseEvent *event)
{
    if(m_cefClient){
        QCefView::getInstance().ismove=true;
        m_cefClient->root->setPosition(event->pos()-m_cefClient->clickPos);
        /*移动他名下的子窗口*/
        foreach(QString childName,m_cefClient->children){
            CefClientStruct* client=QCefView::getInstance().getCefClientStruct(childName);
            if(client){
                client->root->setPosition(event->pos()-m_cefClient->clickPos+QPoint(client->parent_left,client->parent_top));
            }
        }
    }
    return true;
}

bool WindowHook::mousePressEvent(QHookMouseEvent *event)
{
    return true;
}

bool WindowHook::mouseReleaseEvent(QHookMouseEvent *event)
{
    if(m_cefClient){
        QCefView::getInstance().ismove=false;
        m_cefClient=nullptr;
    }
    return true;
}

bool WindowHook::keyPressEvent(QHookKeyEvent *event)
{
    return true;
}

bool WindowHook::keyReleaseEvent(QHookKeyEvent *event)
{
    /*F8*/
    if(event->keyCode()==119){
        QCefView::getInstance().setWindowHidden();
    }
    return true;
}

void WindowHook::moveWindow(CefClientStruct *cefClient)
{
    cefClient->clickPos=QCursor::pos()-cefClient->root->position();
    m_cefClient=cefClient;
}


WindowHook::WindowHook()
{
    m_hook=QHook::instance();
    m_hook->hookMouse();
    m_hook->hookKeyboard();
}
