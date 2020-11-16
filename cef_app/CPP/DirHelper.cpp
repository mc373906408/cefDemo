#include "DirHelper.h"

#include "FDJ_Macro.h"

#include <QDir>
#include <QDebug>
#include <QVector>
#include <QFileInfoList>
#include <QStandardPaths>
#include <QSettings>
#include <QCoreApplication>

#ifdef WIN32
#include <Windows.h>
#endif
#include <algorithm>
#include <iostream>

#define REG_FDJ_APP_DATA "FDJ_APP_DATA"

#ifdef WIN32
const int GB = 1024*1024*1024;
#endif

quint64 getDiskRemainSpace(QString driver)
{
#ifdef WIN32
    if (driver.isEmpty())
        return 0;
    LPCWSTR strDriver=(LPCWSTR)driver.utf16();
    ULARGE_INTEGER freeDiskSpaceAvailable, totalDiskSpace, totalFreeDiskSpace;

    GetDiskFreeSpaceEx( strDriver, &freeDiskSpaceAvailable, &totalDiskSpace, &totalFreeDiskSpace) ;
    return (quint64) totalFreeDiskSpace.QuadPart/GB;
#else
    Q_UNUSED(driver);
    return 0;
#endif
}

quint64 getDiskTotalSpace(QString driver)
{
#ifdef WIN32
    if (driver.isEmpty())
        return 0;
    LPCWSTR strDriver=(LPCWSTR)driver.utf16();
    ULARGE_INTEGER freeDiskSpaceAvailable, totalDiskSpace, totalFreeDiskSpace;

    //调用函数获取磁盘参数(单位为字节Byte),转化为GB，需要除以(1024*1024*1024)
    GetDiskFreeSpaceEx( strDriver, &freeDiskSpaceAvailable, &totalDiskSpace, &totalFreeDiskSpace) ;
    return (quint64) totalDiskSpace.QuadPart/GB;
#else
    Q_UNUSED(driver);
    return 0;
#endif
}

//获取电脑所有盘符名
QStringList getDiskNames()
{
    QFileInfoList list = QDir::drives();
    QStringList diskNameList;

    for (int i=0; i<list.count(); i++) {
        QString str = list.at(i).absoluteDir().absolutePath();
        diskNameList.append(str);
    }
    return diskNameList;
}

QString getSuggestDir()
{
    /* 优先返回用户的路径，当大小小于1G后，返回最大磁盘的路径（创建以隐藏状态） */
    QString dir = QDir::homePath() + "/" + ".fudaojun";//C:/Users/home

#ifdef WIN32
    QStringList list = getDiskNames();

    for(const QString &path : list) {
        if (dir.contains(path)) {
            if (getDiskRemainSpace(path) > 1) {
                return QDir::toNativeSeparators(dir);
            }
        }
    }

    QVector<int> driverSpaces(list.size());
    for (int i = 0; i < list.size(); ++i) {
        driverSpaces[i] = getDiskRemainSpace(list.at(i));
    }

    QVector<int>::iterator big = std::max_element(driverSpaces.begin(),driverSpaces.end());
    if (big != driverSpaces.end()) {
        int index = big - driverSpaces.begin();
        QString prefix = list.at(index);
        return prefix.remove("/") + QDir::separator() + ".fudaojun";
    }

    // 找到最大磁盘
    return dir;
#else
    return "";
#endif
}

DirHelper * DirHelper::m_instance = nullptr;
DirHelper *DirHelper::getInstance()
{
    if (!m_instance) {
        m_instance = new DirHelper;
    }
    return m_instance;
}

