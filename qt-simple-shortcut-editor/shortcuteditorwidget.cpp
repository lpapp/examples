#include "shortcuteditorwidget.h"

#include "actionmanager.h"

#include <QAbstractItemModel>
#include <QActionGroup>
#include <QApplication>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeySequenceEdit>
#include <QTreeView>
#include <QVBoxLayout>

ShortcutEditorModelItem::ShortcutEditorModelItem(const std::vector<QVariant> &data, ShortcutEditorModelItem *parent)
    : _itemData(data)
    , _parentItem(parent)
{
}

ShortcutEditorModelItem::~ShortcutEditorModelItem()
{
    qDeleteAll(_childItems);
}

void ShortcutEditorModelItem::appendChild(ShortcutEditorModelItem *item)
{
    _childItems.push_back(item);
}

ShortcutEditorModelItem* ShortcutEditorModelItem::child(int row)
{
    if (row < 0 || static_cast<size_t>(row) >= _childItems.size()) {
        return nullptr;
    }
  
    return _childItems.at(row);
}

int ShortcutEditorModelItem::childCount() const
{
    return _childItems.size();
}

int ShortcutEditorModelItem::row() const
{
    if (_parentItem) {
       auto it = std::find(_parentItem->_childItems.cbegin(), _parentItem->_childItems.cend(), const_cast<ShortcutEditorModelItem*>(this));
       if (it != _parentItem->_childItems.cend()) {
           return std::distance(_parentItem->_childItems.cbegin(), it);
       }
       else {
           return -1;
       }
    }
  
    return 0;
}

int ShortcutEditorModelItem::columnCount() const
{
    return _itemData.size();
}

QVariant ShortcutEditorModelItem::data(int column) const
{
    if (column < 0 || static_cast<size_t>(column) >= _itemData.size()) {
        return QVariant();
    }
  
    QVariant columnVariant = _itemData.at(column);
    if (column != static_cast<int>(Column::Shortcut) || columnVariant.canConvert<QString>()) {
        return columnVariant;
    }
  
    QAction *action = static_cast<QAction *>(columnVariant.value<void *>());
    if (!action) {
        return QVariant();
    }
    QKeySequence keySequence = action->shortcut();
    QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
    return keySequenceString;
}

ShortcutEditorModelItem *ShortcutEditorModelItem::parentItem()
{
    return _parentItem;
}

QAction *ShortcutEditorModelItem::action() const
{
    QVariant actionVariant = _itemData.at(static_cast<int>(Column::Shortcut));
    return static_cast<QAction*>(actionVariant.value<void *>());
}

ShortcutEditorDelegate::ShortcutEditorDelegate(QObject* parent)
  : QStyledItemDelegate(parent)
{
}

QWidget* ShortcutEditorDelegate::createEditor(QWidget* parent,
                                            const QStyleOptionViewItem& /* option */,
                                            const QModelIndex& /* index */) const
{
    QKeySequenceEdit* editor = new QKeySequenceEdit(parent);
    connect(editor, &QKeySequenceEdit::editingFinished, this, &ShortcutEditorDelegate::commitAndCloseEditor);
    return editor;
}

void ShortcutEditorDelegate::commitAndCloseEditor()
{
    QKeySequenceEdit* editor = qobject_cast<QKeySequenceEdit*>(sender());
    if (editor) {
        Q_EMIT commitData(editor);
        Q_EMIT closeEditor(editor);
    }
}

void ShortcutEditorDelegate::setEditorData(QWidget* editor,
                                         const QModelIndex& index) const
{
    if (!editor || !index.isValid()) {
        return;
    }

    QString value = index.model()->data(index, Qt::EditRole).toString();

    QKeySequenceEdit* keySequenceEdit = static_cast<QKeySequenceEdit*>(editor);
    keySequenceEdit->setKeySequence(value);
}

void ShortcutEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                        const QModelIndex& index) const
{
  if (!editor || !model || !index.isValid()) {
      return;
  }

  const QKeySequenceEdit *keySequenceEdit = qobject_cast<QKeySequenceEdit*>(editor);
  if (keySequenceEdit) {
      const QKeySequence keySequence = keySequenceEdit->keySequence();
      QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
      model->setData(index, keySequenceString);
  }
}

void ShortcutEditorDelegate::updateEditorGeometry(QWidget* editor,
                                                const QStyleOptionViewItem& option,
                                                const QModelIndex& /* index */) const
{
    editor->setGeometry(option.rect);
}

ShortcutEditorModel::ShortcutEditorModel(QObject* parent)
  : QAbstractItemModel(parent)
{
    rootItem = new ShortcutEditorModelItem({tr("Name"), tr("Shortcut")});
}

ShortcutEditorModel::~ShortcutEditorModel()
{
    delete rootItem;
}

void ShortcutEditorModel::setActions()
{
    beginResetModel();
    setupModelData(rootItem);
    endResetModel();
}

QModelIndex ShortcutEditorModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!hasIndex(row, column, parent)) {
      return QModelIndex();
  }

  ShortcutEditorModelItem* parentItem;

  if (!parent.isValid()) {
      parentItem = rootItem;
  }
  else {
      parentItem = static_cast<ShortcutEditorModelItem*>(parent.internalPointer());
  }

  ShortcutEditorModelItem* childItem = parentItem->child(row);
  if (childItem) {
      return createIndex(row, column, childItem);
  }

  return QModelIndex();
}

