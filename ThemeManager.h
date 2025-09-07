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
#include <QFileSystemWatcher>
#include <QDateTime>

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
    Q_INVOKABLE bool previewTheme(const QString &themeName);
    Q_INVOKABLE void applyPreviewedTheme();
    Q_INVOKABLE bool exportTheme(const QString &themeName, const QString &filePath);
    Q_INVOKABLE void setThemeDirectories(const QStringList &directories);

signals:
    void themeChanged();
    void themeFileChanged(); // Signal emitted when theme file changes are detected
    void themePreviewStarted();
    void themePreviewEnded();
    void themeLoadError(const QString &error);

private:
    void loadDefaultTheme();
    bool loadThemeFromFile(const QString &filePath);
    QString calculateThemeFileHash(const QString &filePath);
    bool detectThemeChanges();
    bool validateTheme(const QJsonObject &themeObj) const;
    QColor getColorValue(const QJsonObject &themeObj, const QString &key, const QString &mKey) const;
    
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
    QString m_currentThemeFile;
    QStringList m_themeDirectories;
    
    // Theme preview
    struct Theme {
        QColor primaryColor;
        QColor secondaryColor;
        QColor backgroundColor;
        QColor surfaceColor;
        QColor textColor;
        QColor accentColor;
        QColor errorColor;
    };
    
    Theme getCurrentTheme() const;
    void restoreTheme(const Theme &theme);
    
    // File watching
    QFileSystemWatcher *m_watcher;
    void setupFileWatcher();
    
private slots:
    void onThemeFileChanged(const QString &path);
};

#endif // THEMEMANAGER_H