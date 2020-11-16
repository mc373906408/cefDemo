#include <QGuiApplication>
#include <QScreen>
#include <QDebug>
#include <QDesktopServices>
#include <QOpenGLContext>
#include <QWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include "Tools.h"
#include "Tools_QWidget.h"
#include <QApplication>


#include "log/TSpdlog.h"

#ifdef Q_OS_WIN
#include <tchar.h>
#include <TlHelp32.h>
#include <Windows.h>
#include <mmdeviceapi.h>
#include <audiopolicy.h>

#define SAFE_RELEASE(punk) if((punk)!=nullptr) { (punk)->Release(); (punk)=nullptr; }
#define EXIT_ON_ERROR(hres) if(FAILED(hres)) { goto Exit; }
#define FAILED_ON_CONTINUE(hr) if(FAILED((hr))) {goto Exit_This_Loop;}
#endif


Tools::Tools()
{
    m_Time=new QTime;
}

Tools::~Tools()
{

}

QString Tools::formatPath(const QString &path)
{
#ifdef WIN32
    QString newPath = path;
    return newPath.remove("file:///");
#else
    QString newPath = path;
    return newPath.remove("file://");
#endif
}

QString Tools::getFileName(const QString &fullName)
{
    QFileInfo info(fullName);
    return info.fileName();
}

QRectF Tools::getMainWindowRECT()
{
    if(qApp->focusWindow())
    {
        return qApp->focusWindow()->geometry();
    }
    return QRectF();
}

int Tools::getMouseDesktopInt(QPoint mouse)
{
    auto lscreens = QGuiApplication::screens();
    int i =0;

    foreach(auto scr ,lscreens){
        int x1=scr->geometry().x();
        int y1=scr->geometry().y();
        int x2=scr->geometry().width()+x1;
        int y2=scr->geometry().height()+y1;
        if(mouse.x()>=x1&&mouse.x()<=x2)
        {
            if(mouse.y()>=y1&&mouse.y()<=y2)
            {
                return i;
            }
        }
        i++;
    }
    return 0;
}

QRect Tools::getDesktopRect()
{
    QScreen * screen =QGuiApplication::primaryScreen();
    return screen->geometry();
}
QRect Tools::getAvailableRect()
{
    QScreen * screen =QGuiApplication::primaryScreen();
    return  screen->availableGeometry();
}
QRect Tools::getNowDesktopRect(QPoint mouse)
{
    int t=getMouseDesktopInt(mouse);
    auto lscreens = QGuiApplication::screens();
    return lscreens[t]->geometry();

}
QRect Tools::getNowAvailableRect(QPoint mouse)
{
    int t=getMouseDesktopInt(mouse);
    auto lscreens = QGuiApplication::screens();
    return lscreens[t]->availableGeometry();
}

double Tools::getZoomFactor()
{
    return Tools_QWidget::getInstance().getZoomFactor(0);
}

double Tools::getNowZoomFactor(QPoint mouse)
{
    int t=getMouseDesktopInt(mouse);
    return Tools_QWidget::getInstance().getZoomFactor(t);
}

static bool OGLSupports(int major, int minor, bool gles = false, QSurfaceFormat::OpenGLContextProfile profile = QSurfaceFormat::NoProfile)
{
    QOpenGLContext ctx;
    QSurfaceFormat fmt;
    fmt.setVersion(major, minor);
    if (gles) {
        fmt.setRenderableType(QSurfaceFormat::OpenGLES);
    } else {
        fmt.setRenderableType(QSurfaceFormat::OpenGL);
        fmt.setProfile(profile);
    }

    ctx.setFormat(fmt);
    ctx.create();
    if (!ctx.isValid())
        return false;
    int ctxMajor = ctx.format().majorVersion();
    int ctxMinor = ctx.format().minorVersion();
    bool isGles = (ctx.format().renderableType() == QSurfaceFormat::OpenGLES);

    if (isGles != gles) return false;
    if (ctxMajor < major) return false;
    if (ctxMajor == major && ctxMinor < minor)
        return false;
    if (!gles && ctx.format().profile() != profile)
        return false;
    return true;
}


