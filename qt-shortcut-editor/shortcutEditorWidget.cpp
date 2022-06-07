#include "shortcutEditorWidget.h"

#include "actionManager.h"
#include "keyboardWidget.h"

#include <QAbstractItemModel>
#include <QActionGroup>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeySequenceEdit>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QSplitter>
#include <QToolButton>
#include <QTreeView>
#include <QVBoxLayout>

#include <iostream>

static ShortcutEditorExpandState sShortcutEditorCurrentExpandState = {};

static SearchToolButtonState sSearchToolButtonState = {
  QString("Name"),
  QString("Contains"),
  true,
  true,
  true,
  {}
};


ShortcutEditorModelItem::ShortcutEditorModelItem(const std::vector<QVariant>& data, const QString& id, ShortcutEditorModelItem* parent)
  : m_itemData(data)
  , m_parentItem(parent)
  , m_id(id)
{
}

ShortcutEditorModelItem::~ShortcutEditorModelItem()
{
  qDeleteAll(m_childItems);
}

void ShortcutEditorModelItem::appendChild(ShortcutEditorModelItem *item)
{
  m_childItems.push_back(item);
}

ShortcutEditorModelItem *ShortcutEditorModelItem::child(int row)
{
  if (row < 0 || row >= m_childItems.size()) {
    return nullptr;
  }

  return m_childItems.at(row);
}

int ShortcutEditorModelItem::childCount() const
{
  return m_childItems.size();
}

int ShortcutEditorModelItem::row() const
{
  if (m_parentItem) {
    auto it = std::find(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(), const_cast<ShortcutEditorModelItem*>(this));
    if (it != m_parentItem->m_childItems.cend()) {
      return std::distance(m_parentItem->m_childItems.cbegin(), it);
    }
    else {
      return -1;
    }
  }

  return 0;
}

int ShortcutEditorModelItem::columnCount() const
{
    return m_itemData.size();
}

QVariant ShortcutEditorModelItem::data(int column) const
{
  if (column < 0 || column >= m_itemData.size()) {
    return QVariant();
  }

  QVariant columnVariant = m_itemData.at(column);
  if (column != static_cast<int>(Column::Shortcut) || columnVariant.canConvert<QString>()) {
    return columnVariant;
  }

  QAction* action = static_cast<QAction*>(columnVariant.value<void*>());
  if (!action) {
    return QVariant();
  }
  QKeySequence keySequence = action->shortcut();
  QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
  return keySequenceString;
}

bool ShortcutEditorModelItem::setData(int column, const QVariant& value)
{
  if (column < 0 || column >= m_itemData.size()) {
    return false;
  }

  if (column == static_cast<int>(Column::Shortcut)) {
    // std::cout << "TEST ITEM SET DATA: " << value.toString().toStdString() << std::endl;
    QAction* action = static_cast<QAction*>(m_itemData[column].value<void*>());
    if (action) {
      action->setShortcut(QKeySequence::fromString(value.toString(), QKeySequence::NativeText));
    }
  }
  else {
    m_itemData[column] = value;
  }

  return true;
}

ShortcutEditorModelItem *ShortcutEditorModelItem::parentItem()
{
  return m_parentItem;
}

const QString& ShortcutEditorModelItem::id() const
{
  return m_id;
}

QAction* ShortcutEditorModelItem::action() const
{
  QVariant actionVariant = m_itemData.at(static_cast<int>(Column::Shortcut));
  return static_cast<QAction*>(actionVariant.value<void*>());
}

ShortcutEditorDelegate::ShortcutEditorDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
  std::cout << "TEST SHORTCUT EDITOR DELEGATE" << std::endl;
}

QWidget* ShortcutEditorDelegate::createEditor(QWidget* parent,
                                            const QStyleOptionViewItem& /* option */,
                                            const QModelIndex& /* index */) const
{
  std::cout << "TEST SHORTCUT EDITOR DELEGATE CREATE EDITOR" << std::endl;
  QKeySequenceEdit* editor = new QKeySequenceEdit(parent);
  // editor->setFocusPolicy(Qt::StrongFocus);
  // connect(editor, &QKeySequenceEdit::editingFinished, this, &ShortcutEditorDelegate::commitAndCloseEditor);
  return editor;
}

void ShortcutEditorDelegate::commitAndCloseEditor()
{
  QKeySequenceEdit *editor = qobject_cast<QKeySequenceEdit *>(sender());
  Q_EMIT commitData(editor);
  Q_EMIT closeEditor(editor);
}

void ShortcutEditorDelegate::setEditorData(QWidget* editor,
                                         const QModelIndex& index) const
{
  std::cout << "TEST SHORTCUT EDITOR DELEGATE SET EDITOR DATA" << std::endl;
  QString value = index.model()->data(index, Qt::EditRole).toString();

  QKeySequenceEdit* keySequenceEdit = static_cast<QKeySequenceEdit*>(editor);
  keySequenceEdit->setKeySequence(value);
}

void ShortcutEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                        const QModelIndex &index) const
{
  std::cout << "TEST SHORTCUT EDITOR DELEGATE SET MODEL DATA: " << model->metaObject()->className() << std::endl;
  const QKeySequenceEdit *keySequenceEdit = qobject_cast<QKeySequenceEdit*>(editor);
  if (keySequenceEdit) {
    const QKeySequence keySequence = keySequenceEdit->keySequence();
    QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
    model->setData(index, keySequenceString);
    return;
  }
}

void ShortcutEditorDelegate::updateEditorGeometry(QWidget* editor,
                                                const QStyleOptionViewItem& option,
                                                const QModelIndex& /* index */) const
{
  std::cout << "TEST UPDATE EDITOR GEOMETRY" << std::endl;
  editor->setGeometry(option.rect);
}

ShortcutEditorModel::ShortcutEditorModel(QObject* parent)
  : QAbstractItemModel(parent)
{
  rootItem = new ShortcutEditorModelItem({tr("Name"), tr("Shortcut")}, QString("root"));
  _hoverTooltip =
    "Define the keyboard shortcuts for any action available";
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

ActionsMap ShortcutEditorModel::getActionsMap() const
{
  return _actionsMap;
}

QModelIndex ShortcutEditorModel::index(int row, int column, const QModelIndex &parent) const
{
  // std::cout << "TEST CREATE INDEX 1, ROW: " << row << ", COLUMN: " << column << std::endl;
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }
  // std::cout << "TEST CREATE INDEX 2, ROW: " << row << ", COLUMN: " << column << std::endl;

  ShortcutEditorModelItem *parentItem;

  if (!parent.isValid()) {
    parentItem = rootItem;
  }
  else {
    parentItem = static_cast<ShortcutEditorModelItem*>(parent.internalPointer());
  }

  ShortcutEditorModelItem *childItem = parentItem->child(row);
  if (childItem) {
    // std::cout << "TEST CREATE INDEX 3, ROW: " << row << ", COLUMN: " << column << std::endl;
    return createIndex(row, column, childItem);
  }

  return QModelIndex();
}

QModelIndex ShortcutEditorModel::parent(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return QModelIndex();
  }

  ShortcutEditorModelItem *childItem = static_cast<ShortcutEditorModelItem*>(index.internalPointer());
  ShortcutEditorModelItem *parentItem = childItem->parentItem();

  if (parentItem == rootItem) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int ShortcutEditorModel::rowCount(const QModelIndex &parent) const
{
  ShortcutEditorModelItem *parentItem;
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

int ShortcutEditorModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return static_cast<ShortcutEditorModelItem*>(parent.internalPointer())->columnCount();
  }

  return rootItem->columnCount();
}

QVariant ShortcutEditorModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  ShortcutEditorModelItem *item = static_cast<ShortcutEditorModelItem*>(index.internalPointer());

  if (role == Qt::ForegroundRole
      && index.column() == static_cast<int>(Column::Shortcut)) {
    QAction* action = item->action();
    if (!action) {
      return QVariant();
    }

    QKeySequence defaultShortcut = action->property(kDefaultShortcutPropertyName).value<QKeySequence>();
    if (action->shortcut() != defaultShortcut) {
      return QVariant(QApplication::palette().color(QPalette::Highlight));
    }
  }

  if (role != Qt::DisplayRole && role != Qt::EditRole) {
    return QVariant();
  }

  return item->data(index.column());
}

