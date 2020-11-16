#ifndef QIE_H
#define QIE_H

#include <QObject>
#include <QAxWidget>
#include <QDebug>
#include <QEvent>
#include <ExDisp.h>

class QIE : public QAxWidget
{
    Q_OBJECT
public:
    QIE(QString url,QHash<QString,QString> cookies);

    void loadUrl(const QString &url);
    void setObjectName(const QString &m_objectName);

private:
    bool event(QEvent *e){
        if(e->type()==QEvent::WinIdChange){
            if(!isClose){
                isClose=true;
            }else{
                emit sgClose(objectName);
            }
        }
        return QWidget::event(e);
    }
signals:
    void sgClose(QString objectName);

private:
    bool isClose=false;
    QString objectName;
    IWebBrowser2 *webBrowser;


};

#endif // QIE_H
