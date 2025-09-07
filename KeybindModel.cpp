#include "KeybindModel.h"
#include <QDir>
#include <QStandardPaths>

// Constructor
KeybindModel::KeybindModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_currentPage(0)
    , m_itemsPerPage(7)
{
}

// Return the number of rows in the current view
int KeybindModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    // Calculate the range of items for the current page
    int start = m_currentPage * m_itemsPerPage;
    int end = qMin(start + m_itemsPerPage, m_filteredKeybinds.size());
    return end - start;
}

// Return data for a specific index and role
QVariant KeybindModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount())
        return QVariant();

    // Calculate the actual index in the filtered list
    int actualIndex = index.row() + (m_currentPage * m_itemsPerPage);
    if (actualIndex >= m_filteredKeybinds.size())
        return QVariant();

    const KeybindItem &item = m_filteredKeybinds.at(actualIndex);

    // Return data based on the requested role
    switch (role) {
    case NameRole:
        return item.name;
    case KeybindRole:
        return item.keybind;
    case DescriptionRole:
        return item.description;
    case IconRole:
        return item.icon;
    default:
        return QVariant();
    }
}

// Define role names for QML access
QHash<int, QByteArray> KeybindModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "name";
    roles[KeybindRole] = "keybind";
    roles[DescriptionRole] = "description";
    roles[IconRole] = "icon";
    return roles;
}

// Pagination getters and setters
int KeybindModel::currentPage() const
{
    return m_currentPage;
}

void KeybindModel::setCurrentPage(int page)
{
    if (m_currentPage != page) {
        m_currentPage = page;
        emit currentPageChanged();
        emit totalPagesChanged();
        refresh();
    }
}

int KeybindModel::totalPages() const
{
    return (m_filteredKeybinds.size() + m_itemsPerPage - 1) / m_itemsPerPage;
}

int KeybindModel::itemsPerPage() const
{
    return m_itemsPerPage;
}

void KeybindModel::setItemsPerPage(int count)
{
    if (m_itemsPerPage != count) {
        m_itemsPerPage = count;
        emit itemsPerPageChanged();
        emit totalPagesChanged();
        refresh();
    }
}

// Filter getters and setters
QString KeybindModel::filterText() const
{
    return m_filterText;
}

void KeybindModel::setFilterText(const QString &text)
{
    if (m_filterText != text) {
        m_filterText = text;
        emit filterTextChanged();
        applyFilter();
        setCurrentPage(0);
        refresh();
    }
}

// View getters and setters
int KeybindModel::viewHeight() const
{
    return m_viewHeight;
}

void KeybindModel::setViewHeight(int height)
{
    if (m_viewHeight != height) {
        m_viewHeight = height;
        emit viewHeightChanged();
        // Recalculate itemsPerPage
        if (m_viewHeight > 0) {
            setItemsPerPage(m_viewHeight / 50); // Assuming item height of 50
        }
    }
}

// Refresh the model (reset and emit signals)
void KeybindModel::refresh()
{
    beginResetModel();
    endResetModel();
    emit totalPagesChanged();
}

// Load keybinds from configuration file
void KeybindModel::loadKeybinds()
{
    beginResetModel();
    m_keybinds.clear();
    parseConfigFile();
    applyFilter();
    m_currentPage = 0;
    endResetModel();
    
    emit totalPagesChanged();
    emit currentPageChanged();
}

