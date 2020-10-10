#include "QCefView/QCefView.h"
#include <include/cef_app.h>
#include <QStackedLayout>
#include <QUrl>
#include <QVariant>
#include <QDebug>

QCefView::QCefView(CefRefPtr<QCefClient> cefClient,QWindow *window)
{
    m_cefEmbedded = false;
    m_cefClient = cefClient;
    m_window=window;

    /*cef的信息循环*/
    m_cefTimer = new QTimer(this);
    connect(m_cefTimer, SIGNAL(timeout()), this, SLOT(onCefTimer()));
    m_cefTimer->start(10);

    QCefClient* cefClientPtr=m_cefClient.get();
    connect(this,&QCefView::sgLoad,cefClientPtr,&QCefClient::load);
    connect(cefClientPtr, SIGNAL(webMsgReceived(QString)), this, SIGNAL(webMsgReceived(QString)));
}

void QCefView::onCefTimer()
{
    CefDoMessageLoopWork();
    if(m_cefEmbedded==false)
    {
        CefWindowHandle browserHandle=m_cefClient->browserWinId();
        if(browserHandle!=(CefWindowHandle)-1)
        {
            subW = QWindow::fromWinId((WId)browserHandle);
            subW->setVisible(true);
//            subW->setProperty("_q_embedded_native_parent_handle",m_window->winId());
            subW->setParent(m_window);
            subW->resize(800,600);
            subW->setX(0);
            subW->setY(300);
//            QWidget* container=QWidget::createWindowContainer(subW,this);
//            QStackedLayout* cefLayout = new QStackedLayout(this);
//            setLayout(cefLayout);
//            container->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
//            cefLayout->addWidget(container);
            m_cefEmbedded=true;
            emit cefEmbedded();
        }
    }

}

void QCefView::setresize()
{
    subW->resize(1600,600);
}


void QCefView::load(QUrl url)
{
    emit sgLoad(url.toString().toStdString());
}


void QCefView::runJavaScript(QString script)
{
//    CefRefPtr<CefFrame> frame = m_cefClient->browser()->GetMainFrame();
//    frame->ExecuteJavaScript(script.toStdString(), frame->GetURL(), 0);
}

void QCefView::sendToWeb(QString msg)
{
    // 发送给H5信息，先发送给RENDERER，在RENDERER进程中转发给H5
//    runJavaScript(QString("recvMessage('%1');").arg(msg));
    CefString msgStr=msg.toStdString().c_str();
    m_cefClient->browser()->GetMainFrame()->SendProcessMessage(PID_RENDERER,CefProcessMessage::Create(msgStr));
}
