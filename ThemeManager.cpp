#include "ThemeManager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QDir>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QDebug>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QDateTime>
#include <QFileInfo>

ThemeManager::ThemeManager(QObject *parent)
    : QObject(parent)
    , m_currentThemeHash(QString())
    , m_watcher(nullptr)
{
    // Set default theme directories
    m_themeDirectories << "themes/";
    
    // Setup file watcher for real-time theme updates
    setupFileWatcher();
    
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
        m_currentThemeFile = appThemePath;
        loadThemeFromFile(appThemePath);
    } else if (QFile::exists(localThemePath)) {
        m_currentThemeFile = localThemePath;
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
        m_currentThemeFile = userThemePath;
        return loadThemeFromFile(userThemePath);
    }
    
    // Try to load from application directory
    QString appThemeDir = QCoreApplication::applicationDirPath() + "/themes/";
    QString appThemePath = appThemeDir + themeName + ".json";
    
    if (QFile::exists(appThemePath)) {
        m_currentThemeFile = appThemePath;
        return loadThemeFromFile(appThemePath);
    }
    
    // Try to load from custom theme directories
    for (const QString &dir : m_themeDirectories) {
        QString customThemePath = QDir(dir).filePath(themeName + ".json");
        if (QFile::exists(customThemePath)) {
            m_currentThemeFile = customThemePath;
            return loadThemeFromFile(customThemePath);
        }
    }
    
    // Try to load from current directory
    QString localThemePath = "themes/" + themeName + ".json";
    if (QFile::exists(localThemePath)) {
        m_currentThemeFile = localThemePath;
        return loadThemeFromFile(localThemePath);
    }
    
    qWarning() << "Theme file not found:" << themeName;
    return false;
}