Qt::ItemFlags ShortcutEditorModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return Qt::NoItemFlags;
  }

  Qt::ItemFlags modelFlags = QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
  if (index.column() == static_cast<int>(Column::Shortcut)) {
    modelFlags |= Qt::ItemIsEditable;
  }
  else if (index.column() == static_cast<int>(Column::Name)) {
    modelFlags |= Qt::ItemIsDragEnabled;
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
    QString actionId = action->property(kIdPropertyName).toString();
    QStringList actionIdSections = actionId.split('.');
    QString context = actionIdSections[1];
    QString category = actionIdSections[2];

    CategoryActionsMap categoryActionsMap;
    if (_actionsMap.count(context)) {
      categoryActionsMap = _actionsMap[context];
    }

    std::vector<QAction*> actions;
    if (categoryActionsMap.count(category)) {
      actions = categoryActionsMap[category];
    }

    actions.push_back(action);

    categoryActionsMap.insert_or_assign(category, actions);
    _actionsMap.insert_or_assign(context, categoryActionsMap);
  }

  QAction* nullAction = nullptr;
  const QString contextIdPrefix = "root";
  // Go through each context, one context - many categories each iteration
  for (const auto& contextLevel : _actionsMap) {
    // TODO: make it "tr()".
    ShortcutEditorModelItem* contextLevelItem = new ShortcutEditorModelItem({contextLevel.first, QVariant::fromValue(nullAction)}, contextIdPrefix + contextLevel.first, parent);
    parent->appendChild(contextLevelItem);
    // Go through each category, one category - many actions each iteration
    for (const auto& categoryLevel : contextLevel.second) {
      ShortcutEditorModelItem* categoryLevelItem = new ShortcutEditorModelItem({categoryLevel.first, QVariant::fromValue(nullAction)}, contextLevel.first + categoryLevel.first, contextLevelItem);
      contextLevelItem->appendChild(categoryLevelItem);
      for (const auto& action : categoryLevel.second) {
        QString name = action->text();
        if (name.isEmpty() || action == nullptr) {
          continue;
        }
        ShortcutEditorModelItem* actionLevelItem = new ShortcutEditorModelItem({name, QVariant::fromValue(reinterpret_cast<void*>(action))}, categoryLevel.first + name, categoryLevelItem);
        categoryLevelItem->appendChild(actionLevelItem);
      }
    }
  }
}

bool ShortcutEditorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  std::cout << "TEST SHORTCUT EDITOR MODEL SET DATA: " << value.toString().toStdString() << ", ROLE: " << role << "(" << Qt::EditRole << "), COLUMN: " << index.column() << "(" << static_cast<int>(Column::Shortcut) << ")" << std::endl;
  if (role == Qt::EditRole && index.column() == static_cast<int>(Column::Shortcut)) {
    std::cout << "TEST SHORTCUT EDITOR MODEL SET DATA 2: " << value.toString().toStdString() << std::endl;
    QString keySequenceString= value.toString();
    ShortcutEditorModelItem *item = static_cast<ShortcutEditorModelItem*>(index.internalPointer());
    if (keySequenceString.isEmpty()) {
      item->setData(static_cast<int>(Column::Shortcut), keySequenceString);
      Q_EMIT dataChanged(index, index);
      return true;
    }

    ShortcutEditorModelItem* foundItem = findKeySequence(keySequenceString);
    const ShortcutEditorModelItem *currentItem = static_cast<ShortcutEditorModelItem*>(index.internalPointer());
    if (!foundItem || currentItem == foundItem) {
      item->setData(static_cast<int>(Column::Shortcut), keySequenceString);
      Q_EMIT dataChanged(index, index);
      return true;
    }

    QMessageBox messageBox;
    messageBox.setWindowTitle("Reassign shortcut?");
    messageBox.setIcon(QMessageBox::Warning);
    const QString foundNameString = foundItem->data(static_cast<int>(Column::Name)).toString();
    const QString foundShortcutString = foundItem->data(static_cast<int>(Column::Shortcut)).toString();
    const QString text = QLatin1String("Keyboard shortcut \"") + foundShortcutString + QLatin1String("\" is already assigned to \"") + foundNameString + QLatin1String("\".");
    messageBox.setText(text);
    messageBox.setInformativeText(tr("Are you sure you want to reassign this shortcut?"));
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);

    const int ret = messageBox.exec();
    switch (ret) {
      case QMessageBox::Yes:
        foundItem->setData(static_cast<int>(Column::Shortcut), QVariant());
        item->setData(static_cast<int>(Column::Shortcut), keySequenceString);
        Q_EMIT dataChanged(index, index);
        return true;
      case QMessageBox::No:
        break;
      default:
        break;
    }
  }

  return QAbstractItemModel::setData(index, value, role);
}

QMimeData* ShortcutEditorModel::mimeData(const QModelIndexList &indexes) const
{
  QMimeData *mimeData = new QMimeData;
  QStringList actionIds;
  for (const QModelIndex &index : indexes) {
    if (index.isValid()) {
      const ShortcutEditorModelItem *currentItem = static_cast<ShortcutEditorModelItem*>(index.internalPointer());
      QString actionId = currentItem->action()->property(kIdPropertyName).toString();
      actionIds.push_back(actionId);
    }
  }

  std::cout << "TEST MIME DATA: " << actionIds.join(' ').toUtf8().toStdString() << std::endl;
  mimeData->setData("text/plain", actionIds.join(' ').toUtf8());
  return mimeData;
}

QStringList ShortcutEditorModel::mimeTypes() const
{
  return {"text/plain"};
}

bool ShortcutEditorModel::canDropMimeData(const QMimeData *data,
                                        Qt::DropAction action,
                                        int row, int column,
                                        const QModelIndex &parent) const
{
  Q_UNUSED(action);
  Q_UNUSED(row);
  Q_UNUSED(parent);

  if (!data->hasFormat("text/plain")) {
    return false;
  }

  return true;
}

