#include "KeybindModel.h"
#include <QDir>
#include <QStandardPaths>

// Constructor
KeybindModel::KeybindModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_currentPage(0)
    , m_itemsPerPage(7)
    , m_viewHeight(0)
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
    // Determine the config file path
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + 
                        "/fredon-modal-cheat/default.conf";

    QFile file(configPath);
    
    // If config file doesn't exist, load sample data
    if (!file.exists()) {
        qWarning() << "Config file not found at:" << configPath << "Loading sample data.";
        loadSampleData();
        return;
    }

    // Try to open the file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open config file:" << configPath << "Loading sample data.";
        loadSampleData();
        return;
    }

    QTextStream in(&file);
    KeybindItem currentItem;
    bool hasCurrentItem = false;

    // Parse the file line by line
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // Skip empty lines and comments
        if (line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        if (line.startsWith("name:")) {
            // Save previous item if exists
            if (hasCurrentItem) {
                m_keybinds.append(currentItem);
            }
            
            // Start new item
            currentItem = KeybindItem();
            currentItem.name = line.mid(5);
            hasCurrentItem = true;
        } 
        else if (line.startsWith("keybind:") && hasCurrentItem) {
            currentItem.keybind = line.mid(8);
        } 
        else if (line.startsWith("description:") && hasCurrentItem) {
            currentItem.description = line.mid(12);
        } 
        else if (line.startsWith("icon:") && hasCurrentItem) {
            currentItem.icon = line.mid(5);
        }
    }

    // Don't forget the last item
    if (hasCurrentItem) {
        m_keybinds.append(currentItem);
    }

    file.close();
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
    
    // Add sample keybinds
    m_keybinds.append({"Terminal", "Mod+Return", "Open terminal emulator", ""});
    m_keybinds.append({"Browser", "Mod+B", "Launch web browser", ""});
    m_keybinds.append({"File Manager", "Mod+E", "Open file manager", ""});
    m_keybinds.append({"Text Editor", "Mod+T", "Open text editor", ""});
    m_keybinds.append({"Code Editor", "Mod+C", "Open code editor", ""});
    m_keybinds.append({"Music Player", "Mod+M", "Launch music player", ""});
    m_keybinds.append({"Screenshot", "Print", "Take a screenshot", ""});
    m_keybinds.append({"Screen Recorder", "Ctrl+Shift+R", "Start screen recording", ""});
    m_keybinds.append({"Lock Screen", "Mod+Shift+L", "Lock the screen", ""});
    m_keybinds.append({"System Monitor", "Mod+Del", "Open system monitor", ""});
    m_keybinds.append({"Calculator", "Mod+U", "Open calculator", ""});
    m_keybinds.append({"Calendar", "Mod+K", "Open calendar", ""});
    m_keybinds.append({"Notes", "Mod+N", "Open notes app", ""});
    m_keybinds.append({"Clipboard", "Mod+V", "Open clipboard manager", ""});
    m_keybinds.append({"Color Picker", "Mod+P", "Launch color picker", ""});
    m_keybinds.append({"Dictionary", "Mod+D", "Open dictionary", ""});
    m_keybinds.append({"VPN Toggle", "Mod+Shift+V", "Toggle VPN connection", ""});
    m_keybinds.append({"Wifi Toggle", "Mod+Shift+W", "Toggle WiFi connection", ""});
    m_keybinds.append({"Bluetooth Toggle", "Mod+Shift+B", "Toggle Bluetooth", ""});
    m_keybinds.append({"Night Light", "Mod+Shift+N", "Toggle night light mode", ""});
    
    m_filteredKeybinds = m_keybinds;
}
