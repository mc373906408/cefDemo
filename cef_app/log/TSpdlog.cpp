#include <QDateTime>
#include <QDebug>
#include <QString>
#include <QStringList>

#ifdef WIN32
#define SPDLOG_WCHAR_FILENAMES
#endif

#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#include "TSpdlogAssistant.h"

#include "TSpdlog.h"


#define DEBUG_LOG

HashList<QString,TSpdlog*> TSpdlog::s_logs;

bool TSpdlog::registerLog(const char *identify,const char *path)
{
    if (s_logs.contains(identify)) {
        return false;
    } else {
        //        s_logs[identify] = new TSpdlog(path);
        s_logs.append(identify,new TSpdlog(path));
        return true;
    }
}

TSpdlog* TSpdlog::getInstance(const char *identify){
    if (identify == nullptr && s_logs.size() > 0) {
        //        return (s_logs.begin().value());
        return s_logs.first();
    } else {
        //        FDJ_ASSERT(false,identify,"error");
    }

    if (s_logs.contains(identify)) {
        return s_logs[identify];
    } else {
        static TSpdlog useless_instance("d:/logs/useless_identify.txt");
        //        FDJ_ASSERT(false,identify,"log name not register!");
        return &useless_instance;
    }
}

TSpdlog::TSpdlog(const char *identify)
{
    // actionlog basic log not allow equal name
    static qint64 ms = QDateTime::currentMSecsSinceEpoch();
    qint64 currentMs = QDateTime::currentMSecsSinceEpoch();
    if (ms >= currentMs) {
        ms++;
    } else {
        ms = currentMs;
    }

    QString name = QString::number(ms);
    QString fileName = identify;

    try {
#ifdef WIN32
        m_file_logger = spdlog::basic_logger_mt(name.toStdString().c_str(),fileName.toStdWString());
#else
        m_file_logger = spdlog::basic_logger_mt(name.toStdString().c_str(),fileName.toStdString());
#endif
        spdlog::set_default_logger(m_file_logger);
        m_file_logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e][thread %t][%l] : %v");
    } catch (const spdlog::spdlog_ex& ex) {
        qDebug() << "Log initialization failed: " << ex.what() ;
    }
    if (m_file_logger) {
        /* 仅当发生错误立即刷新，已经累积的其他消息也会刷进去,程序异常结束将可能日志丢失 */
        m_file_logger->flush_on(spdlog::level::err);
        spdlog::flush_every(std::chrono::seconds(5));
    } else {
        qDebug() << "init log error";
    }
}

TSpdlog::~TSpdlog()
{
    for (auto log : s_logs.keys()) {
        s_logs.value(log)->m_file_logger->flush();
    }
    spdlog::drop_all();
}

void TSpdlog::info(const char *msg)
{
    if(m_file_logger)
        m_file_logger->info(msg);
}

void TSpdlog::error(const char *msg)
{
    if(m_file_logger) {
        m_file_logger->error(msg);
        m_file_logger->flush();
    }
}

void TSpdlog::warn(const char *msg)
{
    if (m_file_logger)
        m_file_logger->warn(msg);
}

void TSpdlog::critical(const char *msg)
{
    if (m_file_logger)
        m_file_logger->critical(msg);
}

void TSpdlog::uiClick(const QString &info)
{
    QString type = TSpdlogAssistant().getTypeName(UI_CLICK);
    QString result = " [" + type + "]" + " " + info;
#ifdef DEBUG_LOG
    qDebug() << "[Click:]" << result;
#endif
    this->info(result.toStdString().c_str());
    this->flush();
}

void TSpdlog::avInfo(const QString &info)
{
    QString type = TSpdlogAssistant().getTypeName(AV_TYPE);
    QString result = " [" + type + "]" + " " + info;
#ifdef DEBUG_LOG
    qDebug() << "[avInfo]" << result;
#endif
    result.replace("\n","");
    this->info(result.toStdString().c_str());
}

void TSpdlog::avWarn(const QString &msg)
{
    QString type = TSpdlogAssistant().getTypeName(AV_TYPE);
    QString result = " [" + type + "]" + " " + msg;
#ifdef DEBUG_LOG
    qDebug() << "[avWarn]" << result;
#endif
    this->warn(result.toStdString().c_str());
}

void TSpdlog::avError(const QString &msg)
{
    QString type = TSpdlogAssistant().getTypeName(AV_TYPE);
    QString result = " [" + type + "]" + " " + msg;
#ifdef DEBUG_LOG
    qDebug() << "[avError]" << result;
#endif
    this->error(result.toStdString().c_str());
}

void TSpdlog::netInfo(const QString &info)
{
    QString type = TSpdlogAssistant().getTypeName(NET_Type);
    QString result = " [" + type + "]" + " " + info;
#ifdef DEBUG_LOG
    qDebug() << "[netInfo]" << result;
#endif
    result.replace("\n","");
    this->info(result.toStdString().c_str());
}

void TSpdlog::netWarn(const QString &info)
{
    QString type = TSpdlogAssistant().getTypeName(NET_Type);
    QString result = " [" + type + "]" + " " + info;
#ifdef DEBUG_LOG
    qDebug() << "[netWarn]" << result;
#endif
    this->warn(result.toStdString().c_str());
}

void TSpdlog::netError(const QString &info)
{
    QString type = TSpdlogAssistant().getTypeName(NET_Type);
    QString result = " [" + type + "]" + " " + info;
#ifdef DEBUG_LOG
    qDebug() << "[netError]" << result;
#endif
    this->error(result.toStdString().c_str());
}

void TSpdlog::otherInfo(const QString &msg)
{
    QString type = TSpdlogAssistant().getTypeName(Other_Type);
    QString result = " [" + type + "]" + " " + msg;
#ifdef DEBUG_LOG
    qDebug() << "[OtherType]" << result;
#endif
    result.replace("\n","");
    this->info(result.toStdString().c_str());
}

void TSpdlog::otherWarn(const QString &msg)
{
    QString type = TSpdlogAssistant().getTypeName(Other_Type);
    QString result = " [" + type + "]" + " " + msg;
#ifdef DEBUG_LOG
    qDebug() << "[OtherType]" << result;
#endif
    this->warn(result.toStdString().c_str());
}

void TSpdlog::otherError(const QString &info)
{
    QString type = TSpdlogAssistant().getTypeName(Other_Type);
    QString result = " [" + type + "]" + " " + info;
#ifdef DEBUG_LOG
    qDebug() << "[OtherType]" << result;
#endif
    this->error(result.toStdString().c_str());
}

void TSpdlog::flush()
{
    if (m_file_logger)
        m_file_logger->flush();
}

void TSpdlog::setInfoFlush()
{
    if (m_file_logger)
        m_file_logger->flush_on(spdlog::level::info);
}

void TSpdlog::setErrFlush()
{
    if (m_file_logger)
        m_file_logger->flush_on(spdlog::level::err);
}