bool ShortcutEditorModel::dropMimeData(const QMimeData *data,
                                     Qt::DropAction action,
                                     int row, int column,
                                     const QModelIndex &parent)
{
  if (!canDropMimeData(data, action, row, column, parent)) {
    return false;
  }

  if (action == Qt::IgnoreAction) {
    return true;
  }

  if (!parent.isValid()) {
    return false;
  }

  if (!data->hasText()) {
    return false;
  }

  QModelIndex modelIndex = index(parent.row(), static_cast<int>(Column::Shortcut), parent.parent());
  setData(modelIndex, data->text());

  return true;
}

ShortcutEditorModelItem* ShortcutEditorModel::findKeySequence(const QString& keySequenceString)
{
  for (int i = 0; i < rootItem->childCount(); ++i) {
    ShortcutEditorModelItem* contextLevel = rootItem->child(i);
    for (int j = 0; j < contextLevel->childCount(); ++j) {
      ShortcutEditorModelItem* categoryLevel = contextLevel->child(j);
      for (int k = 0; k < categoryLevel->childCount(); ++k) {
        ShortcutEditorModelItem* actionLevel = categoryLevel->child(k);
        const QVariant actionLevelShortcut = actionLevel->data(static_cast<int>(Column::Shortcut));
        if (keySequenceString == actionLevelShortcut.toString()) {
          return actionLevel;
        }
      }
    }
  }

  return nullptr;
}

void ShortcutEditorModel::resetAll()
{
  for (int i = 0; i < rootItem->childCount(); ++i) {
    ShortcutEditorModelItem* contextLevel = rootItem->child(i);
    for (int j = 0; j < contextLevel->childCount(); ++j) {
      ShortcutEditorModelItem* categoryLevel = contextLevel->child(j);
      for (int k = 0; k < categoryLevel->childCount(); ++k) {
        ShortcutEditorModelItem* actionLevel = categoryLevel->child(k);
        QAction* action = actionLevel->action();
        QKeySequence shortcut = action->shortcut();
        QKeySequence defaultShortcut = action->property(kDefaultShortcutPropertyName).value<QKeySequence>();
        if (shortcut != defaultShortcut) {
          action->setShortcut(defaultShortcut);
          QModelIndex index = createIndex(k, 1, actionLevel);
          Q_EMIT dataChanged(index, index);
        }
      }
    }
  }
}

void ShortcutEditorModel::assignShortcut(const QString& actionId, const QKeySequence& keySequence)
{
  std::cout << "TEST ASSIGN SHORTCUT ACTION ID: " << actionId.toStdString() << std::endl;
  for (int i = 0; i < rootItem->childCount(); ++i) {
    ShortcutEditorModelItem* contextLevel = rootItem->child(i);
    for (int j = 0; j < contextLevel->childCount(); ++j) {
      ShortcutEditorModelItem* categoryLevel = contextLevel->child(j);
      for (int k = 0; k < categoryLevel->childCount(); ++k) {
        ShortcutEditorModelItem* actionLevel = categoryLevel->child(k);
        QAction* action = actionLevel->action();
        QString currentActionId = action->property(kIdPropertyName).toString();
        // std::cout << "TEST ASSIGN SHORTCUT CURRENT ACTION ID: " << currentActionId.toStdString() << std::endl;
        if (currentActionId == actionId) {
          std::cout << "TEST ASSIGN SHORTCUT ACTION ID 2: " << actionId.toStdString() << std::endl;
          action->setShortcut(keySequence);
          QModelIndex index = createIndex(k, 1, actionLevel);
          Q_EMIT dataChanged(index, index);
        }
      }
    }
  }
}

void ShortcutEditorModel::reset(const QModelIndexList& selectedItems)
{
  for (const QModelIndex &selectedItem : selectedItems) {
    ShortcutEditorModelItem *item = static_cast<ShortcutEditorModelItem*>(selectedItem.internalPointer());
    QAction* action = item->action();
    QKeySequence shortcut = action->shortcut();
    QKeySequence defaultShortcut = action->property(kDefaultShortcutPropertyName).value<QKeySequence>();
    if (shortcut != defaultShortcut) {
      action->setShortcut(defaultShortcut);
    }
    Q_EMIT dataChanged(selectedItem, selectedItem);
  }
}

void ShortcutEditorModel::setHoverTooltipText(const QString& hoverTooltipText)
{
  _hoverTooltip = hoverTooltipText;
}

const QString& ShortcutEditorModel::hoverTooltipText()
{
  return _hoverTooltip;
}

