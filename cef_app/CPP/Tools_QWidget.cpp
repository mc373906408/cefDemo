#include "Tools_QWidget.h"
#include <QScreen>
#include <QGuiApplication>

Tools_QWidget::Tools_QWidget(QObject *parent) : QObject(parent)
{

}


double Tools_QWidget::getZoomFactor(int id)
{
#ifdef Q_OS_WIN32
    /* 可能存在的优化：使用静态变量存储值，并监听系统改变界面信号，重置该值 */
    QScreen* screen = QGuiApplication::screens()[id];
    int dpi = static_cast<int>(screen->logicalDotsPerInch());
    int devicePixel = static_cast<int>(screen->devicePixelRatio());

    if (devicePixel == 1) {
        if(dpi<=96){
            return 1.0;
        }else if(dpi<=120){
            return 1.25;
        }else if(dpi<=144){
            return 1.5;
        }else if(dpi<=168){
            return 1.75;
        }else if(dpi<=192){
            return 2.0;
        }else if(dpi>192){
            return 2.0;
        }
    }

    if (devicePixel == 2) {
        if(dpi<=72){
            return 1.5;
        }else if (dpi<=84){
            return 1.75;
        }else if(dpi<=96){
            return 2.0;
        }else if(dpi>96){
            return 2.0;
        }
    }
    return 1.0;
#else
    return 1.0;
#endif
}
