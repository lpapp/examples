
#include "hotkeyEditorWidget.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QKeySequenceEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>

#include <QtCore/QAbstractItemModel>

#include <cassert>
#include <iostream>
#include <set>

static const char* HOTKEY_EDITOR_HEADER_PREFERENCE_KEY = "headerColumns";

HotkeyEditorModelItem::HotkeyEditorModelItem(const std::vector<QVariant> &data, HotkeyEditorModelItem *parent)
    : m_itemData(data), m_parentItem(parent)
{}

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
  if (column == static_cast<int>(Column::Hotkey) && !columnVariant.canConvert<QString>()) {
    QAction* action = static_cast<QAction*>(columnVariant.value<void*>());
    // std::cout << "TEST ITEM DATA 0: " << std::endl;
    if (!action) {
      return QVariant();
    }
    QKeySequence keySequence = action->shortcut();
    QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
    std::cout << "TEST ITEM DATA: " << keySequenceString.toStdString() << std::endl;
    return keySequenceString;
  }
  
  return columnVariant;
}

bool HotkeyEditorModelItem::setData(int column, const QVariant& value)
{
  if (column < 0 || column >= m_itemData.size()) {
    return false;
  }

  if (column == static_cast<int>(Column::Hotkey)) {
    QAction* action = static_cast<QAction*>(m_itemData[column].value<void*>());
    action->setShortcut(QKeySequence::fromString(value.toString(), QKeySequence::NativeText));
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
  return editor;
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
  QKeySequenceEdit *keySequenceEdit = static_cast<QKeySequenceEdit*>(editor);
  QKeySequence keySequence = keySequenceEdit->keySequence();

  model->setData(index, keySequence.toString(QKeySequence::NativeText), Qt::EditRole);
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
  rootItem = new HotkeyEditorModelItem({tr("Name"), tr("Hotkey"), tr("Description")});
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

void HotkeyEditorModel::setHotkeys(const std::vector<HotkeyEntry>& hotkeys)
{
  beginResetModel();
  _hotkeys = hotkeys;
  setupModelData(rootItem);
  endResetModel();
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

  if (role != Qt::DisplayRole && role != Qt::EditRole) {
    return QVariant();
  }

  HotkeyEditorModelItem *item = static_cast<HotkeyEditorModelItem*>(index.internalPointer());

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
    for (const auto& firstLevel : _hotkeys) {
      // TODO: make it "tr()".
      QAction* firstLevelAction = nullptr;
      HotkeyEditorModelItem* firstLevelItem = new HotkeyEditorModelItem({firstLevel.first, QVariant::fromValue(firstLevelAction), QString()}, parent);
      parent->appendChild(firstLevelItem);
      for (const auto& secondLevel : firstLevel.second) {
        QAction* action = std::get<1>(secondLevel);
        QKeySequence keySequence = action->shortcut();
        QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
        std::cout << "TEST SETUP MODEL DATA: " << keySequenceString.toStdString() << std::endl;
        HotkeyEditorModelItem* secondLevelItem = new HotkeyEditorModelItem({std::get<0>(secondLevel), QVariant::fromValue(reinterpret_cast<void*>(action)), std::get<2>(secondLevel)}, firstLevelItem);
        firstLevelItem->appendChild(secondLevelItem);
      }
    }
}

bool HotkeyEditorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if (role == Qt::EditRole) {
    QString newValue = value.toString();

    if (index.column() < (static_cast<int>(Column::Description) + 1)) {
      HotkeyEditorModelItem *item = static_cast<HotkeyEditorModelItem*>(index.internalPointer());
      item->setData(static_cast<int>(Column::Hotkey), value);
      Q_EMIT dataChanged(index, index);
      return true;
    }
  }

  return QAbstractItemModel::setData(index, value, role);
}

void HotkeyEditorModel::resetAll()
{
  beginResetModel();
  // TODO: implement me, maybe have a static default data container and reset it to that here?
  // _hotkeys.clear();
  endResetModel();
}

QModelIndex HotkeyEditorModel::reset(const QModelIndexList& selectedItems)
{
  // TODO: implement me
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


//---------------------------------------------------------------------------------------------------------------------------------

HotkeyEditorWidget::HotkeyEditorWidget(const char* objName, QWidget* parent) :
  QWidget(parent),
  _resetAllButton(nullptr),
  _resetButton(nullptr)
{
  if (objName) {
    setObjectName(objName);
  }

  _search = new QLineEdit(this);
  _search->setPlaceholderText("Search Hotkeys");
  _search->addAction(QIcon("icons:Search-icon2.png"), QLineEdit::LeadingPosition);

  // set up the model and view
  _view = new QTreeView(this);
  _view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  _model = new HotkeyEditorModel(_view);
  setToolTip(_model->hoverTooltipText());
  // TODO: "new" signal/slot syntax.
  bool connected = connect(_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), SIGNAL(remapsChanged()));
  assert(connected);
  _view->setModel(_model);
  _delegate = new HotkeyEditorDelegate(_view);
  _view->setItemDelegateForColumn(1, _delegate);

  QItemSelectionModel* selectionModel = _view->selectionModel();
  connect(selectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), SLOT(selectionChanged()));

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0); // fill out to the entire widget area, no insets
  setLayout(layout);

  layout->addWidget(_search);
  layout->addWidget(_view);

  QHBoxLayout* buttonLayout = new QHBoxLayout();
  layout->addLayout(buttonLayout);

  _resetAllButton = new QPushButton("Reset All", this);
  // TODO: "new" signal/slot syntax
  connect(_resetAllButton, SIGNAL(clicked()), SLOT(resetAll()));
  buttonLayout->addWidget(_resetAllButton);

  _resetButton = new QPushButton("Reset", this);
  connect(_resetButton, SIGNAL(clicked()), SLOT(reset()));
  buttonLayout->addWidget(_resetButton);

  buttonLayout->addStretch(0);

  _importButton = new QPushButton("Import", this);
  connect(_importButton, SIGNAL(clicked()), SLOT(importHotkeys()));
  buttonLayout->addWidget(_importButton);

  _exportButton = new QPushButton("Export", this);
  connect(_exportButton, SIGNAL(clicked()), SLOT(exportHotkeys()));
  buttonLayout->addWidget(_exportButton);

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

void HotkeyEditorWidget::setHotkeys(const std::vector<HotkeyEntry>& hotkeys)
{
  _model->setHotkeys(hotkeys);

  // make sure the button states are properly updated
  selectionChanged();
}

void HotkeyEditorWidget::resetAll()
{
  _model->resetAll();
  Q_EMIT hotkeysChanged();
}

void HotkeyEditorWidget::reset()
{
  QModelIndexList selectedItems = _view->selectionModel()->selectedIndexes();
  QModelIndex newItem = _model->reset(selectedItems);

  // select the last item
  _view->setCurrentIndex(newItem);

  // edit it
  _view->edit(newItem);

  Q_EMIT hotkeysChanged();
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
