QT += quick core axcontainer quickwidgets
CONFIG += c++11
CONFIG += qtquickcompiler

TARGET = cef_app

#文件版权信息
include ($$PWD/../project.pri)
#用于指定生产商
QMAKE_TARGET_COMPANY=$${COMPANY}
#版本号
VERSION=$${VAR_VERSION}
#用于声明版权
QMAKE_TARGET_COPYRIGHT=$${COPYRIGHT}
#用于指定产品名称
QMAKE_TARGET_PRODUCT=$${PRODUCT}
#用于描述应用程序
QMAKE_TARGET_DESCRIPTION=cef_app

DEFINES += FDJ_COMPANY_APP_VERSION=\\\"$${VAR_VERSION}\\\"

DEFINES += QT_DEPRECATED_WARNINGS


#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# spdlog [https://github.com/gabime/spdlog 1.4.2]
INCLUDEPATH += $$PWD/../libs/spdlog/include
INCLUDEPATH += $$OUT_PWD

HEADERS += \
    CPP/Tools_QWidget.h\
    CPP/WindowHook.h \
    iebrowser/QIE.h \
    log/TSpdlog.h \
    log/TSpdlogAssistant.h \
    CPP/HashList.h \
    CPP/Tools.h \
    browser/QCefApp.h \
    browser/QCefClient.h \
    browser/QCefContext.h \
    browser/QCefView.h \
    CPP/DirHelper.h \
    CPP/FDJ_Macro.h \
    CPP/FDJConfig.h \



SOURCES += \
    CPP/Tools_QWidget.cpp\
    CPP/WindowHook.cpp \
    iebrowser/QIE.cpp \
    log/TSpdlog.cpp \
    log/TSpdlogAssistant.cpp \
    CPP/Tools.cpp \
    CPP/HashList.cpp \
    browser/QCefApp.cpp \
    browser/QCefClient.cpp \
    browser/QCefContext.cpp \
    browser/QCefView.cpp \
    main.cpp \
    CPP/DirHelper.cpp \
    CPP/FDJConfig.cpp \

RESOURCES += $$PWD/qml/qml.qrc


# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: {

    LIBS += -lUser32 -lWininet

    # release下可调试
    QMAKE_LFLAGS_RELEASE+=/MAP
    QMAKE_CFLAGS_RELEASE += /Zi
    QMAKE_CXXFLAGS_RELEASE += /Zi
    QMAKE_LFLAGS_RELEASE +=/debug /opt:ref

    #cef
    INCLUDEPATH+=$$PWD/../libs/cef
    LIBS+=-L$$PWD/../libs/cef/lib -llibcef_dll_wrapper -llibcef

    #qhook
    INCLUDEPATH+=$$PWD/../libs/qhook/include
    LIBS+=-L$$PWD/../libs/qhook/lib -lQHook


    win32:CONFIG(release, debug|release){
        QMAKE_POST_LINK += xcopy $$system_path($$PWD/../libs/cef/dll) $$system_path($$OUT_PWD/release/) /y &
        QMAKE_POST_LINK += xcopy $$system_path($$PWD/../libs/cef/dll/locales) $$system_path($$OUT_PWD/release/locales/) /y &
        QMAKE_POST_LINK += xcopy $$system_path($$PWD/../libs/cef/dll/swiftshader) $$system_path($$OUT_PWD/release/swiftshader/) /y &
        QMAKE_POST_LINK += xcopy $$system_path($$PWD/../libs/ie/gameplugins) $$system_path($$OUT_PWD/release/gameplugins/) /y &
        QMAKE_POST_LINK += xcopy $$system_path($$PWD/../libs/ie/gamerender) $$system_path($$OUT_PWD/release/gamerender/) /y &
        QMAKE_POST_LINK += xcopy $$system_path($$PWD/res/html) $$system_path($$OUT_PWD/release/html/) /y &
    }
}

