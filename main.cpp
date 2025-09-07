#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QDebug>
#include "KeybindModel.h"
#include "ThemeManager.h"

int main(int argc, char *argv[])
{
    // Enable high DPI scaling
    QGuiApplication app(argc, argv);
    
    // Set the application name and organization
    QGuiApplication::setApplicationName("fredon-modal-cheat");
    QGuiApplication::setOrganizationName("fredon");
    
    qDebug() << "Starting fredon-modal-cheat application";
    
    // Register the KeybindModel type for use in QML
    qmlRegisterType<KeybindModel>("KeybindModel", 1, 0, "KeybindModel");
    
    // Create the QML engine
    QQmlApplicationEngine engine;
    
    // Create and expose the keybind model to QML
    KeybindModel *keybindModel = new KeybindModel(&engine);
    engine.rootContext()->setContextProperty("keybindModel", keybindModel);
    
    // Create and expose the theme manager to QML
    ThemeManager *themeManager = new ThemeManager(&engine);
    engine.rootContext()->setContextProperty("themeManager", themeManager);
    
    // List available themes
    QStringList themes = themeManager->availableThemes();
    qDebug() << "Available themes:" << themes;
    
    // Determine QML file path
    QString appDir = QCoreApplication::applicationDirPath();
    QString qmlPath = appDir + "/test.qml";
    
    // If test.qml doesn't exist, try main.qml
    if (!QFile::exists(qmlPath)) {
        qmlPath = appDir + "/main.qml";
        if (!QFile::exists(qmlPath)) {
            qmlPath = "main.qml";
        }
    }
    
    qDebug() << "Loading QML file:" << qmlPath;
    
    // Load the QML file
    engine.load(QUrl::fromLocalFile(qmlPath));
    
    if (engine.rootObjects().isEmpty()) {
        qDebug() << "Failed to load QML file:" << qmlPath;
        return -1;
    }
    
    qDebug() << "QML file loaded successfully";
    
    // Load the keybinds
    qDebug() << "Loading keybinds...";
    keybindModel->loadKeybinds();
    qDebug() << "Keybinds loaded";
    
    qDebug() << "Starting application event loop...";
    int result = app.exec();
    qDebug() << "Application event loop finished with result:" << result;
    
    return result;
}
