#include "QCefView.h"
#include <QUrl>
#include <QDebug>
#include "CPP/Tools.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QCursor>

#include <CPP/WindowHook.h>
#include <QProcess>
#include <QDesktopServices>



void QCefView::init( QWindow *window, QCefContext *cef)
{
    m_window=window;
    m_cef=cef;

    /*cef的信息循环*/
    m_cefTimer = new QTimer(this);
    connect(m_cefTimer, SIGNAL(timeout()), this, SLOT(onCefTimer()));
    m_cefTimer->start(10);
}

void QCefView::onCefTimer()
{
    CefDoMessageLoopWork();
    if(!m_cefClientsOpenList.empty()){
        if(m_cefClients.contains(m_cefClientsOpenList.at(0))){
            CefClientStruct *cefClient= &m_cefClients[m_cefClientsOpenList.at(0)];
            QWindow* root = m_window->findChild<QWindow*>(cefClient->objectName);
            if(!root){
                return;
            }else{
                cefClient->root=root;
            }
            if(!cefClient->ie){
                CefWindowHandle browserHandle=cefClient->client->browserWinId();
                if(browserHandle!=(CefWindowHandle)-1){
                    m_cefClientsOpenList.removeAt(0);
                    /*获取cookie*/
                    CefRefPtr<CefCookieManager> cefCookieManager = CefCookieManager::GetGlobalManager(nullptr);
                    if(cefCookieManager){
                        cefCookieManager->VisitUrlCookies(cefClient->url.toStdString(),true,cefClient->client);
                    }
                    /*绑定父窗口*/
                    QWindow *window=QWindow::fromWinId((WId)browserHandle);
                    window->setParent(cefClient->root);
                    window->resize(cefClient->width,cefClient->height);
                    window->setX(0);
                    window->setY(0);
                    window->setVisible(true);

                    /*绑定cefWindow*/
                    cefClient->cefWnd=window;
                    /*绑定唯一ID*/
                    cefClient->identifier=cefClient->client->browser()->GetIdentifier();
                    /*绑定信号槽*/
                    QCefClient* cefClientPtr=cefClient->client.get();
                    connect(cefClientPtr, SIGNAL(sgWebMsgReceived(QString)), this, SIGNAL(sgWebMsgReceived(QString)));

                    /*窗口长宽自适应*/
                    connect(cefClient->root,&QWindow::widthChanged,[=](int arg){
                        cefClient->cefWnd->setWidth(arg);
                    });
                    connect(cefClient->root,&QWindow::heightChanged,[=](int arg){
                        cefClient->cefWnd->setHeight(arg);
                    });
                }
            }else{
                m_cefClientsOpenList.removeAt(0);
                QIE *qie=new QIE(cefClient->url,cookies);
                qie->windowHandle()->setParent(cefClient->root);
                qie->loadUrl(cefClient->url);
                qie->setObjectName(cefClient->objectName);
                qie->resize(cefClient->width,cefClient->height);
                qie->move(0,0);
                qie->show();

                /*绑定ieWindow*/
                cefClient->ieWnd=qie;

                /*窗口长宽自适应*/
                connect(cefClient->root,&QWindow::widthChanged,[=](int arg){
                    cefClient->ieWnd->resize(arg,cefClient->ieWnd->height());
                });
                connect(cefClient->root,&QWindow::heightChanged,[=](int arg){
                    cefClient->ieWnd->resize(cefClient->ieWnd->width(),arg);
                });


                /*绑定关闭事件*/
                connect(qie,&QIE::sgClose,this,&QCefView::onIECloseBrowser);
            }


            connect(cefClient->root,&QWindow::widthChanged,[=](int arg){
                cefClient->width=arg;
                this->windowMetamorphosis(cefClient,Metamorphosis::Width);
            });
            connect(cefClient->root,&QWindow::heightChanged,[=](int arg){
                cefClient->height=arg;
                this->windowMetamorphosis(cefClient,Metamorphosis::Height);
            });
            connect(cefClient->root,&QWindow::xChanged,[=](int arg){
                if(!ismove){
                    this->windowMetamorphosis(cefClient,Metamorphosis::X);
                }

            });
            connect(cefClient->root,&QWindow::yChanged,[=](int arg){
                if(!ismove){
                    this->windowMetamorphosis(cefClient,Metamorphosis::Y);
                }
            });


            /*自动移动像素,修复界面错位bug*/
            cefClient->root->setWidth(cefClient->root->width()+1);
            cefClient->root->setWidth(cefClient->root->width()-1);
            cefClient->root->setHeight(cefClient->root->height()+1);
            cefClient->root->setHeight(cefClient->root->height()-1);
            if(!cefClient->parent.isEmpty()){
                if(m_cefClients.contains(cefClient->parent)){
                    CefClientStruct *client= &m_cefClients[cefClient->parent];
                    cefClient->root->setPosition(client->root->position()+QPoint(cefClient->parent_left,cefClient->parent_top));
                }
            }
        }
    }
}

