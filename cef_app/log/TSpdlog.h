#ifndef TKG_SPD_LOG_H
#define TKG_SPD_LOG_H

#include <QHash>
#include <QObject>
#include <CPP/HashList.h>

/*
  设计目标：支持全局访问；可创建多个log；访问log时能够发现因为字符串写错的报告
  线程不安全
  初衷：先注册，每次注册对应一个独立的log文件，单例参数是获取每个文件，并继续调用info或error等api写入日志文件。
  注意：由于spd的设计，basic_logger_mt的第一个参数，需要指定为不同的标识。刷新也是最后一次设置为可用状态（info，warn，error）
  useage:
    char * mainLog = "xxx";
    TSpdlog::registerLog(mainLog);
    TSpdlog::registerLog(anotherLog);

    TSpdlog::getInstance(); // equal to TSpdlog::getInstance(mainLog)
    TSpdlog::getInstance(anotherLog).xxx(); // use anotherlog instance
*/

#ifdef __GNUC__
#    define FDJ_GCC_VERSION_AT_LEAST(x,y) (__GNUC__ > (x) || __GNUC__ == (x) && __GNUC_MINOR__ >= (y))
#    define FDJ_GCC_VERSION_AT_MOST(x,y)  (__GNUC__ < (x) || __GNUC__ == (x) && __GNUC_MINOR__ <= (y))
#else
#    define FDJ_GCC_VERSION_AT_LEAST(x,y) 0
#    define FDJ_GCC_VERSION_AT_MOST(x,y)  0
#endif

#if FDJ_GCC_VERSION_AT_LEAST(3,1)
#    define attribute_deprecated __attribute__((deprecated))
#elif defined(_MSC_VER)
#    define attribute_deprecated __declspec(deprecated)
#else
#    define attribute_deprecated
#endif

#include "spdlog/spdlog.h"

class TSpdlog : public QObject{
    Q_OBJECT
public:
    static bool registerLog(const char *identify, const char *path);
    static TSpdlog *getInstance(const char *identify = nullptr);

public:
    //@normal
    Q_INVOKABLE void avInfo(const QString &info);
    Q_INVOKABLE void avWarn(const QString &msg);
    Q_INVOKABLE void avError(const QString &msg);

    Q_INVOKABLE void netInfo(const QString &info);
    Q_INVOKABLE void netWarn(const QString &info);
    Q_INVOKABLE void netError(const QString &info);


    Q_INVOKABLE void otherInfo(const QString &msg);
    Q_INVOKABLE void otherWarn(const QString &msg);
    Q_INVOKABLE void otherError(const QString &msg);


    Q_INVOKABLE void uiClick(const QString &info);

    Q_INVOKABLE void flush();
    Q_INVOKABLE void setInfoFlush();
    Q_INVOKABLE void setErrFlush();

    virtual ~TSpdlog();

private:
    static HashList<QString,TSpdlog*> s_logs;

private:
    void info(const char *);
    void error(const char *);
    void warn(const char *);
    void critical(const char *);
private:
    TSpdlog(const char *identify);

    std::shared_ptr<spdlog::logger> m_file_logger;

    TSpdlog(const TSpdlog &sg) = delete;
    TSpdlog &operator=(const TSpdlog &sg) = delete;
};
#endif
