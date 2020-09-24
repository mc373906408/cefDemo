#ifndef TEST_H
#define TEST_H

#include <QObject>
#include "QCefView/QCefView.h"

class test : public QObject
{
    Q_OBJECT
public:
    explicit test(QCefView* cefView,QObject *parent = nullptr);

    Q_INVOKABLE void qtest();

public slots:
    void onRecvFromWeb(QString msg);
private:
    QCefView* m_cefView;
    bool bb=false;
signals:

};

#endif // TEST_H
