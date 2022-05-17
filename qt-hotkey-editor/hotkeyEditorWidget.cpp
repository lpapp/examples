#include "hotkeyEditorWidget.h"

#include "keyboardWidget.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QKeySequenceEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QSignalMapper>
#include <QtCore/QSortFilterProxyModel>

#include <iostream>
#include <set>

static const char* HOTKEY_EDITOR_HEADER_PREFERENCE_KEY = "headerColumns";

HotkeyEditorModelItem::HotkeyEditorModelItem(const std::vector<QVariant>& data, HotkeyEditorModelItem* parent)
  : m_itemData(data)
  , m_parentItem(parent)
{
}

HotkeyEditorModelItem::~HotkeyEditorModelItem()
{
  qDeleteAll(m_childItems);
}

void HotkeyEditorModelItem::appendChild(HotkeyEditorModelItem *item)
{
  m_childItems.push_back(item);
}

HotkeyEditorModelItem *HotkeyEditorModelItem::child(int row)
{
  if (row < 0 || row >= m_childItems.size()) {
    return nullptr;
  }

  return m_childItems.at(row);
}

int HotkeyEditorModelItem::childCount() const
{
  return m_childItems.size();
}

int HotkeyEditorModelItem::row() const
{
  if (m_parentItem) {
    auto it = std::find(m_parentItem->m_childItems.cbegin(), m_parentItem->m_childItems.cend(), const_cast<HotkeyEditorModelItem*>(this));
    if (it != m_parentItem->m_childItems.cend()) {
      std::distance(m_parentItem->m_childItems.cbegin(), it);
    }
    else {
      return -1;
    }
  }

  return 0;
}

int HotkeyEditorModelItem::columnCount() const
{
    return m_itemData.size();
}

QVariant HotkeyEditorModelItem::data(int column) const
{
  if (column < 0 || column >= m_itemData.size()) {
    return QVariant();
  }

  QVariant columnVariant = m_itemData.at(column);
  if (column != static_cast<int>(Column::Hotkey)) {
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

bool HotkeyEditorModelItem::setData(int column, const QVariant& value)
{
  if (column < 0 || column >= m_itemData.size()) {
    return false;
  }

  if (column == static_cast<int>(Column::Hotkey)) {
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

HotkeyEditorModelItem *HotkeyEditorModelItem::parentItem()
{
  return m_parentItem;
}

HotkeyEditorDelegate::HotkeyEditorDelegate(QObject *parent)
  : QStyledItemDelegate(parent)
{
}

QWidget* HotkeyEditorDelegate::createEditor(QWidget* parent,
                                            const QStyleOptionViewItem& /* option */,
                                            const QModelIndex& /* index */) const
{
  QKeySequenceEdit* editor = new QKeySequenceEdit(parent);
  // editor->setFocusPolicy(Qt::StrongFocus);
  // connect(editor, &QKeySequenceEdit::editingFinished, this, &HotkeyEditorDelegate::commitAndCloseEditor);
  return editor;
}

void HotkeyEditorDelegate::commitAndCloseEditor()
{
  QKeySequenceEdit *editor = qobject_cast<QKeySequenceEdit *>(sender());
  Q_EMIT commitData(editor);
  Q_EMIT closeEditor(editor);
}

void HotkeyEditorDelegate::setEditorData(QWidget* editor,
                                         const QModelIndex& index) const
{
  QString value = index.model()->data(index, Qt::EditRole).toString();

  QKeySequenceEdit* keySequenceEdit = static_cast<QKeySequenceEdit*>(editor);
  keySequenceEdit->setKeySequence(value);
}

void HotkeyEditorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                        const QModelIndex &index) const
{
  const QKeySequenceEdit *keySequenceEdit = qobject_cast<QKeySequenceEdit*>(editor);
  if (keySequenceEdit) {
    const QKeySequence keySequence = keySequenceEdit->keySequence();
    QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
    model->setData(index, keySequenceString);
    return;
  }
}

void HotkeyEditorDelegate::updateEditorGeometry(QWidget* editor,
                                                const QStyleOptionViewItem& option,
                                                const QModelIndex& /* index */) const
{
  editor->setGeometry(option.rect);
}

HotkeyEditorModel::HotkeyEditorModel(QObject* parent)
  : QAbstractItemModel(parent)
{
  rootItem = new HotkeyEditorModelItem({tr("Name"), tr("Hotkey")});
  _hoverTooltip =
    "Define substitution pairs to convert file paths saved on one OS to be usable when loaded on another. This allows Hiero projects to be shared across different operating systems.\n\n"
    "For example, if you set 'z:' in the Windows column, and '/Volumes/networkmount' in the OSX column:\n"
    "\tOn Windows, any clip paths encountered that start with '/Volumes/networkmount' will be converted to start with 'z:' instead.\n"
    "\tOn OSX, any clip paths encountered that start with 'z:' will be converted to start with '/Volumes/networkmount' instead.\n";
}

HotkeyEditorModel::~HotkeyEditorModel()
{
  delete rootItem;
}

void HotkeyEditorModel::setHotkeys(const HotkeysMap& hotkeys)
{
  beginResetModel();
  _hotkeys = hotkeys;
  setupModelData(rootItem);
  endResetModel();
}

HotkeysMap HotkeyEditorModel::getHotkeys() const
{
  return _hotkeys;
}

QModelIndex HotkeyEditorModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent)) {
    return QModelIndex();
  }

  HotkeyEditorModelItem *parentItem;

  if (!parent.isValid()) {
    parentItem = rootItem;
  }
  else {
    parentItem = static_cast<HotkeyEditorModelItem*>(parent.internalPointer());
  }

  HotkeyEditorModelItem *childItem = parentItem->child(row);
  if (childItem) {
    return createIndex(row, column, childItem);
  }

  return QModelIndex();
}