QStringList Tools::getFileNames(const QString &path, const QStringList &types)
{
    QDir dir(path);
    QStringList files = dir.entryList(types, QDir::Files|QDir::Readable, QDir::Name);
    return files;
}
void Tools::getFileNames(const QString &path,QHash<QString/*fileList*/,QString /*dirList*/> &list)
{
    QString m_path=path;
    m_path.replace("\\","/");
    if(m_path.right(1)!='/')
    {
        m_path+='/';
    }
    QStringList filesPath;
    recursiveFileNames(m_path,filesPath);
    QStringList dirList = filesPath;
    QStringList::iterator it;
    for(it=filesPath.begin();it!=filesPath.end();++it)
    {
        it->replace(m_path,"");
    }
    QStringList fileList=filesPath;
    int i=0;
    foreach(QString it,dirList)
    {
        list.insert(fileList.at(i),it);
        i++;
    }
}
void Tools::getFileNames(const QString &path, QHash<QString/*fileList*/,QString /*dirList*/> &list, const QStringList &type)
{
    QHash<QString,QString>m_list;
    Tools::getFileNames(path,m_list);

    QString m_type="(^\\S+\\.(";
    foreach(QString it,type)
    {
        m_type=m_type+it.section('.',-1)+'|';
    }
    if(m_type.right(1)=='|')
    {
        m_type.chop(1);
    }
    m_type+=")$)";

    QRegExp rx(m_type);
    QHash<QString, QString>::const_iterator i;
    for(i=m_list.constBegin();i!=m_list.constEnd();i++)
    {
        rx.indexIn(i.key());
        QString fileList=rx.cap(0);
        rx.indexIn(i.value());
        QString dirList=rx.cap(0);
        if(!fileList.isEmpty()||!dirList.isEmpty())
        {
            list.insert(fileList,dirList);
        }
    }
}
void Tools::recursiveFileNames(const QString &path,QStringList &filesPath)
{
    QDir dir(path);
    foreach(QFileInfo mfi ,dir.entryInfoList())
    {
        if(mfi.isFile())
        {
            filesPath.append(mfi.filePath());
        }else
        {
            if(mfi.fileName()=="." || mfi.fileName() == "..")continue;
            recursiveFileNames(mfi.absoluteFilePath(),filesPath);
        }
    }
}

QVariantMap Tools::getJsonToMap(const QString &json)
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toUtf8());
    QJsonObject jsonObject = jsonDocument.object();
    return jsonObject.toVariantMap();
}



void Tools::quitApp()
{
    TSpdlog::getInstance()->uiClick(QStringLiteral("退出主进程"));
    qApp->quit();
}

void Tools::restartApp()
{
    qApp->quit();
    startProcess(QApplication::applicationFilePath());
}

bool Tools::startProcess(const QString &name)
{
    //#ifdef Q_OS_WIN
    //    unsigned long processId;
    //    if(getTask(name,processId))
    //    {
    //        return true;
    //    }
    //    SHELLEXECUTEINFO shexeinfo;
    //    memset(&shexeinfo, 0, sizeof(shexeinfo));
    //    shexeinfo.cbSize = sizeof(SHELLEXECUTEINFO);
    //    shexeinfo.fMask = SEE_MASK_FLAG_NO_UI;
    //    shexeinfo.hwnd = nullptr;
    //    shexeinfo.lpVerb = QString("open").toStdWString().c_str();
    //    shexeinfo.lpFile = QString(QCoreApplication::applicationDirPath()+"/"+name).toStdWString().c_str();
    //    shexeinfo.lpDirectory = nullptr;
    //    shexeinfo.nShow = SW_HIDE;
    //    ShellExecuteEx(&shexeinfo);
    //    return getTask(name,processId);
    //#else
    //    return QProcess::startDetached(name,QStringList(name));
    //#endif

    unsigned long processId;
    if(getTask(name,processId))
    {
        return true;
    }
    return QProcess::startDetached(name,QStringList(name));
}

void Tools::startLocalProcess(const QString &name)
{
    startProcess(QApplication::applicationDirPath()+"/"+name);
}

bool Tools::getTask(QString process,unsigned long &processId)
{
#ifdef Q_OS_WIN
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnap)
    {
        return false;
    }
    if (Process32First(hSnap, &pe))
    {
        do
        {
            if (lstrcmpi(process.toStdWString().c_str(), pe.szExeFile) == 0)
            {
                processId= pe.th32ProcessID;
                TSpdlog::getInstance()->otherInfo(process+QStringLiteral(" 正在运行"));
                return true;
            }
        } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
    TSpdlog::getInstance()->otherInfo(process+QStringLiteral(" 不在运行"));
    return false; //其余都返回FALSE
#else
    Q_UNUSED(process);
    Q_UNUSED(processId);
    return false;
#endif
}

bool Tools::getTask(QString process,QList<int> &processId)
{
#ifdef Q_OS_WIN
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnap)
    {
        return false;
    }
    if (Process32First(hSnap, &pe))
    {
        do
        {
            if (lstrcmpi(process.toStdWString().c_str(), pe.szExeFile) == 0)
            {
                processId<<pe.th32ProcessID;
                TSpdlog::getInstance()->otherInfo(process+QStringLiteral(" 正在运行"));
            }
        } while (Process32Next(hSnap, &pe));
    }
    CloseHandle(hSnap);
    if(!processId.isEmpty()){
        return true;
    }
    TSpdlog::getInstance()->otherInfo(process+QStringLiteral(" 不在运行"));
    return false; //其余都返回FALSE