void QCefView::windowMetamorphosis(CefClientStruct *cefClient, QCefView::Metamorphosis mode)
{
    if(!cefClient->parent.isEmpty()){
        if(m_cefClients.contains(cefClient->parent)){
            CefClientStruct *client= &m_cefClients[cefClient->parent];
            /*修改与父窗口的相对位置*/
            switch(mode){
            case Width:
                cefClient->parent_right=client->width-(cefClient->width+cefClient->parent_left);
                break;
            case Height:
                cefClient->parent_bottom=client->height-(cefClient->height+cefClient->parent_top);
                break;
            }
        }
    }
    foreach(QString childName,cefClient->children){
        if(m_cefClients.contains(childName)){
            CefClientStruct *client= &m_cefClients[childName];
            /*移动他名下的子窗口*/
            switch(mode){
            case X:
                client->root->setX(cefClient->root->x()+client->parent_left);
                break;
            case Y:
                client->root->setY(cefClient->root->y()+client->parent_top);
                break;
            case Width:
                client->root->setWidth(cefClient->root->width()-client->parent_left-client->parent_right);
                break;
            case Height:
                client->root->setHeight(cefClient->root->height()-client->parent_top-client->parent_bottom);
                break;
            }
        }
    }
}



void QCefView::sendToWeb(QString objectName,QString msg)
{
    // 发送给H5信息，先发送给RENDERER，在RENDERER进程中转发给H5
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient= &m_cefClients[objectName];
        cefClient->client->browser()->GetMainFrame()->SendProcessMessage(PID_RENDERER,CefProcessMessage::Create(msg.toStdString().c_str()));
    }
}

void QCefView::setWindowStatus(QString objectName, QString status)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *client= &m_cefClients[objectName];
        if(status=="Maximized"){
            client->win_x=client->root->x();
            client->win_y=client->root->y();
            client->win_width=client->root->width();
            client->win_height=client->root->height();
            QRect rect=Tools::getInstance().getNowAvailableRect(Tools::getInstance().getMousePos());
            client->root->setX(rect.x());
            client->root->setY(rect.y());
            client->root->setWidth(rect.width());
            client->root->setHeight(rect.height());
            client->isMax=true;
        }else if(status=="Windowed"){
            client->root->setVisibility(QWindow::Windowed);
            if(client->isMax){
                client->isMax=false;
                client->root->setX(client->win_x);
                client->root->setY(client->win_y);
                client->root->setWidth(client->win_width);
                client->root->setHeight(client->win_height);
            }
        }else if(status=="Minimized"){
            m_window->setVisibility(QWindow::Minimized);
        }else if(status=="FullScreen"){
            client->root->setVisibility(QWindow::FullScreen);
        }
    }
}


void QCefView::setWindowFillScreen(int identifier)
{
    /*忽略近期重复按键*/
    if((QDateTime::currentMSecsSinceEpoch()-1000)>m_Flock){
        m_Flock=QDateTime::currentMSecsSinceEpoch();
    }else{
        return;
    }
    for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
        if(cefClient.value().identifier==identifier){
            if(cefClient.value().root->visibility()==QWindow::FullScreen){
                cefClient.value().root->setVisibility(QWindow::Windowed);
            }
            break;
        }
    }
}


void QCefView::setWindowHidden()
{
    if(m_window->visibility()==QWindow::Hidden){
        m_window->setVisibility(QWindow::Windowed);
    }else{
        m_window->setVisibility(QWindow::Hidden);
    }
    for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
        if(cefClient.value().root->visibility()==QWindow::Hidden){
            cefClient.value().root->setVisibility(QWindow::Windowed);
        }else{
            cefClient.value().root->setVisibility(QWindow::Hidden);
        }
    }
}

void QCefView::closeWindow(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *client= &m_cefClients[objectName];
        client->root->close();
    }
}



