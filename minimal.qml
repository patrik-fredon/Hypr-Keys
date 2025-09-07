import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import KeybindModel 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 400
    height: 300
    title: "Test Window"

    Text {
        anchors.centerIn: parent
        text: "Hello, World!"
        font.pixelSize: 24
    }
}