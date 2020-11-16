#include "QIE.h"
#include <QDebug>
#include <wininet.h>


void QIE::loadUrl(const QString &url)
{
    this->dynamicCall("Navigate2(const QString&)",url);
}

void QIE::setObjectName(const QString &m_objectName)
{
    this->objectName=m_objectName;
}


QIE::QIE(QString url,QHash<QString,QString> cookies) :QAxWidget()
{
    setControl("{8856F961-340A-11D0-A96B-00C04FD705A2}");
    this->queryInterface((QUuid&)IID_IWebBrowser2,(void **)&webBrowser);
    // 禁止弹出错误框
    if(webBrowser)
    {
        VARIANT_BOOL bValue = true;
        webBrowser->put_Silent(bValue);
        webBrowser->Release();
    }
    /*设置cookie*/
    for(auto cookie=cookies.begin();cookie!=cookies.constEnd();++cookie){
        InternetSetCookie(url.toStdWString().c_str(),cookie.key().toStdWString().c_str(),cookie.value().toStdWString().c_str());
    }
}
