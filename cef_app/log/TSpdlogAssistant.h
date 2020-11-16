#ifndef TSPDLOGASSISTANT_H
#define TSPDLOGASSISTANT_H

#include <QObject>

enum FDJLogType {
    AV_TYPE,
    UI_EVENTType,   /* UI操作、主程序日志 */
    UI_CLICK,
    NET_Type,
    H5_Type,
    Other_Type,
};

class TSpdlogAssistant : public QObject
{
    Q_OBJECT
public:
    explicit TSpdlogAssistant(QObject *parent = nullptr);
    QString getTypeName(FDJLogType type);
signals:

public slots:
};

#endif // TSPDLOGASSISTANT_H