QModelIndex HotkeyEditorModel::parent(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return QModelIndex();
  }

  HotkeyEditorModelItem *childItem = static_cast<HotkeyEditorModelItem*>(index.internalPointer());
  HotkeyEditorModelItem *parentItem = childItem->parentItem();

  if (parentItem == rootItem) {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int HotkeyEditorModel::rowCount(const QModelIndex &parent) const
{
  HotkeyEditorModelItem *parentItem;
  if (parent.column() > 0) {
    return 0;
  }

  if (!parent.isValid()) {
    parentItem = rootItem;
  }
  else {
    parentItem = static_cast<HotkeyEditorModelItem*>(parent.internalPointer());
  }

  return parentItem->childCount();
}

int HotkeyEditorModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid()) {
    return static_cast<HotkeyEditorModelItem*>(parent.internalPointer())->columnCount();
  }

  return rootItem->columnCount();
}

QVariant HotkeyEditorModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }

  HotkeyEditorModelItem *item = static_cast<HotkeyEditorModelItem*>(index.internalPointer());

  if (role == Qt::ForegroundRole
      && index.column() == static_cast<int>(Column::Hotkey)) {
    if (item->data(static_cast<int>(Column::Hotkey)) != item->data(static_cast<int>(Column::DefaultHotkey))) {
      return QVariant(QApplication::palette().color(QPalette::Highlight));
    }
  }

  if (role != Qt::DisplayRole && role != Qt::EditRole) {
    return QVariant();
  }

  return item->data(index.column());
}

Qt::ItemFlags HotkeyEditorModel::flags(const QModelIndex &index) const
{
  if (!index.isValid()) {
    return Qt::NoItemFlags;
  }

  Qt::ItemFlags modelFlags = QAbstractItemModel::flags(index);
  if (index.column() == static_cast<int>(Column::Hotkey)) {
    modelFlags |= Qt::ItemIsEditable;
  }

  return modelFlags;
}

