#ifndef FDJ_MACRO_H
#define FDJ_MACRO_H

#include <QCoreApplication>

#include "log/TSpdlog.h"

#define FDJ_COMPANY_NAME "gamebox"   // orgnization
#define FDJ_COMPANY_URL "gamebox.com"
#define FDJ_COMPANY_APP_NAME "app"        // application


/**
 * 关于git记录版本与调试说明说明
 * 1. 未发布版本字符串始终为空
 * 2. 当一切测试完毕，把最后一次的git log放进去，重新编译包，发布（注意，更改的git字符串，不可上传到git服务器上）
 * 3. 发布版本，始终在主分支如4.0.x，不可在个人分支上发布如：abc_4.0.x
 * 4. 当用户端出现bug，生成了dump文件后，查阅git日志，直接切换到那个git log，把git log填进去，即可保证和当时发布的完全一致
 */
#define FDJ_PUBLISH_GIT_ID  "b71867bf79956231ba7c958b3983dab32669e924"

#define FDJ_ASSERT(flag,title,reason) do{ \
    if (flag == false) { \
      TSpdlog::getInstance()->otherError("Fatal:" + QString(title)+ " " + QString(reason)); \
      exit(0); \
    } \
    }while(false)

/* 单例宏不具备普遍性，因为有的构造函数可能并不一样,而且静态变量也需要在外面 */
/**
 * use info
 * .h Singleton(MyClass)
 * .cpp in front of cpp file,Singleton_IMPL(MyClass)
*/
#define Singleton(ClassName) \
    private: \
    static ClassName *_instance; \
    ClassName(const ClassName &) = delete; \
    ClassName &operator =(const ClassName &) = delete; \
    ClassName(){} \
    public: \
    static ClassName *getInstance() { \
    if (!_instance) \
    _instance = new ClassName; \
    return _instance; \
    }

#define Singleton_IMPL(ClassName) \
    ClassName *ClassName::_instance=nullptr;


#endif // FDJ_MACRO_H