ShortcutEditorWidget::ShortcutEditorWidget(const char* objName, QWidget* parent) :
  QWidget(parent)
{
  if (objName) {
    setObjectName(objName);
  }

  QHBoxLayout* searchLayout = new QHBoxLayout();

  _searchToolButtonMenu = new QMenu();
  _searchToolButton = new QToolButton(this);
  _searchToolButton->setIcon(QIcon(":/ui/qrc/images/Search-icon.png"));
  _searchToolButton->setMenu(_searchToolButtonMenu);
  _searchToolButton->setPopupMode(QToolButton::InstantPopup);

  _search = new QLineEdit(this);
  _search->setPlaceholderText("Search Shortcuts");
  _search->setClearButtonEnabled(true);

  searchLayout->addWidget(_searchToolButton);
  searchLayout->addWidget(_search);
  searchLayout->setSpacing(0);

  // set up the model and view
  _view = new QTreeView(this);
  _model = new ShortcutEditorModel(_view);

  // TODO: Add meaningful toolbar, maybe with some delay to be less distractive?
  // setToolTip(_model->hoverTooltipText());

  _filterModel = new QSortFilterProxyModel(this);
  _filterModel->setSourceModel(_model);
  _filterModel->setFilterKeyColumn(-1);
  _filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  _filterModel->setRecursiveFilteringEnabled(true);
  _filterModel->setDynamicSortFilter(true);

  _view->setModel(_filterModel);

  _delegate = new ShortcutEditorDelegate(_view);
  _view->setItemDelegateForColumn(1, _delegate);

  // _view->setMinimumSize(250, 350);
  _view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  _view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  // _view->resizeColumnToContents(0);
  _view->setAlternatingRowColors(true);
  _view->setSelectionBehavior(QTreeView::SelectRows);
  _view->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _view->setDragDropMode(QAbstractItemView::DragDrop);
  _view->setDragEnabled(true);
  _view->setAcceptDrops(true);
  _view->setDropIndicatorShown(true);

  QAction *expandAllAction = new QAction(tr("expand all"), this);
  expandAllAction->setToolTip(tr("Expands all nodes"));
  connect(expandAllAction, &QAction::triggered, _view, &QTreeView::expandAll);
  _view->insertAction(nullptr, expandAllAction);

  QAction *expandRecursivelyAction = new QAction(tr("expand recursively"), this);
  expandRecursivelyAction->setToolTip(tr("Expands the selected nodes recursively"));
  connect(expandRecursivelyAction, &QAction::triggered, [this](){
    for (const auto& selectedIndex : _view->selectionModel()->selectedIndexes()) {
      // TODO: From Qt 5.13
      // _view->expandRecursively(selectedIndex);
      expandRecursively(selectedIndex, false);
    }
  });
  _view->insertAction(nullptr, expandRecursivelyAction);

  QAction *expandAction = new QAction(tr("expand selection"), this);
  expandAction->setToolTip(tr("Expands the selected nodes"));
  connect(expandAction, &QAction::triggered, [this](){
    for (const auto& selectedIndex : _view->selectionModel()->selectedIndexes()) {
      _view->expand(selectedIndex);
    }
  });
  _view->insertAction(nullptr, expandAction);

  QAction *collapseAllAction = new QAction(tr("collapse all"), this);
  collapseAllAction->setToolTip(tr("Collapses all nodes"));
  connect(collapseAllAction, &QAction::triggered, _view, &QTreeView::collapseAll);
  _view->insertAction(nullptr, collapseAllAction);

  QAction *collapseAction = new QAction(tr("collapse selection"), this);
  collapseAction->setToolTip(tr("Collapses the selected nodes"));
  connect(collapseAction, &QAction::triggered, [this](){
    for (const auto& selectedIndex : _view->selectionModel()->selectedIndexes()) {
      _view->collapse(selectedIndex);
    }
  });
  _view->insertAction(nullptr, collapseAction);

  _view->setContextMenuPolicy(Qt::ActionsContextMenu);
  _view->setAllColumnsShowFocus(true);
  _view->header()->resizeSection(0, 250);

  connect(_search, &QLineEdit::textChanged, [this](const QString& text){
    if (_matchContainsAction->isChecked()) {
      _filterModel->setFilterFixedString(text);
    }
    else if (_matchExactlyAction->isChecked()) {
      _filterModel->setFilterRegularExpression("^" + text + "$");
    }
    else if (_matchStartsWithAction->isChecked()) {
      _filterModel->setFilterRegularExpression("^" + text);
    }
    else if (_matchEndsWithAction->isChecked()) {
      _filterModel->setFilterRegularExpression(text + "$");
    }
    else if (_matchWildcardAction->isChecked()) {
      _filterModel->setFilterWildcard(text);
    }
    else if (_matchRegularExpressionAction->isChecked()) {
      _filterModel->setFilterRegularExpression(text);
    }

    if (text.isEmpty()) {
      _view->collapseAll();
    }
    else {
      _view->expandAll();
    }
  });

  // QItemSelectionModel* selectionModel = _view->selectionModel();
  // connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &ShortcutEditorWidget::selectionChanged);

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0); // fill out to the entire widget area, no insets
  setLayout(layout);

  layout->addLayout(searchLayout);
  layout->addWidget(_view);

  QHBoxLayout* keyboardExpandLayout = new QHBoxLayout();
  _keyboardExpandToolButton = new QToolButton(this);
  QIcon keyboardExpandIcon;
  keyboardExpandIcon.addPixmap(style()->standardPixmap(QStyle::SP_ArrowRight),
                           QIcon::Normal, QIcon::Off);
  keyboardExpandIcon.addPixmap(style()->standardPixmap(QStyle::SP_ArrowDown),
                           QIcon::Normal, QIcon::On);
  _keyboardExpandToolButton->setIcon(keyboardExpandIcon);
  _keyboardExpandToolButton->setCheckable(true);
  keyboardExpandLayout->addWidget(_keyboardExpandToolButton);
  keyboardExpandLayout->addWidget(new QLabel(tr("Keyboard"), this));

  layout->addLayout(keyboardExpandLayout);

  QHBoxLayout* contextLayout = new QHBoxLayout();
  _contextComboBox = new QComboBox();
  _contextComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  connect(_contextComboBox, &QComboBox::currentIndexChanged, this, &ShortcutEditorWidget::highlightShortcuts);
  contextLayout->addWidget(_contextComboBox);
  contextLayout->addStretch();
  layout->addLayout(contextLayout);

  _keyboardWidget = new KeyboardWidget(this);
  connect(_keyboardWidget, &KeyboardWidget::actionDropped, _model, &ShortcutEditorModel::assignShortcut);
  // TODO: make it dynamically expanding
  // _keyboardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  layout->addWidget(_keyboardWidget);
  connect(_view->model(), &QAbstractItemModel::dataChanged, _keyboardWidget, &KeyboardWidget::highlightShortcuts);

  connect(_keyboardExpandToolButton, &QAbstractButton::clicked, [this](){
    _keyboardWidget->setVisible(!_keyboardWidget->isVisible());
    _contextComboBox->setVisible(!_contextComboBox->isVisible());
  }); 

  QHBoxLayout* buttonLayout = new QHBoxLayout();
  layout->addLayout(buttonLayout);

  _resetAllButton = new QPushButton("Reset All", this);
  _resetAllButton->setFocusPolicy(Qt::TabFocus);
  connect(_resetAllButton, &QAbstractButton::clicked, _model, &ShortcutEditorModel::resetAll);
  buttonLayout->addWidget(_resetAllButton);

  _resetButton = new QPushButton("Reset", this);
  _resetButton->setFocusPolicy(Qt::TabFocus);
  connect(_resetButton, &QAbstractButton::clicked, this, &ShortcutEditorWidget::reset);
  buttonLayout->addWidget(_resetButton);

  buttonLayout->addStretch(0);

  if (!objectName().isEmpty()) {
    // AppSettings settings;
    // settings.beginGroup(objectName());
    /*QByteArray headerColumns = settings.value(SHORTCUT_EDITOR_HEADER_PREFERENCE_KEY + sPlatformStrings[sCurrentPlaform]).toByteArray();
    if (!headerColumns.isEmpty()) {
      treeHeader->restoreState(headerColumns);
    }*/
  }

  connect(_view, &QTreeView::collapsed, this, &ShortcutEditorWidget::updateExpandStates);
  connect(_view, &QTreeView::expanded, this, &ShortcutEditorWidget::updateExpandStates);

  // update the selection, so that the buttons are in the right state
  // selectionChanged();

  setActions();
}

