#ifndef SHORTCUTEDITORWIDGET_H
#define SHORTCUTEDITORWIDGET_H

#include <QSortFilterProxyModel>
#include <QString>
#include <QStyledItemDelegate>
#include <QUndoCommand>
#include <QWidget>

#include <string>
#include <unordered_map>
#include <unordered_set>

class QAction;
class QCheckBox;
class QHBoxLayout;
class QItemSelection;
class QLineEdit;
class QMenu;
class QPushButton;
class QToolButton;
class QTreeView;
class QUndoStack;

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

enum class SearchTarget : uint8_t {
  Name,
  Shortcut,
  DefaultShortcut,
  CustomShortcut
};

struct SearchToolButtonState
{
  QString _actionGroupName;
  QString _matchGroupName;
  bool _allContexts;
  bool _defaultShortcutChecked;
  bool _customShortcutChecked;
  std::map<std::string, bool> _contextActionsState;
};

struct ShortcutCommandData
{
  QAction* _action;
  QKeySequence _oldShortcut;
  QKeySequence _newShortcut;
};

class AssignShortcutCommand : public QUndoCommand
{
public:
  AssignShortcutCommand(QAction* action, QKeySequence newShortcut, QUndoCommand* parent = nullptr);
  AssignShortcutCommand(QAction* action, QUndoCommand* parent = nullptr);
  ~AssignShortcutCommand() = default;

  void undo() override;
  void redo() override;

private:
  std::vector<ShortcutCommandData> _data;
};

class ShortcutEditorModelItem
{
public:
    explicit ShortcutEditorModelItem(const std::vector<QVariant>& data,
                                   const QString& id,
                                   ShortcutEditorModelItem* parentItem = nullptr);
    ~ShortcutEditorModelItem();

    void appendChild(ShortcutEditorModelItem* child);

    ShortcutEditorModelItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    ShortcutEditorModelItem* parentItem();
    const QString& id() const;
    QAction* action() const;

private:
    std::vector<ShortcutEditorModelItem*> _childItems;
    std::vector<QVariant> _itemData;
    ShortcutEditorModelItem* _parentItem;
    QString _id;
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

  QMimeData* mimeData(const QModelIndexList& indexes) const override;
  QStringList mimeTypes() const override;

  bool canDropMimeData(const QMimeData* data, Qt::DropAction action, int row,
                       int column, const QModelIndex& parent) const override;
  bool dropMimeData(const QMimeData* data, Qt::DropAction action,
                    int row, int column, const QModelIndex& parent) override;

  ShortcutEditorModelItem* findShortcut(const QString& shortcut, const std::string& context);

  void setHoverTooltipText(const QString& hoverTooltipText);
  const QString& hoverTooltipText();

  void setActions();
  ActionsMap getActionsMap() const;

  QUndoStack* undoStack() const;

public Q_SLOTS:
  void reset(const QModelIndexList& selectedItems);
  void resetAll();
  void assignShortcut(const QString& actionId, const QKeySequence& keySequence);

private:
  void setShortcut(ShortcutEditorModelItem* item, const QString& shortcutString, const QModelIndex& index);
  void setupModelData(ShortcutEditorModelItem* parent);

  ShortcutEditorModelItem* rootItem;
  ActionsMap _actionsMap;
  QString _hoverTooltip;
  QUndoStack* _undoStack;
};

class ShortcutEditorSortFilterProxyModel : public QSortFilterProxyModel
{
  Q_OBJECT

public:
  ShortcutEditorSortFilterProxyModel(QObject *parent = 0);

public Q_SLOTS:
  void updateContext(const std::string& context, bool checked);
  void updateTarget(SearchTarget target);

private:
  bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

  std::unordered_set<std::string> _contexts;
  SearchTarget _target;
};

class ShortcutEditorDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  ShortcutEditorDelegate(QObject* parent = nullptr);

protected:
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override;

  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const override;

  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                            const QModelIndex& index) const override;

  bool eventFilter(QObject *editor, QEvent *event) override;

private Q_SLOTS:
  void commitAndCloseEditor();
};

class ShortcutEditorWidget : public QWidget
{
  Q_OBJECT

public:
  ShortcutEditorWidget(QWidget* parent = nullptr);
  ~ShortcutEditorWidget() override;

  void setHoverTooltipText(const QString& hoverTooltipText);

  void setActions();

public Q_SLOTS:
  void reset();
  void resetAll();

  void expandRecursively(const QModelIndex& index, bool fromExpandState = false);
  void updateExpandStates(const QModelIndex&);

private Q_SLOTS:
  void setKeyboardContext(const QItemSelection& selected, const QItemSelection& deselected);

private:
  void restoreExpandState();

  void updateSearchToolButtonState();

  void createLayout();
  void createSearchLayout();
  void createFilterModel();
  void createTreeView();
  void createTreeViewContextMenuActions();
  void setupTreeViewFiltering();
  void createKeyboardExpandLayout();
  QHBoxLayout* createButtonLayout();

  ShortcutEditorDelegate* _delegate;
  ShortcutEditorModel* _model;
  ShortcutEditorSortFilterProxyModel* _filterModel;
  QHBoxLayout* _searchLayout;
  QToolButton* _searchToolButton;
  QMenu* _searchToolButtonMenu;
  QLineEdit* _search;
  QTreeView* _view;
  QHBoxLayout* _keyboardExpandLayout;
  QToolButton* _keyboardExpandToolButton;
  KeyboardWidget* _keyboardWidget;
  QPushButton* _resetAllButton;
  QPushButton* _resetButton;

  QAction* _nameAction;
  QAction* _shortcutAction;
  QAction* _allContextsAction;
  std::vector<QAction*> _contextActions;
  QAction* _defaultShortcutAction;
  QAction* _customShortcutAction;
  QAction* _matchContainsAction;
  QAction* _matchExactlyAction;
  QAction* _matchStartsWithAction;
  QAction* _matchEndsWithAction;
  QAction* _matchWildcardAction;
  QAction* _matchRegularExpressionAction;

};

#endif