QModelIndex ShortcutEditorModel::parent(const QModelIndex& index) const
{
  if (!index.isValid()) {
      return QModelIndex();
  }

  ShortcutEditorModelItem* childItem = static_cast<ShortcutEditorModelItem*>(index.internalPointer());
  ShortcutEditorModelItem* parentItem = childItem->parentItem();

  if (parentItem == rootItem) {
      return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int ShortcutEditorModel::rowCount(const QModelIndex& parent) const
{
  ShortcutEditorModelItem* parentItem;
  if (parent.column() > 0) {
      return 0;
  }

  if (!parent.isValid()) {
      parentItem = rootItem;
  }
  else {
      parentItem = static_cast<ShortcutEditorModelItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}

int ShortcutEditorModel::columnCount(const QModelIndex& parent) const
{
  if (parent.isValid()) {
      return static_cast<ShortcutEditorModelItem*>(parent.internalPointer())->columnCount();
  }

  return rootItem->columnCount();
}

QVariant ShortcutEditorModel::data(const QModelIndex& index, int role) const
{
  if (!index.isValid()) {
      return QVariant();
  }

  if (role != Qt::DisplayRole && role != Qt::EditRole) {
      return QVariant();
  }

  ShortcutEditorModelItem* item = static_cast<ShortcutEditorModelItem*>(index.internalPointer());
  return item->data(index.column());
}

Qt::ItemFlags ShortcutEditorModel::flags(const QModelIndex& index) const
{
  if (!index.isValid()) {
      return Qt::NoItemFlags;
  }

  Qt::ItemFlags modelFlags = QAbstractItemModel::flags(index);
  if (index.column() == static_cast<int>(Column::Shortcut)) {
      modelFlags |= Qt::ItemIsEditable;
  }
  return modelFlags;
}

QVariant ShortcutEditorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
      return rootItem->data(section);
  }

  return QVariant();
}

void ShortcutEditorModel::setupModelData(ShortcutEditorModelItem* parent)
{
  _actionsMap.clear();
  std::vector<QAction*> registeredActions = ActionManager::registeredActions();
  for (QAction* action : registeredActions) {
    QString context = QString::fromStdString(ActionManager::getContext(action));
    QString category = QString::fromStdString(ActionManager::getCategory(action));

    CategoryActionsMap categoryActionsMap;
    const bool containsContext = _actionsMap.count(context);
    if (containsContext) {
      categoryActionsMap = _actionsMap[context];
    }

    std::vector<QAction*> actions;
    const bool containsCategory = categoryActionsMap.count(category);
    if (containsCategory) {
      actions = categoryActionsMap[category];
    }

    actions.push_back(action);

    if (containsCategory) {
      categoryActionsMap[category] = actions;
    }
    else {
      categoryActionsMap.insert({category, actions});
    }

    if (containsContext) {
      _actionsMap[context] = categoryActionsMap;
    }
    else {
      _actionsMap.insert({context, categoryActionsMap});
    }
  }

  QAction* nullAction = nullptr;
  const QString contextIdPrefix = "root";
  // Go through each context, one context - many categories each iteration
  for (const auto& contextLevel : _actionsMap) {
    // TODO: make it "tr()".
    ShortcutEditorModelItem* contextLevelItem = new ShortcutEditorModelItem({contextLevel.first, QVariant::fromValue(nullAction)}, parent);
    parent->appendChild(contextLevelItem);
    // Go through each category, one category - many actions each iteration
    for (const auto& categoryLevel : contextLevel.second) {
      ShortcutEditorModelItem* categoryLevelItem = new ShortcutEditorModelItem({categoryLevel.first, QVariant::fromValue(nullAction)}, contextLevelItem);
      contextLevelItem->appendChild(categoryLevelItem);
      for (const auto& action : categoryLevel.second) {
        QString name = action->text();
        if (name.isEmpty() || action == nullptr) {
          continue;
        }
        ShortcutEditorModelItem* actionLevelItem = new ShortcutEditorModelItem({name, QVariant::fromValue(reinterpret_cast<void*>(action))}, categoryLevelItem);
        categoryLevelItem->appendChild(actionLevelItem);
      }
    }
  }
}

bool ShortcutEditorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (role == Qt::EditRole && index.column() == static_cast<int>(Column::Shortcut)) {
      QString keySequenceString = value.toString();
      ShortcutEditorModelItem* item = static_cast<ShortcutEditorModelItem*>(index.internalPointer());
      QAction* itemAction = item->action();
      if (itemAction) {
          if (keySequenceString == itemAction->shortcut().toString(QKeySequence::NativeText)) {
            return true;
          }
      }

      itemAction->setShortcut(keySequenceString);
      Q_EMIT dataChanged(index, index);

      if (keySequenceString.isEmpty()) {
          return true;
      }
  }

  return QAbstractItemModel::setData(index, value, role);
}

ShortcutEditorWidget::ShortcutEditorWidget(QWidget* parent)
    : QWidget(parent)
{
    _model = new ShortcutEditorModel(this);
    _delegate = new ShortcutEditorDelegate(this);
    _view = new QTreeView(this);
    _view->setModel(_model);
    _view->setItemDelegateForColumn(1, _delegate);
    _view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    _view->setAllColumnsShowFocus(true);
    _view->header()->resizeSection(0, 250);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_view);
    setLayout(layout);

    _model->setActions();
}
