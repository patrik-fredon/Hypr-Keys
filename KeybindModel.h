#ifndef KEYBINDMODEL_H
#define KEYBINDMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <QString>
#include <QStandardPaths>
#include <QFile>
#include <QTextStream>
#include <QDebug>

// Structure to hold keybind data
struct KeybindItem {
    QString name;
    QString keybind;
    QString description;
    QString icon;
};

// Model class for keybind data
class KeybindModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(int totalPages READ totalPages NOTIFY totalPagesChanged)
    Q_PROPERTY(int itemsPerPage READ itemsPerPage WRITE setItemsPerPage NOTIFY itemsPerPageChanged)
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)
    Q_PROPERTY(int viewHeight READ viewHeight WRITE setViewHeight NOTIFY viewHeightChanged)

public:
    // Define roles for the model
    enum Roles {
        NameRole = Qt::UserRole + 1,
        KeybindRole,
        DescriptionRole,
        IconRole
    };

    // Constructor
    explicit KeybindModel(QObject *parent = nullptr);

    // Required methods for QAbstractListModel
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    // Pagination properties
    int currentPage() const;
    void setCurrentPage(int page);

    int totalPages() const;

    int itemsPerPage() const;
    void setItemsPerPage(int count);

    // Filter properties
    QString filterText() const;
    void setFilterText(const QString &text);

    int viewHeight() const;
    void setViewHeight(int height);

    // Public methods
    Q_INVOKABLE void loadKeybinds();
    Q_INVOKABLE void refresh();

signals:
    void currentPageChanged();
    void totalPagesChanged();
    void itemsPerPageChanged();
    void filterTextChanged();
    void viewHeightChanged();

private:
    // Internal data storage
    QList<KeybindItem> m_keybinds;          // All keybinds
    QList<KeybindItem> m_filteredKeybinds;  // Filtered keybinds
    int m_currentPage;                      // Current page index
    int m_itemsPerPage;                     // Items per page
    QString m_filterText;                   // Current filter text
    int m_viewHeight;                   // Height of the view in pixels

    // Private methods
    void parseConfigFile();
    void applyFilter();
    void loadSampleData();
};

#endif // KEYBINDMODEL_H
