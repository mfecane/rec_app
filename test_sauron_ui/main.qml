import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import io.qt.examples.backend 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("Hello World")
    id: wnd

    BackEnd {
        id:backend
    }

    ColumnLayout{
        id: grid
        anchors.fill: parent
        GroupBox{
            title: "Recording"
            Layout.fillWidth: true
            RowLayout{
                anchors.fill: parent
                Button{
                    text: "Start"
                    onClicked: backend.startRecording()
                }
                Button{
                    text: "Stop"
                    onClicked: backend.stopRecording()
                }
            }
        }

        Flickable{


            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            contentHeight: textArea.implicitHeight


            TextArea {
                id:textArea
                selectByMouse: true
                text: backend.outputText
                onTextChanged: backend.outputText = text
            }

            ScrollBar.vertical: ScrollBar { }
        }
    }
}