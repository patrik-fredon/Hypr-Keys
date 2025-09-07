import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import KeybindModel 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    width: 900
    height: 400
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: "transparent"
    
    // Start position below the screen (hidden)
    y: Screen.height
    
    // Position at bottom center of screen (final position)
    x: (Screen.width - width) / 2

    // Animation for liquid popup effect
    NumberAnimation {
        id: showAnimation
        target: mainWindow
        property: "y"
        from: Screen.height
        to: Screen.height - mainWindow.height
        duration: 300
        easing.type: Easing.OutCubic
    }

    NumberAnimation {
        id: hideAnimation
        target: mainWindow
        property: "y"
        from: Screen.height - mainWindow.height
        to: Screen.height
        duration: 200
        easing.type: Easing.InCubic
    }

    // Main container with rounded top corners
    Rectangle {
        id: mainContainer
        anchors.fill: parent
        color: "#1e1e2e"
        opacity: 0.95
        radius: 20
        
        // Border
        border {
            color: "#cba6f7"
            width: 1
        }
    }

    // Main content area
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 15

        // Header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "transparent"
            
            RowLayout {
                anchors.fill: parent
                
                Text {
                    text: "Hypr-Keys"
                    font.pixelSize: 20
                    font.bold: true
                    color: "#cba6f7"
                    Layout.alignment: Qt.AlignLeft
                }
                
                Item {
                    Layout.fillWidth: true
                }
                
                // Close button
                Rectangle {
                    width: 30
                    height: 30
                    color: "#313244"
                    radius: 15
                    Layout.alignment: Qt.AlignRight
                    
                    Text {
                        anchors.centerIn: parent
                        text: "✕"
                        color: "#f38ba8"
                        font.pixelSize: 16
                    }
                    
                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            Qt.quit();
                        }
                    }
                }
            }
        }

        // Search/filter field
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "#313244"
            radius: 8
            
            TextField {
                anchors.fill: parent
                anchors.margins: 8
                placeholderText: "Search keybinds..."
                color: "#cdd6f4"
                placeholderTextColor: "#7f849c"
                font.pixelSize: 14
                background: Rectangle {
                    color: "transparent"
                }
                
                onTextChanged: {
                    keybindModel.filterText = text;
                }
                
                Keys.onEscapePressed: {
                    text = "";
                    keybindModel.filterText = "";
                }
            }
        }

        // Table header
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "#313244"
            radius: 8

            RowLayout {
                anchors.fill: parent
                anchors.margins: 10

                Text {
                    Layout.preferredWidth: 150
                    text: "Keybind"
                    font.bold: true
                    color: "#cdd6f4"
                }

                Text {
                    Layout.preferredWidth: 200
                    text: "Command"
                    font.bold: true
                    color: "#cdd6f4"
                }

                Text {
                    Layout.fillWidth: true
                    text: "Description"
                    font.bold: true
                    color: "#cdd6f4"
                }
            }
        }

        // Content area with scrollable table
        ScrollView {
            id: scrollView
            Layout.fillWidth: true
            Layout.fillHeight: true

            onHeightChanged: {
                keybindModel.viewHeight = height
            }

            ListView {
                id: keybindListView
                model: keybindModel
                clip: true
                spacing: 5
                boundsBehavior: Flickable.StopAtBounds

                delegate: Rectangle {
                    width: ListView.view.width
                    height: 50
                    color: index % 2 === 0 ? "#45475a" : "#313244"
                    radius: 6

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 10

                        // Keybind with icon
                        Rectangle {
                            Layout.preferredWidth: 150
                            Layout.fillHeight: true
                            color: "transparent"

                            Row {
                                anchors.centerIn: parent
                                spacing: 8

                                Image {
                                    source: model.icon && model.icon !== "" ? model.icon : ""
                                    width: 20
                                    height: 20
                                    fillMode: Image.PreserveAspectFit
                                    
                                    // Only show placeholder if icon is empty or doesn't exist
                                    visible: model.icon && model.icon !== ""
                                    
                                    // Handle loading errors
                                    onStatusChanged: {
                                        if (status === Image.Error) {
                                            visible = false;
                                        }
                                    }
                                }

                                // Placeholder icon when no icon is available
                                Rectangle {
                                    width: 20
                                    height: 20
                                    color: "transparent"
                                    visible: !model.icon || model.icon === ""
                                    
                                    Text {
                                        anchors.centerIn: parent
                                        text: "🔍"
                                        color: "#f5c2e7"
                                        font.pixelSize: 14
                                    }
                                }

                                Text {
                                    text: model.keybind
                                    font.bold: true
                                    font.pixelSize: 14
                                    color: "#f5c2e7"
                                    Layout.alignment: Qt.AlignVCenter
                                }
                            }
                        }

                        // Command name
                        Text {
                            Layout.preferredWidth: 200
                            text: model.name
                            font.pixelSize: 14
                            color: "#89b4fa"
                            elide: Text.ElideRight
                            Layout.alignment: Qt.AlignVCenter
                        }

                        // Description
                        Text {
                            Layout.fillWidth: true
                            text: model.description
                            font.pixelSize: 14
                            color: "#cdd6f4"
                            wrapMode: Text.WordWrap
                            Layout.alignment: Qt.AlignVCenter
                            elide: Text.ElideRight
                        }
                    }
                }

                // Scroll bar styling
                ScrollBar.vertical: ScrollBar {
                    policy: ScrollBar.AsNeeded
                    width: 10
                    contentItem: Rectangle {
                        implicitWidth: 10
                        radius: width / 2
                        color: "#585b70"
                    }
                }
            }
        }

        // Pagination controls
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 40
            color: "transparent"

            Row {
                anchors.centerIn: parent
                spacing: 15

                Button {
                    text: "◀"
                    enabled: keybindModel.currentPage > 0
                    onClicked: {
                        if (keybindModel.currentPage > 0) {
                            keybindModel.currentPage--;
                        }
                    }
                    background: Rectangle {
                        color: enabled ? "#89b4fa" : "#313244"
                        radius: 6
                    }
                    contentItem: Text {
                        text: "◀"
                        color: enabled ? "#11111b" : "#585b70"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                    }
                }

                Text {
                    text: (keybindModel.currentPage + 1) + " / " + keybindModel.totalPages
                    color: "#cdd6f4"
                    font.pixelSize: 14
                    Layout.alignment: Qt.AlignVCenter
                }

                Button {
                    text: "▶"
                    enabled: keybindModel.currentPage < keybindModel.totalPages - 1
                    onClicked: {
                        if (keybindModel.currentPage < keybindModel.totalPages - 1) {
                            keybindModel.currentPage++;
                        }
                    }
                    background: Rectangle {
                        color: enabled ? "#cba6f7" : "#313244"
                        radius: 6
                    }
                    contentItem: Text {
                        text: "▶"
                        color: enabled ? "#11111b" : "#585b70"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: 14
                    }
                }
            }
        }
    }

    // Initialize on component completion
    Component.onCompleted: {
        // Small delay to ensure window is ready before animation
        showAnimation.start();
    }

    // Close on Escape key
    Keys.onEscapePressed: {
        hideAnimation.start();
        hideAnimation.finished.connect(function() {
            Qt.quit();
        });
    }

    // Make window draggable from header area
    MouseArea {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height: 70
        onPressed: {
            mainWindow.startSystemMove();
        }
    }
}
