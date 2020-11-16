#ifndef FDJCONFIG_H
#define FDJCONFIG_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
class QSettings;
class FDJConfigData;

class FDJConfig : public QObject
{
    Q_OBJECT
public:
    static FDJConfig *getInstance();

signals:

public:

    bool getIsCache();
    Q_INVOKABLE void setIsCache(bool flag);

    Q_INVOKABLE void set(const QString &k,const QString &v);
    Q_INVOKABLE QString get(const QString &k);
private:
    explicit FDJConfig(QObject *parent = nullptr);
    static FDJConfig *m_config;

    FDJConfig(const FDJConfig &) = delete;
    FDJConfig &operator =(const FDJConfig &) = delete;

    QSettings *m_setting = nullptr;

    QString KEY_Browser_IsCache="Browser/IsCache";

private:

};

#endif // FDJCONFIG_H