// Parse the configuration file
void KeybindModel::parseConfigFile()
{
    // Clear existing data
    m_keybinds.clear();
    
    // Map to store variables
    QMap<QString, QString> variables;
    
    // Try to read the Hyprland config file first (updated paths based on user note)
    QString hyprConfigPath = QDir::homePath() + "/.local/share/fredon/default/hypr/bindings.conf";
    QString fallbackConfigPath = QDir::homePath() + "/.config/hypr/hyprland.conf";
    QString localConfigPath = QDir(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).filePath("bindings.conf");
    
    QString configPath = hyprConfigPath;
    if (!QFile::exists(configPath)) {
        configPath = localConfigPath;
        if (!QFile::exists(configPath)) {
            configPath = fallbackConfigPath;
        }
    }
    
    QFile file(configPath);
    
    // If none of the config files exist, try the default one
    if (!file.exists()) {
        configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + 
                    "/fredon-modal-cheat/default.conf";
        file.setFileName(configPath);
        
        // If default config doesn't exist either, load sample data
        if (!file.exists()) {
            qWarning() << "Config file not found at any expected location. Loading sample data.";
            loadSampleData();
            return;
        }
    }

    qDebug() << "Loading config file:" << configPath;
    if (!QFile::exists(configPath)) {
        qWarning() << "Config file does not exist:" << configPath;
    }

    // Try to open the file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open config file:" << configPath << "Loading sample data.";
        loadSampleData();
        return;
    }

    QTextStream in(&file);
    QString lastIcon = "";
    
    // Parse the file line by line
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // Skip empty lines
        if (line.isEmpty()) {
            continue;
        }
        
        // Handle variable definitions
        if (line.startsWith("$")) {
            int equalPos = line.indexOf('=');
            if (equalPos != -1) {
                QString varName = line.left(equalPos).trimmed();
                QString varValue = line.mid(equalPos + 1).trimmed();
                variables[varName] = varValue;
                qDebug() << "Found variable:" << varName << "=" << varValue;
            }
            continue;
        }
        
        // Handle icon comments
        if (line.startsWith("## ICON:")) {
            lastIcon = line.mid(8).trimmed(); // Remove "## ICON:" prefix
            qDebug() << "Found icon:" << lastIcon;
            continue;
        }
        
        // Handle bind commands
        if (line.startsWith("bind") || line.startsWith("bindm") || line.startsWith("bindi") || line.startsWith("bindel") || line.startsWith("bindl")) {
            qDebug() << "Processing binding:" << line;
            
            // Parse the binding
            KeybindItem item;
            
            // Extract command type (bind, bindm, etc.)
            int equalPos = line.indexOf('=');
            if (equalPos == -1) continue;
            
            QString commandType = line.left(equalPos).trimmed();
            QString bindingData = line.mid(equalPos + 1).trimmed();
            
            // Split the binding data by commas
            QStringList parts = bindingData.split(',');
            if (parts.size() < 3) continue;
            
            // Extract modifiers and key
            QString modifiers = parts[0].trimmed();
            QString key = parts[1].trimmed();
            
            // Resolve variables in modifiers
            for (auto it = variables.constBegin(); it != variables.constEnd(); ++it) {
                modifiers.replace(it.key(), it.value());
            }
            
            // Format the keybind display
            item.keybind = modifiers;
            if (!item.keybind.isEmpty() && !key.isEmpty()) {
                item.keybind += " + ";
            }
            item.keybind += key;
            
            // Extract dispatcher and parameters
            QString dispatcher = parts[2].trimmed();
            QString params = "";
            if (parts.size() > 3) {
                params = parts.mid(3).join(',').trimmed();
            }
            
            // Set name and description based on dispatcher and params
            if (dispatcher == "exec" && !params.isEmpty()) {
                // For exec commands, try to extract a meaningful name
                QString command = params;
                
                // Remove common prefixes
                if (command.startsWith("\"")) command = command.mid(1);
                if (command.endsWith("\"")) command = command.left(command.length() - 1);
                
                // Extract command name from path
                QStringList commandParts = command.split(' ');
                QString executable = commandParts.first();
                QFileInfo fileInfo(executable);
                QString commandName = fileInfo.baseName();
                
                // Special handling for some common commands
                if (commandName == "kitty") {
                    item.name = "Terminal";
                } else if (commandName == "hyprshot") {
                    item.name = "Screenshot";
                } else if (commandName == "hyprpicker") {
                    item.name = "Color Picker";
                } else if (commandName == "code") {
                    item.name = "Code Editor";
                } else if (commandName == "walker") {
                    item.name = "Application Launcher";
                } else if (commandName == "waypaper") {
                    item.name = "Wallpaper Changer";
                } else if (commandName == "wlogout") {
                    item.name = "Power Menu";
                } else if (commandName == "swaync-client") {
                    item.name = "Notifications";
                } else if (commandName == "btop") {
                    item.name = "System Monitor";
                } else if (commandName == "lazydocker") {
                    item.name = "Docker Manager";
                } else if (commandName == "clipse") {
                    item.name = "Clipboard Manager";
                } else {
                    // Capitalize first letter
                    commandName[0] = commandName[0].toUpper();
                    item.name = commandName;
                }
                
                item.description = "Execute: " + command;
            } else {
                // For other dispatchers, use the dispatcher name as the name
                item.name = dispatcher;
                // Capitalize first letter
                if (!item.name.isEmpty()) {
                    item.name[0] = item.name[0].toUpper();
                }
                
                // Create description from dispatcher and params
                item.description = dispatcher;
                if (!params.isEmpty()) {
                    item.description += " " + params;
                }
            }
            
            // Set icon if available
            item.icon = lastIcon;
            
            // Add to list
            m_keybinds.append(item);
            qDebug() << "Added keybind:" << item.name << "-" << item.keybind;
            
            // Reset lastIcon for next binding
            lastIcon = "";
        }
    }

    file.close();
    
    qDebug() << "Loaded" << m_keybinds.size() << "keybinds";
}