void QCefView::openWindow(QString objectName,QString fatherName,QString ie,QString url,QString rect,QString align,QString valign,QString needHide,QString lostFocusHide,QString front)
{
    if(!m_cef){
        return;
    }
    if(!m_cefClients.contains(objectName)){
        CefClientStruct m_CefClientStruct;
        QStringList m_rect=rect.split(',');
        double zoom=Tools::getInstance().getNowZoomFactor(QCursor::pos());
        int x=m_rect.at(0).toInt()*zoom;
        int y=m_rect.at(1).toInt()*zoom;
        int width=m_rect.at(2).toInt()*zoom-x;
        int height=m_rect.at(3).toInt()*zoom-y;

        QJsonObject json;
        json["objectName"]=objectName;
        json["x"]=x;
        json["y"]=y;
        json["width"]=width;
        json["height"]=height;
        m_CefClientStruct.beginWidth=json["width"].toInt();
        m_CefClientStruct.beginHeight=json["height"].toInt();

        QRect p_rect;
        /*找下父窗口存不存在*/
        QObject *m_father=nullptr;
        if(m_cefClients.contains(fatherName)){
            CefClientStruct *cefClient= &m_cefClients[fatherName];
            p_rect=QRect(cefClient->root->x(),cefClient->root->y(),cefClient->width,cefClient->height);
            json["x"]=x+p_rect.x();
            json["y"]=y+p_rect.y();
            if(lostFocusHide!="1"){
                json["width"]=width+cefClient->width-cefClient->beginWidth;
                json["height"]=height+cefClient->height-cefClient->beginHeight;
            }
            m_father=cefClient->root->focusObject();
            /*加到父窗口队列中，并排在第一位*/
            cefClient->children.push_front(objectName);
            m_CefClientStruct.parent=cefClient->objectName;

            if(needHide=="1"){
                /*将父窗口的子窗口页面中其他需要隐藏的隐藏*/
                for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
                    if(cefClient.value().parent==fatherName&&cefClient.value().needHide){
                        cefClient->root->setVisibility(QWindow::Hidden);
                    }
                }
                m_CefClientStruct.needHide=true;
            }
        }else{
            p_rect=Tools::getInstance().getNowDesktopRect(QCursor::pos());
        }
        if(align=="Left"){
            json["x"]=p_rect.x();
        }else if(align=="Center"){
            json["x"]=(p_rect.width()-width)/2+p_rect.x();
        }else if(align=="Right"){
            json["x"]=p_rect.width()-width+p_rect.x();
        }
        if(valign=="Top"){
            json["y"]=p_rect.y();
        }else if(valign=="VCenter"){
            json["y"]=(p_rect.height()-height)/2+p_rect.y();
        }else if(valign=="Bottom"){
            json["y"]=p_rect.height()-height+p_rect.y();
        }

        json["m_width"]=json["width"].toInt();
        json["m_height"]=json["height"].toInt();
        if(front=="1"){
            m_CefClientStruct.isFront=true;
        }


        m_CefClientStruct.width=json["width"].toInt();
        m_CefClientStruct.height=json["height"].toInt();
        m_CefClientStruct.parent_left=json["x"].toInt()-p_rect.x();
        m_CefClientStruct.parent_top=json["y"].toInt()-p_rect.y();
        m_CefClientStruct.parent_right=p_rect.width()-(json["x"].toInt()+json["width"].toInt());
        m_CefClientStruct.parent_bottom=p_rect.height()-(json["y"].toInt()+json["height"].toInt());
        emit sgOpenNewWindow(m_father,QString(QJsonDocument(json).toJson()));

        if(ie!="1"){
            CefRefPtr<QCefClient> client=m_cef->cefApp()->addBrowser(this,"app",false,url,width,height,0,0,nullptr);
            m_CefClientStruct.client=client;
            m_CefClientStruct.ie=false;
        }else{
            m_CefClientStruct.ie=true;
        }

        m_CefClientStruct.objectName=objectName;
        m_CefClientStruct.url=url;

        if(lostFocusHide=="1"){
            m_CefClientStruct.lostFocusHide=true;
        }
        m_cefClients[objectName]=m_CefClientStruct;
        m_cefClientsOpenList.append(objectName);
    }else{
        load(objectName,url);
    }
}

