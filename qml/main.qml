import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12


Item {
    visible: true
    width: 1300
    height: 1000
//    signal sgbutton()
//    title: qsTr("Hello World")

    Rectangle{
        objectName:"webview"
        width: 800
        height:600
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        color: "#F4D6D6"
    }
    Button{
        width: 200
        height: 100
        onClicked: {
            cpphello.qtest()
        }
    }
}
