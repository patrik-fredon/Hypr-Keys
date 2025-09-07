#include "ThemeManager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentThemeHash(QString())
{
    // Check for theme changes first
    detectThemeChanges();
    
    // Load default theme
    loadDefaultTheme();
}

void ThemeManager::loadDefaultTheme()
{
    // Try to load from themes.json first
    QString themePath = "themes/themes.json";
    
    // Try to load from application directory
    QString appThemeDir = QCoreApplication::applicationDirPath() + "/themes/";
    QString appThemePath = appThemeDir + "themes.json";
    
    // Try to load from current directory
    QString localThemePath = "themes/themes.json";
    
    if (QFile::exists(appThemePath)) {
        loadThemeFromFile(appThemePath);
    } else if (QFile::exists(localThemePath)) {
        loadThemeFromFile(localThemePath);
    } else {
        // Fallback to hardcoded default theme
        m_primaryColor = QColor("#ffffff");
        m_secondaryColor = QColor("#89b4fa");
        m_backgroundColor = QColor("#000000");
        m_surfaceColor = QColor("#313244");
        m_textColor = QColor("#cdd6f4");
        m_accentColor = QColor("#f5c2e7");
        m_errorColor = QColor("#f38ba8");
        
        emit themeChanged();
    }
}

QColor ThemeManager::primaryColor() const
{
    return m_primaryColor;
}

QColor ThemeManager::secondaryColor() const
{
    return m_secondaryColor;
}

QColor ThemeManager::backgroundColor() const
{
    return m_backgroundColor;
}

QColor ThemeManager::surfaceColor() const
{
    return m_surfaceColor;
}

QColor ThemeManager::textColor() const
{
    return m_textColor;
}

QColor ThemeManager::accentColor() const
{
    return m_accentColor;
}

QColor ThemeManager::errorColor() const
{
    return m_errorColor;
}

bool ThemeManager::loadTheme(const QString &themeName)
{
    // Try to load from user directory first
    QString userThemeDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/fredon-modal-cheat/themes/";
    QString userThemePath = userThemeDir + themeName + ".json";
    
    if (QFile::exists(userThemePath)) {
        return loadThemeFromFile(userThemePath);
    }
    
    // Try to load from application directory
    QString appThemeDir = QCoreApplication::applicationDirPath() + "/themes/";
    QString appThemePath = appThemeDir + themeName + ".json";
    
    if (QFile::exists(appThemePath)) {
        return loadThemeFromFile(appThemePath);
    }
    
    // Try to load from current directory
    QString localThemePath = "themes/" + themeName + ".json";
    if (QFile::exists(localThemePath)) {
        return loadThemeFromFile(localThemePath);
    }
    
    qWarning() << "Theme file not found:" << themeName;
    return false;
}

bool ThemeManager::loadThemeFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open theme file:" << filePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "Invalid theme file format:" << filePath;
        return false;
    }
    
    QJsonObject themeObj = doc.object();
    
    // Load colors from JSON (support both "property" and "m_property" formats for backward compatibility)
    auto getColorValue = [&](const QString &key, const QString &mKey) -> QColor {
        if (themeObj.contains(key) && themeObj[key].isString()) {
            return QColor(themeObj[key].toString());
        } else if (themeObj.contains(mKey) && themeObj[mKey].isString()) {
            return QColor(themeObj[mKey].toString());
        }
        return QColor(); // Return invalid color if not found
    };
    
    QColor primaryColor = getColorValue("primaryColor", "m_primaryColor");
    if (primaryColor.isValid()) m_primaryColor = primaryColor;
    
    QColor secondaryColor = getColorValue("secondaryColor", "m_secondaryColor");
    if (secondaryColor.isValid()) m_secondaryColor = secondaryColor;
    
    QColor backgroundColor = getColorValue("backgroundColor", "m_backgroundColor");
    if (backgroundColor.isValid()) m_backgroundColor = backgroundColor;
    
    QColor surfaceColor = getColorValue("surfaceColor", "m_surfaceColor");
    if (surfaceColor.isValid()) m_surfaceColor = surfaceColor;
    
    QColor textColor = getColorValue("textColor", "m_textColor");
    if (textColor.isValid()) m_textColor = textColor;
    
    QColor accentColor = getColorValue("accentColor", "m_accentColor");
    if (accentColor.isValid()) m_accentColor = accentColor;
    
    QColor errorColor = getColorValue("errorColor", "m_errorColor");
    if (errorColor.isValid()) m_errorColor = errorColor;
    
    emit themeChanged();
    return true;
}

QStringList ThemeManager::availableThemes() const
{
    QStringList themes;
    
    // Check user directory
    QString userThemeDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/fredon-modal-cheat/themes/";
    QDir userDir(userThemeDir);
    if (userDir.exists()) {
        QStringList userThemes = userDir.entryList(QStringList() << "*.json", QDir::Files);
        for (const QString &theme : userThemes) {
            themes.append(theme.left(theme.lastIndexOf('.')));
        }
    }
    
    // Check application directory
    QString appThemeDir = QCoreApplication::applicationDirPath() + "/themes/";
    QDir appDir(appThemeDir);
    if (appDir.exists()) {
        QStringList appThemes = appDir.entryList(QStringList() << "*.json", QDir::Files);
        for (const QString &theme : appThemes) {
            QString themeName = theme.left(theme.lastIndexOf('.'));
            if (!themes.contains(themeName)) {
                themes.append(themeName);
            }
        }
    }
    
    // Check current directory
    QDir localDir("themes/");
    if (localDir.exists()) {
        QStringList localThemes = localDir.entryList(QStringList() << "*.json", QDir::Files);
        for (const QString &theme : localThemes) {
            QString themeName = theme.left(theme.lastIndexOf('.'));
            if (!themes.contains(themeName)) {
                themes.append(themeName);
            }
        }
    }
    
    return themes;
}

QString ThemeManager::calculateThemeFileHash(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open theme file for hashing:" << filePath;
        return QString();
    }
    
    QByteArray fileData = file.readAll();
    file.close();
    
    QCryptographicHash hash(QCryptographicHash::Sha256);
    hash.addData(fileData);
    return QString(hash.result().toHex());
}

bool ThemeManager::detectThemeChanges()
{
    // Find the themes.json file
    QString themePath;
    
    // Try to load from application directory
    QString appThemeDir = QCoreApplication::applicationDirPath() + "/themes/";
    QString appThemePath = appThemeDir + "themes.json";
    
    // Try to load from current directory
    QString localThemePath = "themes/themes.json";
    
    if (QFile::exists(appThemePath)) {
        themePath = appThemePath;
    } else if (QFile::exists(localThemePath)) {
        themePath = localThemePath;
    } else {
        // No theme file found
        return false;
    }
    
    // Calculate current theme hash
    QString currentHash = calculateThemeFileHash(themePath);
    
    // Get stored hash from settings
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    QString storedHash = settings.value("theme/hash").toString();
    
    // Store current hash for next comparison
    m_currentThemeHash = currentHash;
    
    // If hashes differ, theme has changed
    if (!storedHash.isEmpty() && currentHash != storedHash) {
        // Save new hash
        settings.setValue("theme/hash", currentHash);
        settings.sync();
        return true;
    }
    
    // Save current hash if not already stored
    if (storedHash.isEmpty()) {
        settings.setValue("theme/hash", currentHash);
        settings.sync();
    }
    
    return false;
}

bool ThemeManager::checkForThemeChanges()
{
    if (detectThemeChanges()) {
        emit themeFileChanged();
        return true;
    }
    return false;
}