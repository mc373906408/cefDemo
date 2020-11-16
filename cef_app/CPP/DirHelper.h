#ifndef DIRHELPER_H
#define DIRHELPER_H

#include <QObject>

class DirHelper : public QObject
{
    Q_OBJECT
public:
    static DirHelper *getInstance();
    static QString toPlatformPath(const QString &path);

    Q_INVOKABLE QString configPath();
    Q_INVOKABLE QString logPath();
    Q_INVOKABLE QString logPathWithMedia();
    Q_INVOKABLE QString logPathWithTRtc();
    Q_INVOKABLE QString logPathWithAgora();
    Q_INVOKABLE QString dumpPath();
    Q_INVOKABLE QString recordPath();
    Q_INVOKABLE QString dbPath();
    Q_INVOKABLE QString downloadPath();
    Q_INVOKABLE QString updatePath();
    Q_INVOKABLE QString cefCachePath();
    Q_INVOKABLE QString getAppPath();
    Q_INVOKABLE QString getAppData();
    Q_INVOKABLE QString macResourcesPath();

    Q_INVOKABLE bool changeDirPath(const QString &path);
    Q_INVOKABLE QString getCurrentDirPath() const;
    Q_INVOKABLE void resetMainPath();

private:
    explicit DirHelper(QObject *parent = nullptr);

    void createDir();

    QString m_mainPath;

    const char *log = "log";
    const char *logMedia = "media";
    const char *logTRtc="trtc";
    const char *logAgora="agora";
    const char *config = "config";
    const char *dump = "dump";
    const char *record="record";
    const char *db="db";
    const char *download="download";
    const char *update="update";
    const char *cefCache="cefCache";

    static DirHelper * m_instance;
};

#endif // DIRHELPER_H
