#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include "KeybindModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Set the application name and organization
    QGuiApplication::setApplicationName("Hypr-Keys");
    QGuiApplication::setOrganizationName("FredonBytes");
    
    // Register the KeybindModel type for use in QML
    qmlRegisterType<KeybindModel>("KeybindModel", 1, 0, "KeybindModel");
    
    // Create the QML engine
    QQmlApplicationEngine engine;
    
    // Create and expose the keybind model to QML
    KeybindModel *keybindModel = new KeybindModel(&engine);
    engine.rootContext()->setContextProperty("keybindModel", keybindModel);
    
    // Try to load QML file from the application directory first
    QString appDir = QCoreApplication::applicationDirPath();
    QString qmlPath = appDir + "/main.qml";
    
    // If not found, try current directory
    if (!QFile::exists(qmlPath)) {
        qmlPath = "main.qml";
    }
    
    // Load the main QML file
    engine.load(QUrl::fromLocalFile(qmlPath));
    
    if (engine.rootObjects().isEmpty()) {
        qDebug() << "Failed to load QML file:" << qmlPath;
        return -1;
    }
    
    // Load the keybinds
    keybindModel->loadKeybinds();
    
    return app.exec();
}
