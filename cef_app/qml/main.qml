import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Window 2.3
import QtGraphicalEffects 1.0
Item {
    id: root_father
    objectName: "root_father"
    visible: true
    width: 800
    height: 600

    TextField{
        id:m_url
        width: 600
        height: 50
        background:Rectangle{
            color: "#999999"
        }
        placeholderText:"url"
        text: "file:///"+cppDir.getAppPath()+"/html/404.html"
    }
    TextField{
        id:m_urlname
        width: 200
        height: 50
        anchors.top: m_url.bottom
        background:Rectangle{
            color: "#995599"
        }
        placeholderText:"urlname"
        text:"404"
    }
    TextField{
        id:m_fathername
        width: 200
        height: 50
        anchors.top: m_url.bottom
        anchors.left: m_urlname.right
        background:Rectangle{
            color: "#995555"
        }
        placeholderText:"fathername"
    }
    TextField{
        id:m_mode
        width: 200
        height: 50
        anchors.top: m_url.bottom
        anchors.left: m_fathername.right
        background:Rectangle{
            color: "#555555"
        }
        placeholderText:"IE"
    }
    TextField{
        id:m_rect
        width: 200
        height: 50
        anchors.top: m_mode.bottom
        //        anchors.left: m_mode.right
        background:Rectangle{
            color: "#990099"
        }
        placeholderText:"rect"
        text: "0,0,300,500"
    }
    TextField{
        id:m_align
        width: 200
        height: 50
        anchors.top: m_mode.bottom
        anchors.left: m_rect.right
        background:Rectangle{
            color: "#999999"
        }
        placeholderText:"align"
        text: "Center"
    }
    TextField{
        id:m_valign
        width: 200
        height: 50
        anchors.top: m_mode.bottom
        anchors.left: m_align.right
        background:Rectangle{
            color: "#990099"
        }
        placeholderText:"valign"
        text: "VCenter"
    }
    TextField{
        id:m_front
        width: 200
        height: 50
        anchors.top: m_valign.bottom
        //        anchors.left: m_valign.right
        background:Rectangle{
            color: "#999999"
        }
        placeholderText:"front"
    }
    TextField{
        id:m_needHide
        width: 200
        height: 50
        anchors.top: m_valign.bottom
        anchors.left: m_resize.right
        background:Rectangle{
            color: "#555555"
        }
        placeholderText:"needHide"
    }
    TextField{
        id:m_lostFocusHide
        width: 200
        height: 50
        anchors.top: m_valign.bottom
        anchors.left: m_needHide.right
        background:Rectangle{
            color: "#995555"
        }
        placeholderText:"lostFocusHide"
    }

    TextField{
        id:m_toname
        width: 150
        height: 50
        anchors.top: m_needHide.bottom
        //        anchors.left: m_width.right
        background:Rectangle{
            color: "#EEEE77"
        }
        placeholderText:"Call_ToName"
    }

    TextField{
        id:m_tomsg
        width: 600
        height: 50
        anchors.top: m_needHide.bottom
        anchors.left: m_toname.right
        background:Rectangle{
            color: "#EEEE22"
        }
        placeholderText:"Call_ToMsg(JSON)"
    }


    Button{
        id:btn_open
        width: 100
        height: 100
        anchors.verticalCenter: parent.verticalCenter
        text: "open"
        onClicked: {
            cppCef.openWindow(m_urlname.text,m_fathername.text,
                              m_mode.text,m_url.text,m_rect.text,
                              m_align.text,m_valign.text,m_needHide.text,m_lostFocusHide.text,m_front.text)
        }
    }
    Button{
        id:btn_call
        width: 100
        height: 100
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:btn_open.right
        text: "call"
        background: Rectangle{
            color: "#EEEEEE"
        }

        onClicked: {
            cppCef.sendToWeb(m_toname.text,m_tomsg.text)
        }
    }
    Button{
        width: 100
        height: 100
        anchors.verticalCenter: parent.verticalCenter
        anchors.left:btn_call.right
        text: "test"
        background: Rectangle{
            color: "#AAAAAA"
        }

        onClicked: {
            cppCef.test()
        }
    }


    Connections{
        target:cppCef
        property var taskMap:{0:0}
        function onSgOpenNewWindow(father,jsonmsg){
            var json=JSON.parse(jsonmsg)
            var component;
            if(father===null){
                component=itemCompont.createObject(root_father,json)
            }else{
                component=itemCompont.createObject(father,json)
            }

            taskMap[json["objectName"]]=component
        }
        function onSgCloseWindow(name){
            taskMap[name].destroy()
        }
    }


    Component {
        id:itemCompont
        Window{
            id:m_window
            visible:true
            //            color: "transparent"
            property int m_width: 0
            property int m_height:0
            Component.onCompleted:{
                flags=cppWindow.flags
                m_window.setWidth(m_width)
                m_window.setHeight(m_height)
            }
        }
    }

    WebView{
        id: m_webview
        Connections {
            target: cppCef
            function onSgWebMsgReceived(msg) {
                var str = JSON.parse(msg)
                if (str["msgName"] === "callQML") {
                    m_webview.jsobject.callQML(msg)
                }
            }
        }
    }

}