ShortcutEditorWidget::~ShortcutEditorWidget()
{
  if (objectName().length() > 0){
    /* QHeaderView* treeHeader = _view->horizontalHeader();
    AppSettings settings;
    settings.beginGroup(objectName());
    settings.setValue(SHORTCUT_EDITOR_HEADER_PREFERENCE_KEY, treeHeader->saveState()); */
  }

  updateSearchToolButtonState();
}

void ShortcutEditorWidget::highlightShortcuts(int index)
{
  std::vector<QAction*> actions;
  ActionsMap actionsMap = _model->getActionsMap();
  for (const auto& category : actionsMap[_contextComboBox->itemText(index)]) {
    for (const auto& action : category.second) {
      actions.push_back(action);
    }
  }
  std::vector<QColor> colors{Qt::white, Qt::black, Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow};
  QColor color = Qt::white;
  if (_contextComboBox->currentIndex() < colors.size()) {
    color = colors[index];
  }

  _keyboardWidget->setButtonColor(color);
  _keyboardWidget->setActions(actions);
}

void ShortcutEditorWidget::setHoverTooltipText(const QString& hoverTooltipText)
{
  _model->setHoverTooltipText(hoverTooltipText);
  // TODO: Add meaningful tooltip, maybe with some delay to be less distractive?
  // setToolTip(_model->hoverTooltipText());
}