QVariant HotkeyEditorModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return rootItem->data(section);
  }

  return QVariant();
}

void HotkeyEditorModel::setupModelData(HotkeyEditorModelItem *parent)
{
  QAction* nullAction = nullptr;
  // Go through each context, one context - many categories each iteration
  for (const auto& contextLevel : _hotkeys) {
    // TODO: make it "tr()".
    HotkeyEditorModelItem* contextLevelItem = new HotkeyEditorModelItem({contextLevel.first, QVariant::fromValue(nullAction), QString()}, parent);
    parent->appendChild(contextLevelItem);
    // Go through each category, one category - many actions each iteration
    for (const auto& categoryLevel : contextLevel.second) {
      HotkeyEditorModelItem* categoryLevelItem = new HotkeyEditorModelItem({categoryLevel.first, QVariant::fromValue(nullAction), QString()}, contextLevelItem);
      contextLevelItem->appendChild(categoryLevelItem);
      for (const auto& action : categoryLevel.second) {
        QString name = action->text();
        if (name.isEmpty() || action == nullptr) {
          continue;
        }
        QString defaultHotkey = action->property(kDefaultShortcutPropertyName).value<QKeySequence>().toString(QKeySequence::NativeText);
        HotkeyEditorModelItem* actionLevelItem = new HotkeyEditorModelItem({name, QVariant::fromValue(reinterpret_cast<void*>(action)), defaultHotkey}, categoryLevelItem);
        categoryLevelItem->appendChild(actionLevelItem);
      }
    }
  }
}

bool HotkeyEditorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (role == Qt::EditRole && index.column() == static_cast<int>(Column::Hotkey)) {
    QString keySequenceString= value.toString();
    HotkeyEditorModelItem *item = static_cast<HotkeyEditorModelItem*>(index.internalPointer());
    if (keySequenceString.isEmpty()) {
      item->setData(static_cast<int>(Column::Hotkey), keySequenceString);
      Q_EMIT dataChanged(index, index);
      return true;
    }

    HotkeyEditorModelItem* foundItem = findKeySequence(keySequenceString);
    const HotkeyEditorModelItem *currentItem = static_cast<HotkeyEditorModelItem*>(index.internalPointer());
    if (!foundItem || currentItem == foundItem) {
      item->setData(static_cast<int>(Column::Hotkey), keySequenceString);
      return true;
    }

    QMessageBox messageBox;
    messageBox.setWindowTitle("Reassign hotkey?");
    messageBox.setIcon(QMessageBox::Warning);
    const QString foundNameString = foundItem->data(static_cast<int>(Column::Name)).toString();
    const QString foundHotkeyString = foundItem->data(static_cast<int>(Column::Hotkey)).toString();
    const QString text = QLatin1String("Keyboard hotkey \"") + foundHotkeyString + QLatin1String("\" is already assigned to \"") + foundNameString + QLatin1String("\".");
    messageBox.setText(text);
    messageBox.setInformativeText(tr("Are you sure you want to reassign this hotkey?"));
    messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    messageBox.setDefaultButton(QMessageBox::No);

    const int ret = messageBox.exec();
    switch (ret) {
      case QMessageBox::Yes:
        foundItem->setData(static_cast<int>(Column::Hotkey), QVariant());
        item->setData(static_cast<int>(Column::Hotkey), keySequenceString);
        break;
      case QMessageBox::No:
        break;
      default:
        break;
    }
  }

  return QAbstractItemModel::setData(index, value, role);
}