bool ThemeManager::loadThemeFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QString error = QString("Could not open theme file '%1': %2")
                       .arg(filePath, file.errorString());
        qWarning() << error;
        emit themeLoadError(error);
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        QString error = QString("Invalid theme file format: %1").arg(filePath);
        qWarning() << error;
        emit themeLoadError(error);
        return false;
    }
    
    QJsonObject themeObj = doc.object();
    
    // Validate theme before applying
    if (!validateTheme(themeObj)) {
        QString error = QString("Invalid theme data in file: %1").arg(filePath);
        qWarning() << error;
        emit themeLoadError(error);
        return false;
    }
    
    // Load colors from JSON (support both "property" and "m_property" formats for backward compatibility)
    QColor primaryColor = getColorValue(themeObj, "primaryColor", "m_primaryColor");
    if (primaryColor.isValid()) m_primaryColor = primaryColor;
    
    QColor secondaryColor = getColorValue(themeObj, "secondaryColor", "m_secondaryColor");
    if (secondaryColor.isValid()) m_secondaryColor = secondaryColor;
    
    QColor backgroundColor = getColorValue(themeObj, "backgroundColor", "m_backgroundColor");
    if (backgroundColor.isValid()) m_backgroundColor = backgroundColor;
    
    QColor surfaceColor = getColorValue(themeObj, "surfaceColor", "m_surfaceColor");
    if (surfaceColor.isValid()) m_surfaceColor = surfaceColor;
    
    QColor textColor = getColorValue(themeObj, "textColor", "m_textColor");
    if (textColor.isValid()) m_textColor = textColor;
    
    QColor accentColor = getColorValue(themeObj, "accentColor", "m_accentColor");
    if (accentColor.isValid()) m_accentColor = accentColor;
    
    QColor errorColor = getColorValue(themeObj, "errorColor", "m_errorColor");
    if (errorColor.isValid()) m_errorColor = errorColor;
    
    // Update file watcher
    if (m_watcher) {
        m_watcher->addPath(filePath);
    }
    
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
    
    // Check custom theme directories
    for (const QString &dir : m_themeDirectories) {
        QDir customDir(dir);
        if (customDir.exists()) {
            QStringList customThemes = customDir.entryList(QStringList() << "*.json", QDir::Files);
            for (const QString &theme : customThemes) {
                QString themeName = theme.left(theme.lastIndexOf('.'));
                if (!themes.contains(themeName)) {
                    themes.append(themeName);
                }
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

bool ThemeManager::validateTheme(const QJsonObject &themeObj) const
{
    QStringList requiredColors = {"primaryColor", "secondaryColor", "backgroundColor", 
                                 "surfaceColor", "textColor", "accentColor", "errorColor"};
    
    for (const QString &colorKey : requiredColors) {
        // Check both formats (with and without "m_" prefix)
        bool hasKey = themeObj.contains(colorKey) || themeObj.contains("m_" + colorKey);
        if (!hasKey) {
            qWarning() << "Missing color in theme:" << colorKey;
            return false;
        }
        
        // Get the value
        QJsonValue colorValue;
        if (themeObj.contains(colorKey)) {
            colorValue = themeObj[colorKey];
        } else if (themeObj.contains("m_" + colorKey)) {
            colorValue = themeObj["m_" + colorKey];
        }
        
        if (!colorValue.isString()) {
            qWarning() << "Invalid color value type for:" << colorKey;
            return false;
        }
        
        QColor color(colorValue.toString());
        if (!color.isValid()) {
            qWarning() << "Invalid color value for:" << colorKey << "=" << colorValue.toString();
            return false;
        }
    }
    return true;
}

QColor ThemeManager::getColorValue(const QJsonObject &themeObj, const QString &key, const QString &mKey) const
{
    if (themeObj.contains(key) && themeObj[key].isString()) {
        return QColor(themeObj[key].toString());
    } else if (themeObj.contains(mKey) && themeObj[mKey].isString()) {
        return QColor(themeObj[mKey].toString());
    }
    return QColor(); // Return invalid color if not found
}

void ThemeManager::setupFileWatcher()
{
    m_watcher = new QFileSystemWatcher(this);
    connect(m_watcher, &QFileSystemWatcher::fileChanged,
            this, &ThemeManager::onThemeFileChanged);
}

void ThemeManager::onThemeFileChanged(const QString &path)
{
    // Reload theme when file changes
    if (path == m_currentThemeFile) {
        loadThemeFromFile(m_currentThemeFile);
        emit themeFileChanged();
    }
}

ThemeManager::Theme ThemeManager::getCurrentTheme() const
{
    Theme theme;
    theme.primaryColor = m_primaryColor;
    theme.secondaryColor = m_secondaryColor;
    theme.backgroundColor = m_backgroundColor;
    theme.surfaceColor = m_surfaceColor;
    theme.textColor = m_textColor;
    theme.accentColor = m_accentColor;
    theme.errorColor = m_errorColor;
    return theme;
}

void ThemeManager::restoreTheme(const Theme &theme)
{
    m_primaryColor = theme.primaryColor;
    m_secondaryColor = theme.secondaryColor;
    m_backgroundColor = theme.backgroundColor;
    m_surfaceColor = theme.surfaceColor;
    m_textColor = theme.textColor;
    m_accentColor = theme.accentColor;
    m_errorColor = theme.errorColor;
    emit themeChanged();
}

bool ThemeManager::previewTheme(const QString &themeName)
{
    // Save current theme
    Theme current = getCurrentTheme();
    
    // Try to load new theme
    if (loadTheme(themeName)) {
        emit themePreviewStarted();
        return true;
    }
    
    // Restore if failed
    restoreTheme(current);
    return false;
}

void ThemeManager::applyPreviewedTheme()
{
    emit themePreviewEnded();
    // In a real implementation, you might want to save the current theme as the new default
}

bool ThemeManager::exportTheme(const QString &themeName, const QString &filePath)
{
    // This is a simplified implementation
    // In a real application, you would load the theme data and export it
    Q_UNUSED(themeName)
    Q_UNUSED(filePath)
    return true;
}

void ThemeManager::setThemeDirectories(const QStringList &directories)
{
    m_themeDirectories = directories;
}