void ShortcutEditorWidget::setActions()
{
  _model->setActions();

  _contextComboBox->clear();
  for (const auto& context : _model->getActionsMap()) {
    _contextComboBox->addItem(context.first);
  }

  _searchToolButtonMenu->addSection("Search");

  QActionGroup* actionGroup = new QActionGroup(this);

  _nameAction = _searchToolButtonMenu->addAction(tr("Name"));
  _nameAction->setCheckable(true);
  _nameAction->setChecked(sSearchToolButtonState._actionGroupName == _nameAction->text());
  actionGroup->addAction(_nameAction);

  _shortcutAction = _searchToolButtonMenu->addAction(tr("Shortcut"));
  _shortcutAction->setCheckable(true);
  _shortcutAction->setChecked(sSearchToolButtonState._actionGroupName == _shortcutAction->text());
  actionGroup->addAction(_shortcutAction);

  _searchToolButtonMenu->addSection("Context");

  _allContextsAction = _searchToolButtonMenu->addAction(tr("All"));
  _allContextsAction->setCheckable(true);
  _allContextsAction->setChecked(sSearchToolButtonState._allContexts);
  connect(_allContextsAction, &QAction::triggered, [this](const bool triggered){
    if (!triggered) {
      return;
    }

    for (QAction* action : contextActions) {
      action->setChecked(triggered);
    }
  });

  _searchToolButtonMenu->addSeparator();

  for (const auto& context : _model->getActionsMap()) { 
    QAction* contextAction = _searchToolButtonMenu->addAction(context.first);
    contextAction->setCheckable(true);
    std::string stdContextName = context.first.toStdString();
    if (!sSearchToolButtonState._contextActionsState.count(stdContextName)) {
      sSearchToolButtonState._contextActionsState[stdContextName] = true;
    }
    contextAction->setChecked(sSearchToolButtonState._contextActionsState[stdContextName]);
    contextActions.push_back(contextAction);
    /* connect(contextAction, &QAction::triggered, [this, &hotkeys, &context](const bool triggered){
      std::vector<QKeySequence> hotkeyVector;
      ActionsMap actionsMap = _model->getActionsMap();
      for (const auto& category : actionsMap[context.first]) {
        for (const auto& action : category.second) {
          hotkeyVector.push_back(action->shortcut());
        }
      }
      std::vector<QColor> colors{Qt::white, Qt::black, Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow};
      _keyboardWidget->setButtonColor(colors[std::distance(hotkeys.begin(), hotkeys.find(context.first))]);
      _keyboardWidget->setHotkeys(hotkeyVector);
    }); */
  }

  _searchToolButtonMenu->addSection("Shortcut");

  _defaultShortcutAction = _searchToolButtonMenu->addAction(tr("Default Shortcut"));
  _defaultShortcutAction->setCheckable(true);
  _defaultShortcutAction->setChecked(sSearchToolButtonState._defaultShortcutChecked);

  _nonDefaultShortcutAction = _searchToolButtonMenu->addAction(tr("Non-default Shortcut"));
  _nonDefaultShortcutAction->setCheckable(true);
  _nonDefaultShortcutAction->setChecked(sSearchToolButtonState._nonDefaultShortcutChecked);

  _searchToolButtonMenu->addSection("Match");

  QActionGroup* matchActionGroup = new QActionGroup(this);

  _matchContainsAction = _searchToolButtonMenu->addAction(tr("Contains"));
  _matchContainsAction->setCheckable(true);
  _matchContainsAction->setChecked(sSearchToolButtonState._matchGroupName == _matchContainsAction->text());
  matchActionGroup->addAction(_matchContainsAction);

  _matchExactlyAction = _searchToolButtonMenu->addAction(tr("Exactly"));
  _matchExactlyAction->setCheckable(true);
  _matchExactlyAction->setChecked(sSearchToolButtonState._matchGroupName == _matchExactlyAction->text());
  matchActionGroup->addAction(_matchExactlyAction);

  _matchStartsWithAction = _searchToolButtonMenu->addAction(tr("Starts with"));
  _matchStartsWithAction->setCheckable(true);
  _matchStartsWithAction->setChecked(sSearchToolButtonState._matchGroupName == _matchStartsWithAction->text());
  matchActionGroup->addAction(_matchStartsWithAction);

  _matchEndsWithAction = _searchToolButtonMenu->addAction(tr("Ends with"));
  _matchEndsWithAction->setCheckable(true);
  _matchEndsWithAction->setChecked(sSearchToolButtonState._matchGroupName == _matchEndsWithAction->text());
  matchActionGroup->addAction(_matchEndsWithAction);

  _matchWildcardAction = _searchToolButtonMenu->addAction(tr("Wildcard"));
  _matchWildcardAction->setCheckable(true);
  _matchWildcardAction->setChecked(sSearchToolButtonState._matchGroupName == _matchWildcardAction->text());
  matchActionGroup->addAction(_matchWildcardAction);

  _matchRegularExpressionAction = _searchToolButtonMenu->addAction(tr("Regular Expression"));
  _matchRegularExpressionAction->setCheckable(true);
  _matchRegularExpressionAction->setChecked(sSearchToolButtonState._matchGroupName == _matchRegularExpressionAction->text());
  matchActionGroup->addAction(_matchRegularExpressionAction);

  restoreExpandState();

  // make sure the button states are properly updated
  // TODO: do we need this?
  // selectionChanged();
}