HotkeyEditorModelItem* HotkeyEditorModel::findKeySequence(const QString& keySequenceString)
{
  for (int i = 0; i < rootItem->childCount(); ++i) {
    HotkeyEditorModelItem* contextLevel = rootItem->child(i);
    for (int j = 0; j < contextLevel->childCount(); ++j) {
      HotkeyEditorModelItem* categoryLevel = contextLevel->child(j);
      for (int k = 0; k < categoryLevel->childCount(); ++k) {
        HotkeyEditorModelItem* actionLevel = categoryLevel->child(k);
        const QVariant actionLevelHotkey = actionLevel->data(static_cast<int>(Column::Hotkey));
        if (keySequenceString == actionLevelHotkey.toString()) {
          return actionLevel;
        }
      }
    }
  }

  return nullptr;
}

void HotkeyEditorModel::resetAll()
{
  for (int i = 0; i < rootItem->childCount(); ++i) {
    HotkeyEditorModelItem* contextLevel = rootItem->child(i);
    for (int j = 0; j < contextLevel->childCount(); ++j) {
      HotkeyEditorModelItem* categoryLevel = contextLevel->child(j);
      for (int k = 0; k < categoryLevel->childCount(); ++k) {
        HotkeyEditorModelItem* actionLevel = categoryLevel->child(k);
        actionLevel->setData(static_cast<int>(Column::Hotkey), actionLevel->data(static_cast<int>(Column::DefaultHotkey)));
      }
    }
  }
  Q_EMIT dataChanged(QModelIndex(), QModelIndex());
}

QModelIndex HotkeyEditorModel::reset(const QModelIndexList& selectedItems)
{
  for (const QModelIndex &selectedItem : selectedItems) {
    HotkeyEditorModelItem *item = static_cast<HotkeyEditorModelItem*>(selectedItem.internalPointer());
    item->setData(static_cast<int>(Column::Hotkey), item->data(static_cast<int>(Column::DefaultHotkey)));
    Q_EMIT dataChanged(selectedItem, selectedItem);
  }
  return QModelIndex();
}

void HotkeyEditorModel::setHoverTooltipText(const QString& hoverTooltipText)
{
  _hoverTooltip = hoverTooltipText;
}

const QString& HotkeyEditorModel::hoverTooltipText()
{
  return _hoverTooltip;
}

HotkeyEditorWidget::HotkeyEditorWidget(const char* objName, QWidget* parent) :
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
  _search->setPlaceholderText("Search Hotkeys");
  _search->setClearButtonEnabled(true);

  searchLayout->addWidget(_searchToolButton);
  searchLayout->addWidget(_search);

  // set up the model and view
  _view = new QTreeView(this);
  _model = new HotkeyEditorModel(_view);
  setToolTip(_model->hoverTooltipText());

  _filterModel = new QSortFilterProxyModel(this);
  _filterModel->setSourceModel(_model);
  _filterModel->setFilterKeyColumn(-1);
  _filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  _filterModel->setRecursiveFilteringEnabled(true);
  _filterModel->setDynamicSortFilter(true);

  _view->setModel(_filterModel);
  connect(_view->model(), &QAbstractItemModel::dataChanged, this, &HotkeyEditorWidget::hotkeysChanged);

  _delegate = new HotkeyEditorDelegate(_view);
  _view->setItemDelegateForColumn(1, _delegate);

  // _view->setMinimumSize(250, 300);
  _view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  // _view->resizeColumnToContents(0);
  _view->setAlternatingRowColors(true);
  _view->setSelectionBehavior(QTreeView::SelectRows);
  _view->setSelectionMode(QAbstractItemView::ExtendedSelection);

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
      expandRecursively(selectedIndex, _view);
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

  connect(_search, &QLineEdit::textChanged, _filterModel, &QSortFilterProxyModel::setFilterFixedString);

  QItemSelectionModel* selectionModel = _view->selectionModel();
  connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &HotkeyEditorWidget::selectionChanged);

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0); // fill out to the entire widget area, no insets
  setLayout(layout);

  layout->addLayout(searchLayout);
  layout->addWidget(_view);
  layout->addWidget(new QSplitter(this));

  _keyboardWidget = new KeyboardWidget(this);
  _keyboardWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  layout->addWidget(_keyboardWidget);

  QHBoxLayout* buttonLayout = new QHBoxLayout();
  layout->addLayout(buttonLayout);

  _resetAllButton = new QPushButton("Reset All", this);
  connect(_resetAllButton, &QAbstractButton::clicked, this, &HotkeyEditorWidget::resetAll);
  buttonLayout->addWidget(_resetAllButton);

  _resetButton = new QPushButton("Reset", this);
  connect(_resetButton, &QAbstractButton::clicked, this, &HotkeyEditorWidget::reset);
  buttonLayout->addWidget(_resetButton);

  buttonLayout->addStretch(0);

  // _importButton = new QPushButton("Import", this);
  // connect(_importButton, &QAbstractButton::clicked, this, &HotkeyEditorWidget::importHotkeys);
  // buttonLayout->addWidget(_importButton);

  // _exportButton = new QPushButton("Export", this);
  // connect(_exportButton, &QAbstractButton::clicked, this, &HotkeyEditorWidget::exportHotkeys);
  // buttonLayout->addWidget(_exportButton);

  if (!objectName().isEmpty()) {
    /*QByteArray headerColumns = settings.value(HOTKEY_EDITOR_HEADER_PREFERENCE_KEY + sPlatformStrings[sCurrentPlaform]).toByteArray();
    if (!headerColumns.isEmpty()) {
      treeHeader->restoreState(headerColumns);
    }*/
  }

  // update the selection, so that the buttons are in the right state
  selectionChanged();
}

