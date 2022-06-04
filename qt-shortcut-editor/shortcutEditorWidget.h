#ifndef HOTKEYEDITORWIDGET_H
#define HOTKEYEDITORWIDGET_H

#include <QString>
#include <QStyledItemDelegate>
#include <QWidget>

#include <string>
#include <unordered_map>

class QAction;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QMenu;
class QPushButton;
class QSortFilterProxyModel;
class QToolButton;
class QTreeView;

class KeyboardWidget;

// List of actions for all categories
using CategoryActionsMap = std::map<QString, std::vector<QAction*>>;

// List of categories for all contexts
using ActionsMap = std::map<QString, CategoryActionsMap>;

using ShortcutEditorExpandState = std::unordered_map<std::string, bool>;

enum class Column : uint8_t {
  Name,
  Shortcut
};

struct SearchToolButtonState
{
  QString _actionGroupName;
  QString _matchGroupName;
  bool _allContexts;
  bool _defaultHotkeyChecked;
  bool _nonDefaultHotkeyChecked;
  std::map<std::string, bool> _contextActionsState;
};

class ShortcutEditorModelItem
{
public:
    explicit ShortcutEditorModelItem(const std::vector<QVariant>& data,
                                   const QString& id,
                                   ShortcutEditorModelItem* parentItem = nullptr);
    ~ShortcutEditorModelItem();

    void appendChild(ShortcutEditorModelItem *child);

    ShortcutEditorModelItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool setData(int column, const QVariant& value);
    int row() const;
    ShortcutEditorModelItem *parentItem();
    const QString& id() const;
    QAction* action() const;

private:
    std::vector<ShortcutEditorModelItem*> m_childItems;
    std::vector<QVariant> m_itemData;
    ShortcutEditorModelItem *m_parentItem;
    QString m_id;
};

class ShortcutEditorModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit ShortcutEditorModel(QObject* parent = nullptr);
  ~ShortcutEditorModel() override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  int rowCount(const QModelIndex& index = QModelIndex()) const override;
  int columnCount(const QModelIndex& index = QModelIndex()) const override;

  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  QMimeData* mimeData(const QModelIndexList &indexes) const override;
  QStringList mimeTypes() const override;

  bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row,
                       int column, const QModelIndex &parent) const override;
  bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                    int row, int column, const QModelIndex &parent) override;

  ShortcutEditorModelItem* findKeySequence(const QString& keySequenceString);

  void setHoverTooltipText(const QString& hoverTooltipText);
  const QString& hoverTooltipText();

  void setHotkeys();
  ActionsMap getActionsMap() const;

public Q_SLOTS:
  void reset(const QModelIndexList& selectedItems);
  void resetAll();
  void assignShortcut(const QString& actionId, const QKeySequence& keySequence);

private:
  void setupModelData(ShortcutEditorModelItem* parent);

  ShortcutEditorModelItem* rootItem;
  ActionsMap _actionsMap;
  QString _hoverTooltip;
};

class ShortcutEditorDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  ShortcutEditorDelegate(QObject* parent = nullptr);
  QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override;

  void commitAndCloseEditor();

  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const override;

  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                            const QModelIndex& index) const override;
};

class ShortcutEditorWidget : public QWidget
{
  Q_OBJECT

public:

  // specify the object name if you want settings for this widget to be autosaved and loaded (like the column widths)
  ShortcutEditorWidget(const char* objectName = nullptr, QWidget* parent = nullptr);
  ~ShortcutEditorWidget() override;

  void setHoverTooltipText(const QString& hoverTooltipText);

  void setHotkeys();

public Q_SLOTS:
  void reset();

  void expandRecursively(const QModelIndex& index, bool fromExpandState = false);
  void updateExpandStates(const QModelIndex&);

Q_SIGNALS:
  void hotkeysChanged();

private Q_SLOTS:
  void highlightHotkeys(int index);

private:
  void restoreExpandState();

  void updateSearchToolButtonState();

  ShortcutEditorDelegate* _delegate;
  ShortcutEditorModel* _model;
  QSortFilterProxyModel* _filterModel;
  QToolButton* _searchToolButton;
  QMenu* _searchToolButtonMenu;
  QLineEdit* _search;
  QTreeView* _view;
  QToolButton* _keyboardExpandToolButton;
  QComboBox* _contextComboBox;
  KeyboardWidget* _keyboardWidget;
  QPushButton* _resetAllButton;
  QPushButton* _resetButton;
  QPushButton* _importButton;
  QPushButton* _exportButton;

  QAction* _nameAction;
  QAction* _hotkeyAction;
  QAction* _allContextsAction;
  std::vector<QAction*> contextActions;
  QAction* _defaultHotkeyAction;
  QAction* _nonDefaultHotkeyAction;
  QAction* _matchContainsAction;
  QAction* _matchExactlyAction;
  QAction* _matchStartsWithAction;
  QAction* _matchEndsWithAction;
  QAction* _matchWildcardAction;
  QAction* _matchRegularExpressionAction;

};

#endif


