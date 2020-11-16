#include <QSettings>
#include <QDebug>
#include "FDJ_Macro.h"
#include "FDJConfig.h"
#include "DirHelper.h"




FDJConfig *FDJConfig::m_config = nullptr;
FDJConfig *FDJConfig::getInstance()
{
    if (!m_config) {
        m_config = new FDJConfig(nullptr);
    }
    return m_config;
}

bool FDJConfig::getIsCache()
{
    return m_setting->value(KEY_Browser_IsCache,false).toBool();
}

void FDJConfig::setIsCache(bool flag)
{
    m_setting->setValue(KEY_Browser_IsCache,flag);
}

FDJConfig::FDJConfig(QObject *parent) : QObject(parent)
{
    DirHelper * handle = DirHelper::getInstance();
    m_setting = new QSettings(handle->configPath()+ "/" + "config.ini",QSettings::IniFormat);
}


void FDJConfig::set(const QString &k, const QString &v)
{
    m_setting->setValue("Setting/"+k,v);
}

QString FDJConfig::get(const QString &k)
{
    return  m_setting->value("Setting/"+k,"").toString();
}


