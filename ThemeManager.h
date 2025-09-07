#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <QColor>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QStandardPaths>
#include <QDebug>
#include <QCryptographicHash>
#include <QSettings>

class ThemeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor primaryColor READ primaryColor NOTIFY themeChanged)
    Q_PROPERTY(QColor secondaryColor READ secondaryColor NOTIFY themeChanged)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor NOTIFY themeChanged)
    Q_PROPERTY(QColor surfaceColor READ surfaceColor NOTIFY themeChanged)
    Q_PROPERTY(QColor textColor READ textColor NOTIFY themeChanged)
    Q_PROPERTY(QColor accentColor READ accentColor NOTIFY themeChanged)
    Q_PROPERTY(QColor errorColor READ errorColor NOTIFY themeChanged)

public:
    explicit ThemeManager(QObject *parent = nullptr);

    // Color properties
    QColor primaryColor() const;
    QColor secondaryColor() const;
    QColor backgroundColor() const;
    QColor surfaceColor() const;
    QColor textColor() const;
    QColor accentColor() const;
    QColor errorColor() const;

    // Methods
    Q_INVOKABLE bool loadTheme(const QString &themeName);
    Q_INVOKABLE QStringList availableThemes() const;
    Q_INVOKABLE bool checkForThemeChanges();

signals:
    void themeChanged();
    void themeFileChanged(); // Signal emitted when theme file changes are detected

private:
    void loadDefaultTheme();
    bool loadThemeFromFile(const QString &filePath);
    QString calculateThemeFileHash(const QString &filePath);
    bool detectThemeChanges();
    
    // Theme colors
    QColor m_primaryColor;
    QColor m_secondaryColor;
    QColor m_backgroundColor;
    QColor m_surfaceColor;
    QColor m_textColor;
    QColor m_accentColor;
    QColor m_errorColor;
    
    // Theme file tracking
    QString m_currentThemeHash;
};

#endif // THEMEMANAGER_H