#else
    Q_UNUSED(process);
    Q_UNUSED(processId);
    return false;
#endif
}

void Tools::killProcess(const QString &process)
{
#ifdef Q_OS_WIN
    QList<int> processId;
    int pid=_getpid();
    if(getTask(process,processId))
    {
        int res=processId.indexOf(pid);
        if(res!=-1){
            processId.swap(res,processId.size()-1);
        }
        foreach(int i_processId,processId){
            HANDLE hProcess=OpenProcess(PROCESS_TERMINATE,FALSE,i_processId);
            if(hProcess==nullptr)
                continue;
            if(TerminateProcess(hProcess,0))
                TSpdlog::getInstance()->otherInfo(process+QStringLiteral(" 已杀死"));
        }
    }
#endif
}


bool Tools::deleteDirectory(const QString &path)
{
    QString m_path=path;
    m_path.replace("\\","/");
    if(m_path.right(1)!='/')
    {
        m_path+='/';
    }

    QDir dir(m_path);
    if(dir.exists())
    {
        dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
        QFileInfoList fileList = dir.entryInfoList();
        foreach (QFileInfo fi, fileList)
        {
            if (fi.isFile())
            {
                QFile::remove(fi.absoluteFilePath());
            }
            else
            {
                deleteDirectory(fi.absoluteFilePath());
            }
        }
    }
    return dir.rmpath(dir.absolutePath());
}

void Tools::deleteFile(const QString &file)
{
    QFile m_file(file);
    if(m_file.exists())
    {
        QFileInfo fi(file);
        fi.dir().remove(fi.fileName());
    }
}

bool Tools::createDirectory(const QString &path,const QString &dirname)
{
    QDir dir(path);
    if(!dir.exists(dirname))
    {
        if(!dir.mkdir(dirname))
        {
#ifdef Q_OS_WIN
            if(!CreateDirectory(QString(path+"/"+dirname).toStdWString().c_str(),nullptr))
            {
                TSpdlog::getInstance()->otherError(QStringLiteral("创建目录失败:path:")+path+" dirname:"+dirname+" GetLastError:"+QString::number(GetLastError()));
                return false;
            }
#else
            TSpdlog::getInstance()->otherError(QStringLiteral("创建目录失败:path:")+path+" dirname:"+dirname);
            return false;
#endif
        }
    }
    return true;
}

bool Tools::createDirectory(const QString &path)
{
    QString s_path=path;
    s_path.replace("\\","/");

    QString exp;
    if(s_path.right(1)=='/')
    {
        exp="(.*)(/)(.*)(/)";
    }
    else
    {
        exp="(.*)(/)(.*)";
    }
    QRegExp rx(exp);
    rx.indexIn(s_path);
    QString m_path=rx.cap(1);
    QString dirname=rx.cap(3);

    QDir dir(m_path);
    if(!dir.exists(dirname))
    {
        if(!dir.mkdir(dirname))
        {
#ifdef Q_OS_WIN
            if(!CreateDirectory(QString(path+"/"+dirname).toStdWString().c_str(),nullptr))
            {
                TSpdlog::getInstance()->otherError(QStringLiteral("创建目录失败:path:")+path+" dirname:"+dirname+" GetLastError:"+GetLastError());
                return false;
            }
#else
            TSpdlog::getInstance()->otherError(QStringLiteral("创建目录失败:path:")+path+" dirname:"+dirname);
            return false;
#endif
        }
    }
    return true;
}

bool Tools::copyDirectoryFiles(const QDir &fromDir, const QDir &toDir, bool coverFileIfExist)
{
    QDir sourceDir;
    QFileInfoList fileInfoList;
    QFileInfo f_sourceDir(fromDir.path());
    if(f_sourceDir.isDir())
    {
        sourceDir= fromDir;
        fileInfoList = sourceDir.entryInfoList();
    }
    else
    {
        fileInfoList=QFileInfoList()<<f_sourceDir;
    }

    QDir targetDir = toDir;
    if(!targetDir.exists()){    /**< 如果目标目录不存在，则进行创建 */
        if(!targetDir.mkdir(toDir.absolutePath()))
        {
            TSpdlog::getInstance()->otherError(QStringLiteral("创建目录失败:path:")+toDir.absolutePath());
            return false;
        }
    }


    foreach(QFileInfo fileInfo, fileInfoList){
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        if(fileInfo.isDir()){    /**< 当为目录时，递归的进行copy */
            if(!copyDirectoryFiles(fileInfo.filePath(),
                                   targetDir.filePath(fileInfo.fileName()),
                                   coverFileIfExist))
                return false;
        }
        else{            /**< 当允许覆盖操作时，将旧文件进行删除操作 */
            if(coverFileIfExist && targetDir.exists(fileInfo.fileName())){
                targetDir.remove(fileInfo.fileName());
            }

            // 进行文件copy
            if(!QFile::copy(fileInfo.filePath(),
                            targetDir.filePath(fileInfo.fileName()))){
                return false;
            }
        }
    }
    return true;
}