void DirHelper::createDir()
{
    if (!QFileInfo::exists(m_mainPath)) {
        if (!QDir().mkpath(m_mainPath) ) {
            FDJ_ASSERT(false,"create dir error","main path");
        }
    }

    if (QFileInfo::exists(m_mainPath)) {
        QDir dir(m_mainPath);
        if (!dir.exists(log)) {
            if (!dir.mkdir(log)) {
                FDJ_ASSERT(false,"create dir error","log path");
            }
        }

        QDir logdir(logPath());
        if (!logdir.exists(logMedia)) {
            if (!logdir.mkdir(logMedia)) {
                FDJ_ASSERT(false,"create dir error","log media path");
            }
        }
        if (!logdir.exists(logTRtc)) {
            if (!logdir.mkdir(logTRtc)) {
                FDJ_ASSERT(false,"create dir error","log media path");
            }
        }
        if (!logdir.exists(logAgora)) {
            if (!logdir.mkdir(logAgora)) {
                FDJ_ASSERT(false,"create dir error","log media path");
            }
        }

        if (!dir.exists(config)){
            if (!dir.mkdir(config))
                FDJ_ASSERT(false,"create dir error","config path");
        }


        if (!dir.exists(record)) {
            if (!dir.mkdir(record))
                FDJ_ASSERT(false,"create dir error","record path");
        }

        if (!dir.exists(dump)) {
            if (!dir.mkdir(dump))
                FDJ_ASSERT(false,"create dir error","dump path");
        }
        if (!dir.exists(db)) {
            if (!dir.mkdir(db))
                FDJ_ASSERT(false,"create dir error","db path");
        }
        if (!dir.exists(download)) {
            if (!dir.mkdir(download))
                FDJ_ASSERT(false,"create dir error","download path");
        }
        if (!dir.exists(cefCache)) {
            if (!dir.mkdir(cefCache))
                FDJ_ASSERT(false,"create dir error","cefCache path");
        }
    } else {
        FDJ_ASSERT(false,"create dir error","av path");
    }
}

DirHelper::DirHelper(QObject *parent) : QObject(parent)
{
    /* 同一组织下的多个app，每个对应自己的数据存储区域，应使application不同，安装默认为用户目录（一般位于C盘），当空间不够时，
     * 改变为其他盘符 */
    QSettings setting(FDJ_COMPANY_NAME,FDJ_COMPANY_APP_NAME);
    QString app_data = setting.value(REG_FDJ_APP_DATA,"none").toString();
    if (app_data == "none") {
        /* C:/Users/sl/AppData/Local/wisezone/fudaojun 必须先设置全局app */
        m_mainPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
        setting.setValue(REG_FDJ_APP_DATA,m_mainPath);
    } else {
        m_mainPath = app_data;
    }
    createDir();
}

bool DirHelper::changeDirPath(const QString &path)
{
#ifdef WIN32
    QString newPath = path;
    newPath = newPath.remove("file:///");
#else
    QString newPath = path;
    newPath = newPath.remove("file://");
#endif
    if (newPath.contains(QRegExp("[\\x4e00-\\x9fa5]+"))) {
        return false;
    } else {
        QSettings setting(FDJ_COMPANY_NAME,FDJ_COMPANY_APP_NAME);
        /* 检查path合法性 */
        if (m_mainPath == newPath + "/fudaojun")
            return true;

        setting.setValue(REG_FDJ_APP_DATA,newPath + "/fudaojun");
        m_mainPath = setting.value(REG_FDJ_APP_DATA).toString();
        createDir();
        return true;
    }
}

void DirHelper::resetMainPath()
{
    QSettings setting(FDJ_COMPANY_NAME,FDJ_COMPANY_APP_NAME);
    m_mainPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    setting.setValue(REG_FDJ_APP_DATA,m_mainPath);
    createDir();
}

QString DirHelper::getCurrentDirPath() const
{
    return m_mainPath;
}

QString DirHelper::configPath()
{
    return m_mainPath + "/" + this->config;
}

QString DirHelper::logPath()
{
    return m_mainPath + "/" + this->log;
}

QString DirHelper::logPathWithMedia()
{
    return logPath() + "/" + this->logMedia;
}
QString DirHelper::logPathWithTRtc()
{
    return logPath() + "/" + this->logTRtc;
}
QString DirHelper::logPathWithAgora()
{
    return logPath() + "/" + this->logAgora;
}

QString DirHelper::dumpPath()
{
    return m_mainPath + "/" + this->dump;
}

QString DirHelper::recordPath()
{
    return  m_mainPath + "/" + this->record;
}
QString DirHelper::dbPath()
{
    return  m_mainPath+"/" + this->db;
}
QString DirHelper::downloadPath()
{
    return m_mainPath+"/" + this->download;
}
QString DirHelper::updatePath()
{
    return m_mainPath+"/"+this->update;
}
QString DirHelper::cefCachePath()
{
    return m_mainPath+"/"+this->cefCache;
}

QString DirHelper::getAppPath()
{
    return QCoreApplication::applicationDirPath();
}
QString DirHelper::getAppData()
{
    return m_mainPath;
}

QString DirHelper::toPlatformPath(const QString &path)
{
    return QDir::toNativeSeparators(path);
}

QString DirHelper::macResourcesPath()
{
#ifdef Q_OS_MAC
    QDir d(QCoreApplication::applicationDirPath());
    d.cdUp();
    d.cd("Resources");
    return d.canonicalPath();
#endif
    return QCoreApplication::applicationDirPath();
}