HotkeyEditorWidget::~HotkeyEditorWidget()
{
  if (objectName().length() > 0){
    /* QHeaderView* treeHeader = _view->horizontalHeader();
    AppSettings settings;
    settings.beginGroup(objectName());
    settings.setValue(HOTKEY_EDITOR_HEADER_PREFERENCE_KEY, treeHeader->saveState()); */
  }
}

void HotkeyEditorWidget::setHoverTooltipText(const QString& hoverTooltipText)
{
  _model->setHoverTooltipText(hoverTooltipText);
  setToolTip(_model->hoverTooltipText());
}

void HotkeyEditorWidget::setHotkeys(const HotkeysMap& hotkeys)
{
  _model->setHotkeys(hotkeys);

  _searchToolButtonMenu->addSection("Search");

  QActionGroup* actionGroup = new QActionGroup(this);

  _nameAction = _searchToolButtonMenu->addAction(tr("Name"));
  _nameAction->setCheckable(true);
  _nameAction->setChecked(true);
  actionGroup->addAction(_nameAction);

  _hotkeyAction = _searchToolButtonMenu->addAction(tr("Hotkey"));
  _hotkeyAction->setCheckable(true);
  _hotkeyAction->setChecked(false);
  actionGroup->addAction(_hotkeyAction);

  _searchToolButtonMenu->addSection("Context");

  _allContextsAction = _searchToolButtonMenu->addAction(tr("All"));
  _allContextsAction->setCheckable(true);
  _allContextsAction->setChecked(true);
  connect(_allContextsAction, &QAction::triggered, [this](const bool triggered){
    if (!triggered) {
      return;
    }

    for (QAction* action : contextActions) {
      action->setChecked(triggered);
    }
  });

  _searchToolButtonMenu->addSeparator();

  for (const auto& context : hotkeys) {
    QAction* contextAction = _searchToolButtonMenu->addAction(context.first);
    contextAction->setCheckable(true);
    contextAction->setChecked(true);
    contextActions.push_back(contextAction);
    connect(contextAction, &QAction::triggered, [this, &hotkeys, &context](const bool triggered){
      std::vector<QKeySequence> hotkeyVector;
      HotkeysMap hotkeysMap = _model->getHotkeys();
      for (const auto& category : hotkeysMap[context.first]) {
        for (const auto& action : category.second) {
          hotkeyVector.push_back(action->shortcut());
        }
      }
      std::vector<QColor> colors{Qt::white, Qt::black, Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow, Qt::darkRed, Qt::darkGreen, Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow};
      _keyboardWidget->setHotkeys(hotkeyVector, colors[std::distance(hotkeys.begin(), hotkeys.find(context.first))]);
    });

  }

  _searchToolButtonMenu->addSection("Hotkey");

  _defaultHotkeyAction = _searchToolButtonMenu->addAction(tr("Default Shortcut"));
  _defaultHotkeyAction->setCheckable(true);
  _defaultHotkeyAction->setChecked(true);

  _nonDefaultHotkeyAction = _searchToolButtonMenu->addAction(tr("Non-default Shortcut"));
  _nonDefaultHotkeyAction->setCheckable(true);
  _nonDefaultHotkeyAction->setChecked(true);

  _searchToolButtonMenu->addSection("Match");

  QActionGroup* matchActionGroup = new QActionGroup(this);

  _matchContainsAction = _searchToolButtonMenu->addAction(tr("Contains"));
  _matchContainsAction->setCheckable(true);
  _matchContainsAction->setChecked(true);
  matchActionGroup->addAction(_matchContainsAction);

  _matchExactlyAction = _searchToolButtonMenu->addAction(tr("Exactly"));
  _matchExactlyAction->setCheckable(true);
  _matchExactlyAction->setChecked(false);
  matchActionGroup->addAction(_matchExactlyAction);

  _matchStartsWithAction = _searchToolButtonMenu->addAction(tr("Starts with"));
  _matchStartsWithAction->setCheckable(true);
  _matchStartsWithAction->setChecked(false);
  matchActionGroup->addAction(_matchStartsWithAction);

  _matchEndsWithAction = _searchToolButtonMenu->addAction(tr("Ends with"));
  _matchEndsWithAction->setCheckable(true);
  _matchEndsWithAction->setChecked(false);
  matchActionGroup->addAction(_matchEndsWithAction);

  _matchWildcardAction = _searchToolButtonMenu->addAction(tr("Wildcard"));
  _matchWildcardAction->setCheckable(true);
  _matchWildcardAction->setChecked(false);
  matchActionGroup->addAction(_matchWildcardAction);

  _matchRegularExpressionAction = _searchToolButtonMenu->addAction(tr("Regular Expression"));
  _matchRegularExpressionAction->setCheckable(true);
  _matchRegularExpressionAction->setChecked(false);
  matchActionGroup->addAction(_matchRegularExpressionAction);

  // make sure the button states are properly updated
  // TODO: do we need this?
  selectionChanged();
}

HotkeysMap HotkeyEditorWidget::getHotkeys() const
{
  return _model->getHotkeys();
}

void HotkeyEditorWidget::resetAll()
{
  _model->resetAll();
}

void HotkeyEditorWidget::reset()
{
  QModelIndexList selectedItems = _view->selectionModel()->selectedIndexes();

  QModelIndexList sourceSelectedItems;
  for (const QModelIndex& selectedItem : selectedItems) {
    sourceSelectedItems.push_back(_filterModel->mapToSource(selectedItem));
  }

  QModelIndex newItem = _model->reset(sourceSelectedItems);
}

void HotkeyEditorWidget::expandRecursively(const QModelIndex& index, QTreeView* view)
{
  if (!index.isValid()) {
    return;
  }

  for (int i = 0; i < index.model()->rowCount(index); i++) {
    const QModelIndex &child = index.child(i, 0);
    expandRecursively(child, view);
  }

  if (!view->isExpanded(index)) {
    view->expand(index);
  }
}

void HotkeyEditorWidget::importHotkeys()
{
}

void HotkeyEditorWidget::exportHotkeys()
{
}

// TODO: do we need this method at all? Do not think we need anything specific that needs changing when the selection changes?
void HotkeyEditorWidget::selectionChanged()
{
}