void Tools::setMute(bool Silence)
{
    GUID m_guidMyContext;
    HRESULT hr = S_OK;
    IMMDeviceCollection *pMultiDevice = nullptr;
    IMMDevice *pDevice = nullptr;
    IAudioSessionEnumerator *pSessionEnum = nullptr;
    IAudioSessionManager2 *pASManager = nullptr;
    IMMDeviceEnumerator *m_pEnumerator = nullptr;
    const IID IID_ISimpleAudioVolume = __uuidof(ISimpleAudioVolume);
    const IID IID_IAudioSessionControl2 = __uuidof(IAudioSessionControl2);
    UINT deviceCount = 0;  //设备数目

    HRESULT hComInitRes = CoInitialize(nullptr);
    if (FAILED(hComInitRes))
    {
        return ;
    }

    hr = CoCreateGuid(&m_guidMyContext);
    EXIT_ON_ERROR(hr)

            hr = CoCreateInstance(__uuidof(MMDeviceEnumerator),nullptr, CLSCTX_ALL,__uuidof(IMMDeviceEnumerator),(void**)&m_pEnumerator);
    EXIT_ON_ERROR(hr)

            hr = m_pEnumerator->EnumAudioEndpoints(eRender, DEVICE_STATE_ACTIVE, &pMultiDevice);
    EXIT_ON_ERROR(hr)

            hr = pMultiDevice->GetCount(&deviceCount);
    EXIT_ON_ERROR(hr)

            for (UINT ii = 0; ii<deviceCount; ii++)
    {
        pDevice = nullptr;
        hr = pMultiDevice->Item(ii, &pDevice);
        EXIT_ON_ERROR(hr)

                hr = pDevice->Activate(__uuidof(IAudioSessionManager), CLSCTX_ALL, nullptr, (void**)&pASManager);
        EXIT_ON_ERROR(hr)

                hr = pASManager->GetSessionEnumerator(&pSessionEnum);
        EXIT_ON_ERROR(hr)

                int nCount = 0;
        hr = pSessionEnum->GetCount(&nCount);
        EXIT_ON_ERROR(hr)

                for (int i = 0; i < nCount; i++)
        {
            IAudioSessionControl *pSessionCtrl = nullptr;
            IAudioSessionControl2 *pSessionCtrl2 = nullptr;
            ISimpleAudioVolume *pSimplevol = nullptr;

            hr = pSessionEnum->GetSession(i, &pSessionCtrl);
            FAILED_ON_CONTINUE(hr)

                    hr = pSessionCtrl->QueryInterface(IID_IAudioSessionControl2, (void **)&pSessionCtrl2);
            FAILED_ON_CONTINUE(hr)

                    DWORD pid;   //当前使用该音频session的进程id
            hr = pSessionCtrl2->GetProcessId(&pid);
            FAILED_ON_CONTINUE(hr)

                    hr = pSessionCtrl2->QueryInterface(IID_ISimpleAudioVolume, (void **)&pSimplevol);
            FAILED_ON_CONTINUE(hr)

                    HANDLE hProcessSnap;
            PROCESSENTRY32 pe32;

            hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (hProcessSnap == INVALID_HANDLE_VALUE)
            {
                return;
            }

            pe32.dwSize = sizeof(PROCESSENTRY32);

            if (!Process32First(hProcessSnap, &pe32))
            {
                CloseHandle(hProcessSnap);
                return;
            }

            do
            {
                if (pid == pe32.th32ProcessID)
                {
                    if (lstrcmp(_T("qgamebox_render.exe"), pe32.szExeFile) == 0)
                    {
                        pSimplevol->SetMute(Silence,nullptr);
                    }
                }
            } while (Process32Next(hProcessSnap, &pe32));
            CloseHandle(hProcessSnap);

Exit_This_Loop:
            SAFE_RELEASE(pSessionCtrl)
                    SAFE_RELEASE(pSessionCtrl2)
                    SAFE_RELEASE(pSimplevol)
        }
    }
Exit:
    SAFE_RELEASE(m_pEnumerator)
            SAFE_RELEASE(pASManager)
            SAFE_RELEASE(pSessionEnum)
            SAFE_RELEASE(pDevice)
            SAFE_RELEASE(pMultiDevice)
            if (SUCCEEDED(hComInitRes))
    {
        CoUninitialize();
    }
}

void Tools::reinstall()
{
#ifdef Q_OS_WIN
    if(!startProcess("fudaojun_install.exe"))
    {
        QDesktopServices::openUrl(QUrl("https://www.fudaojun.com/download.html"));
    }
    quitApp();
#endif
}

