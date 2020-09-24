#include "test.h"
#include <QDebug>
#include <QUrl>

test::test(QCefView* cefView,QObject *parent) : QObject(parent)
{
    m_cefView=cefView;
}

void test::qtest()
{
    if(!bb){
//        m_cefView->load(QUrl("http://www.baidu.com"));
        bb=true;
    }


    m_cefView->setresize();
    m_cefView->sendToWeb("22222");
//    SetProcessDPIAware();
}

void test::onRecvFromWeb(QString msg)
{
    qDebug()<<msg;
}
