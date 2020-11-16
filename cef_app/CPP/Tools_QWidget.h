#ifndef TOOLS_QWIDGET_H
#define TOOLS_QWIDGET_H

#include <QObject>

class Tools_QWidget : public QObject
{
    Q_OBJECT
public:
    static Tools_QWidget& getInstance(){
       static Tools_QWidget instance;
       return instance;
     }

    double getZoomFactor(int id=0);
private:
    explicit Tools_QWidget(QObject *parent = nullptr);
    Tools_QWidget(const Tools_QWidget &sg) = delete;
    Tools_QWidget &operator=(const Tools_QWidget &sg) = delete;
signals:

};

#endif // TOOLS_QWIDGET_H
