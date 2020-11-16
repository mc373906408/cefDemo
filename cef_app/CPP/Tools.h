#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QDir>
#include <QRect>
#include <QCursor>

#include "log/TSpdlog.h"

class Tools : public QObject
{
    Q_OBJECT
public:
    static Tools& getInstance(){
       static Tools instance;
       return instance;
     }

    /**
     * @brief getMainWindowRECT 获取主窗口相对坐标
     * @return
     */
    QRectF getMainWindowRECT();



    Q_INVOKABLE QString formatPath(const QString &path);
    Q_INVOKABLE QString getFileName(const QString &fullName);

    /*鼠标在主窗口时，判断屏幕序号*/
    Q_INVOKABLE int getMouseDesktopInt(QPoint mouse);
    /*实际主桌面RECT*/
    Q_INVOKABLE QRect getDesktopRect();
    /*实际当前桌面RECT*/
    Q_INVOKABLE QRect getNowDesktopRect(QPoint mouse);
    /*可用主桌面RECT，不含任务栏*/
    Q_INVOKABLE QRect getAvailableRect();
    /*可用当前桌面RECT，不含任务栏*/
    Q_INVOKABLE QRect getNowAvailableRect(QPoint mouse);
    /*主桌面DPI*/
    Q_INVOKABLE double getZoomFactor();
    /*当前桌面DPI*/
    Q_INVOKABLE double getNowZoomFactor(QPoint mouse);

    Q_INVOKABLE int getDesktopWidth() {return getDesktopRect().width(); }
    Q_INVOKABLE int getDesktopHeight() { return getDesktopRect().height(); }
    Q_INVOKABLE int getDesktopX() {return getDesktopRect().x(); }
    Q_INVOKABLE int getDesktopY() { return getDesktopRect().y(); }

    Q_INVOKABLE int getNowDesktopWidth(QPoint mouse) {return getNowDesktopRect(mouse).width(); }
    Q_INVOKABLE int getNowDesktopHeight(QPoint mouse) { return getNowDesktopRect(mouse).height(); }
    Q_INVOKABLE int getNowDesktopX(QPoint mouse) {return getNowDesktopRect(mouse).x(); }
    Q_INVOKABLE int getNowDesktopY(QPoint mouse) { return getNowDesktopRect(mouse).y(); }

    Q_INVOKABLE int getAvailableWidth() {return getAvailableRect().width(); }
    Q_INVOKABLE int getAvailableHeight() { return getAvailableRect().height(); }
    Q_INVOKABLE int getAvailableX() {return getAvailableRect().x(); }
    Q_INVOKABLE int getAvailableY() { return getAvailableRect().y(); }

    Q_INVOKABLE int getNowAvailableWidth(QPoint mouse) {return getNowAvailableRect(mouse).width(); }
    Q_INVOKABLE int getNowAvailableHeight(QPoint mouse) { return getNowAvailableRect(mouse).height(); }
    Q_INVOKABLE int getNowAvailableX(QPoint mouse) {return getNowAvailableRect(mouse).x(); }
    Q_INVOKABLE int getNowAvailableY(QPoint mouse) { return getNowAvailableRect(mouse).y(); }


    Q_INVOKABLE QPoint getMousePos(){return QCursor::pos();}

    /**
     * @brief getFileNames  获取文件夹下指定类型不带目录的文件列表
     * @param path  目录
     * @param type  类型 *.*
     * @return
     */
    QStringList getFileNames(const QString &path,const QStringList &type);
    /**
     * @brief getFileNames 重载，递归获取所有带目录的文件列表（包括子目录）
     * @param path
     * @param fileList  文件列表   xxx\xxx.*   or   xxx.*
     * @param dirList   带目录的文件列表    x:\xxx\xxx\xxx.*  or x:\xxx\xxx.*
     * @return
     */
    void getFileNames(const QString &path,QHash<QString/*fileList*/,QString /*dirList*/> &list);
    /**
     * @brief getFileNames 重载，递归获取所有带目录的指定类型文件列表（包括子目录）
     * @param path
     * @param fileList  文件列表   xxx\xxx.*   or   xxx.*
     * @param dirList  带目录的文件列表    x:\xxx\xxx\xxx.*  or x:\xxx\xxx.*
     * @param type  类型 *.*
     */
    void getFileNames(const QString &path,QHash<QString/*fileList*/,QString /*dirList*/> &list,const QStringList &type);

    /**
     * @brief getJsonToMap  json转map
     * @param json
     * @param key   键
     * @return
     */
    QVariantMap getJsonToMap(const QString &json);

    /**
     * @brief QStringList_removeDuplicates List去重复值
     * @param that
     * @return
     */
    template < class T>
    void removeDuplicates(QList<T> *list)
    {
        int n = list->size();
        int j = 0;
        QSet<T> seen;
        seen.reserve(n);
        int setSize = 0;
        for (int i = 0; i < n; ++i) {
            const T &s = list->at(i);
            seen.insert(s);
            if (setSize == seen.size()) // unchanged size => was already seen
                continue;
            ++setSize;
            if (j != i)
                list->swap(i, j);    //将不重复项与重复项交换
            ++j;
        }
        if (n != j)
        {
            list->erase(list->begin() + j, list->end());
        }
    }



    /**
     * @brief quitApp 退出主程序
     */
    Q_INVOKABLE void quitApp();

    /**
     * @brief restartApp 重启客户端
     */
    Q_INVOKABLE void restartApp();

    /**
     * @brief reinstall 重新安装客户端
     */
    Q_INVOKABLE void reinstall();

    /**
     * @brief startProcess 启动指定进程，分离启动
     * @param name
     * @param list  启动参数
     */
    bool startProcess(const QString &name);
    Q_INVOKABLE void startLocalProcess(const QString &name);

//#ifdef Q_OS_WIN
    /**
     * @brief getTask 判断进程是否已经启动
     * @param process
     * @return
     */
    bool getTask(QString process,unsigned long &processId);
    /*重载*/
    bool getTask(QString process,QList<int> &processId);
    /**
     * @brief killProcess 杀死指定进程
     * @param name
     */
    Q_INVOKABLE void killProcess(const QString &process);

//#endif
    /**
     * @brief deleteDirectory 递归删除非空文件夹
     * @param path
     */
    bool deleteDirectory(const QString &path);

    /**
     * @brief deleteFile 删除指定文件
     * @param filename
     * @return
     */
    void deleteFile(const QString &file);

    /**
     * @brief createDirectory 创建目录
     * @param path
     * @param dirname
     * @return
     */
    bool createDirectory(const QString &path,const QString &dirname);

    /**
     * @brief createDirectory 重载 创建目录
     * @param path
     * @return
     */
    bool createDirectory(const QString &path);

    /**
     * @brief copyDirectoryFiles 复制文件或文件夹到另一个文件夹
     * @param fromDir   来自
     * @param toDir   去
     * @param coverFileIfExist  已存在是否覆盖
     * @return
     */
    bool copyDirectoryFiles(const QDir &fromDir, const QDir &toDir, bool coverFileIfExist=true);
    /**
     * @brief setMute 静音
     * @param Silence
     */
    void setMute(bool Silence);

private:
    Tools();
    ~Tools();

    Tools(const Tools &sg) = delete;
    Tools &operator=(const Tools &sg) = delete;

    void recursiveFileNames(const QString &path,QStringList &filesPath);

private:
//    QQmlApplicationEngine* engine;

    QTime *m_Time;

public slots:


signals:
    void sgActive(QString objectName);
    void sgInactive();

};
#endif // TOOLS_H