// Apply filter to the keybind list
void KeybindModel::applyFilter()
{
    m_filteredKeybinds.clear();
    
    // If no filter text, show all items
    if (m_filterText.isEmpty()) {
        m_filteredKeybinds = m_keybinds;
    } 
    else {
        // Filter items based on name, keybind, or description
        for (const KeybindItem &item : m_keybinds) {
            if (item.name.contains(m_filterText, Qt::CaseInsensitive) ||
                item.keybind.contains(m_filterText, Qt::CaseInsensitive) ||
                item.description.contains(m_filterText, Qt::CaseInsensitive)) {
                m_filteredKeybinds.append(item);
            }
        }
    }
    
    // Reset to first page when filter changes
    m_currentPage = 0;
}

// Load sample data when config file is not available
void KeybindModel::loadSampleData()
{
    m_keybinds.clear();
    
    // Add sample keybinds in Hyprland format
    m_keybinds.append({"Terminal", "SUPER + Return", "Execute: kitty", ""});
    m_keybinds.append({"File Manager", "SUPER + E", "Execute: thunar", ""});
    m_keybinds.append({"Browser", "SUPER + W", "Execute: firefox", ""});
    m_keybinds.append({"Code Editor", "SUPER + C", "Execute: code", ""});
    m_keybinds.append({"Application Launcher", "SUPER + A", "Execute: walker", ""});
    m_keybinds.append({"Screenshot", "SUPER + S", "Execute: hyprshot -m output", ""});
    m_keybinds.append({"Color Picker", "SUPER + P", "Execute: hyprpicker -a", ""});
    m_keybinds.append({"System Monitor", "SUPER + T", "Execute: kitty -e btop", ""});
    m_keybinds.append({"Kill Active Window", "SUPER + Q", "Kill active window", ""});
    m_keybinds.append({"Toggle Floating", "SUPER + H", "Toggle floating mode", ""});
    m_keybinds.append({"Workspace 1", "SUPER + 1", "Switch to workspace 1", ""});
    m_keybinds.append({"Workspace 2", "SUPER + 2", "Switch to workspace 2", ""});
    m_keybinds.append({"Move to Workspace 1", "SUPER + SHIFT + 1", "Move window to workspace 1", ""});
    m_keybinds.append({"Resize Width +", "SUPER + =", "Resize active window", ""});
    m_keybinds.append({"Next Workspace", "SUPER + Mouse Wheel Up", "Switch to next workspace", ""});
    m_keybinds.append({"Previous Workspace", "SUPER + Mouse Wheel Down", "Switch to previous workspace", ""});
    m_keybinds.append({"Move Window", "SUPER + LMB", "Move window with mouse", ""});
    m_keybinds.append({"Resize Window", "SUPER + RMB", "Resize window with mouse", ""});
    m_keybinds.append({"Volume Up", "XF86AudioRaiseVolume", "Increase volume", ""});
    m_keybinds.append({"Volume Down", "XF86AudioLowerVolume", "Decrease volume", ""});
    
    m_filteredKeybinds = m_keybinds;
}