void ShortcutEditorWidget::reset()
{
  QModelIndexList selectedItems = _view->selectionModel()->selectedIndexes();

  QModelIndexList sourceSelectedItems;
  for (const QModelIndex& selectedItem : selectedItems) {
    sourceSelectedItems.push_back(_filterModel->mapToSource(selectedItem));
  }

  _model->reset(sourceSelectedItems);
}

void ShortcutEditorWidget::updateExpandStates(const QModelIndex& index)
{
  QModelIndex sourceIndex = _filterModel->mapToSource(index);
  ShortcutEditorModelItem* item = static_cast<ShortcutEditorModelItem*>(sourceIndex.internalPointer());
  sShortcutEditorCurrentExpandState[item->id().toStdString()] = _view->isExpanded(index);
}

void ShortcutEditorWidget::restoreExpandState()
{
  _view->blockSignals(true);
  for (int i = 0; i < _model->rowCount(); i++) {
    const QModelIndex child = _filterModel->index(i, 0);
    expandRecursively(child, true);
  }
  _view->blockSignals(false);
}

void ShortcutEditorWidget::expandRecursively(const QModelIndex& index, bool fromExpandState)
{
  if (!index.isValid()) {
    return;
  }

  for (int i = 0; i < index.model()->rowCount(index); i++) {
    const QModelIndex &child = index.model()->index(i, 0, index);
    expandRecursively(child, fromExpandState);
  }

  if (fromExpandState) {
    const auto& nonProxyIndex = _filterModel->mapToSource(index);
    ShortcutEditorModelItem* item = static_cast<ShortcutEditorModelItem*>(nonProxyIndex.internalPointer());
    if (sShortcutEditorCurrentExpandState[item->id().toStdString()]) {
      _view->expand(index);
    }
  }
  else if (!_view->isExpanded(index)) {
    _view->expand(index);
  }
}

void ShortcutEditorWidget::updateSearchToolButtonState()
{
  sSearchToolButtonState._allContexts = _allContextsAction->isChecked();
  sSearchToolButtonState._defaultShortcutChecked = _defaultShortcutAction->isChecked();
  sSearchToolButtonState._nonDefaultShortcutChecked = _nonDefaultShortcutAction->isChecked();

  for (const auto& contextAction : contextActions) {
    sSearchToolButtonState._contextActionsState[contextAction->text().toStdString()] = contextAction->isChecked();
  }

  if (_matchContainsAction->isChecked()) {
    sSearchToolButtonState._matchGroupName = _matchContainsAction->text();
  }
  else if (_matchExactlyAction->isChecked()) {
    sSearchToolButtonState._matchGroupName = _matchExactlyAction->text();
  }
  else if (_matchStartsWithAction->isChecked()) {
    sSearchToolButtonState._matchGroupName = _matchStartsWithAction->text();
  }
  else if (_matchEndsWithAction->isChecked()) {
    sSearchToolButtonState._matchGroupName = _matchEndsWithAction->text();
  }
  else if (_matchWildcardAction->isChecked()) {
    sSearchToolButtonState._matchGroupName = _matchWildcardAction->text();
  }
  else if (_matchRegularExpressionAction->isChecked()) {
    sSearchToolButtonState._matchGroupName = _matchRegularExpressionAction->text();
  }

  if (_nameAction->isChecked()) {
    sSearchToolButtonState._actionGroupName = _nameAction->text();
  }
  else if (_shortcutAction->isChecked()) {
    sSearchToolButtonState._actionGroupName = _shortcutAction->text();
  }
}