void QCefView::switchWindow(QString objectName, QString needHide)
{
    /*窗口是否已经打开*/
    if(m_cefClients.contains(objectName)){
        CefClientStruct *client= &m_cefClients[objectName];
        /*将父窗口的子窗口页面中其他需要隐藏的隐藏*/
        if(needHide=="1"){
            if(!client->parent.isEmpty()){
                /*如果自己是子窗口*/
                for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
                    if(cefClient.value().parent==client->parent&&cefClient.value().needHide&&cefClient->objectName!=objectName){
                        cefClient->root->setVisibility(QWindow::Hidden);
                    }
                }
            }else{
                /*如果自己是父窗口*/
                for(auto child:client->children){
                    for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
                        if(cefClient.value().objectName==child){
                            cefClient->root->setVisibility(QWindow::Hidden);
                            break;
                        }
                    }
                }
            }
        }
        if(client->root->visibility()==QWindow::Hidden||client->root->visibility()==QWindow::Minimized){
            client->root->setVisibility(QWindow::AutomaticVisibility);
        }
        client->root->raise();
        /*如果父窗口不为空，将自己排在父窗口结构体子窗口队列的第一位*/
        if(!client->parent.isEmpty()){
            if(m_cefClients.contains(client->parent)){
                CefClientStruct *cefClient= &m_cefClients[client->parent];
                cefClient->children.removeOne(objectName);
                cefClient->children.push_front(objectName);
            }
        }
    }
}

void QCefView::moveWindow(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient= &m_cefClients[objectName];
        WindowHook::getInstance().moveWindow(cefClient);
    }
}

QString QCefView::getCefClients(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient= &m_cefClients[objectName];
        /*如果父窗口存在，就发送同级窗口*/
        if(!cefClient->parent.isEmpty()){
            CefClientStruct *client= &m_cefClients[cefClient->parent];
            return "["+client->children.join(',')+"]";
        }
        else{
            return "["+cefClient->children.join(',')+"]";
        }
    }
    return "[]";
}

QString QCefView::getFatherName(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient= &m_cefClients[objectName];
        if(cefClient->parent.isEmpty()){
            return objectName;
        }
        return cefClient->parent;
    }
    return "";
}

void QCefView::test()
{


}

void QCefView::setLostFocusHide(QString objectName)
{
    if(m_lostFocusHideClient){
        if(m_lostFocusHideClient->objectName!=objectName){
            m_lostFocusHideClient->root->close();
            m_lostFocusHideClient=nullptr;
        }
    }
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient= &m_cefClients[objectName];
        if(cefClient->lostFocusHide){
            m_lostFocusHideClient=cefClient;
        }
    }
}

CefClientStruct *QCefView::getCefClientStruct(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        return &m_cefClients[objectName];
    }
    return nullptr;
}

void QCefView::setHookPopup(QString objectName)
{
    m_HookPopupName=objectName;
}

void QCefView::clearIECache()
{
    QProcess process;
    process.start("cmd",QStringList()<<"/c"<<"RunDll32.exe InetCpl.cpl,ClearMyTracksByProcess 10");
    process.waitForStarted();
    process.waitForFinished();
}

void QCefView::openUrl(QString url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void QCefView::reload(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient=&m_cefClients[objectName];
        cefClient->client->browser()->ReloadIgnoreCache();
    }
}

void QCefView::load(QString objectName, QString url)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct *cefClient=&m_cefClients[objectName];
        cefClient->client->browser()->GetMainFrame()->LoadURL(url.toStdString());
    }
}

void QCefView::frontWindow(int identifier)
{
    for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
        if(cefClient.value().isFront&&cefClient.value().client->browser()->GetIdentifier()!=identifier){
            cefClient.value().root->raise();
        }
    }
}



void QCefView::onCloseBrowser(CefRefPtr<CefBrowser> browser)
{
    QString objectName;
    for(auto cefClient=m_cefClients.begin();cefClient!=m_cefClients.constEnd();++cefClient){
        if(cefClient.value().identifier==browser->GetIdentifier()){
            objectName=cefClient.value().objectName;
            if(!cefClient.value().parent.isEmpty()){
                /*在父窗口队列里删掉自己*/
                CefClientStruct* client=&m_cefClients[cefClient.value().parent];
                client->children.removeOne(cefClient.value().objectName);
            }
            emit sgCloseWindow(cefClient.value().objectName);
            break;
        }
    }
    if(!objectName.isEmpty()){
        m_cefClients.remove(objectName);
    }
}

void QCefView::onIECloseBrowser(QString objectName)
{
    if(m_cefClients.contains(objectName)){
        CefClientStruct* cefClient=&m_cefClients[objectName];
        cefClient->ieWnd->deleteLater();
        if(!cefClient->parent.isEmpty()){
            /*在父窗口队列里删掉自己*/
            CefClientStruct* client=&m_cefClients[cefClient->parent];
            client->children.removeOne(objectName);
        }
        emit sgCloseWindow(objectName);
    }
    m_cefClients.remove(objectName);

}

void QCefView::onSendCookie(QString cookieName,QString cookie)
{
    cookies[cookieName]=cookie;
}



void QCefView::closeAllBrowser()
{
    m_cef->cefApp()->closeAllBrowser();
}

