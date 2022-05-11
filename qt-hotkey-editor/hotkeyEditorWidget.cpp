#include "hotkeyEditorWidget.h"

#include <QtWidgets/QAction>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QKeySequenceEdit>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QSignalMapper>
#include <QtCore/QSortFilterProxyModel>

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
    if (!action) {
      return QVariant();
    }
    QKeySequence keySequence = action->shortcut();
    QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
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
  // TODO: do not show the editor for categories. Instead, treat the whole row
  // to expand or collapse?
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
  const QKeySequenceEdit *keySequenceEdit = static_cast<QKeySequenceEdit*>(editor);
  const QKeySequence keySequence = keySequenceEdit->keySequence();
  QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
  if (keySequenceString.isEmpty()) {
    model->setData(index, keySequenceString, Qt::EditRole);
    return;
  }

  HotkeyEditorModel* hotkeyEditorModel = static_cast<HotkeyEditorModel*>(model);
  HotkeyEditorModelItem* foundItem = hotkeyEditorModel->findKeySequence(keySequenceString);
  const HotkeyEditorModelItem *currentItem = static_cast<HotkeyEditorModelItem*>(index.internalPointer());
  if (!foundItem || currentItem == foundItem) {
    model->setData(index, keySequenceString, Qt::EditRole);
    return;
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
      model->setData(index, keySequenceString, Qt::EditRole);
      foundItem->setData(static_cast<int>(Column::Hotkey), QVariant());
      break;
    case QMessageBox::No:
      break;
    default:
      break;
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

std::vector<HotkeyEntry> HotkeyEditorModel::getHotkeys() const
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
      HotkeyEditorModelItem* firstLevelItem = new HotkeyEditorModelItem({firstLevel.first, QVariant::fromValue(firstLevelAction), QString(), QString()}, parent);
      parent->appendChild(firstLevelItem);
      for (const auto& secondLevel : firstLevel.second) {
        QAction* action = std::get<1>(secondLevel);
        QKeySequence keySequence = action->shortcut();
        QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
        HotkeyEditorModelItem* secondLevelItem = new HotkeyEditorModelItem({std::get<0>(secondLevel), QVariant::fromValue(reinterpret_cast<void*>(action)), std::get<2>(secondLevel), std::get<3>(secondLevel)}, firstLevelItem);
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

HotkeyEditorModelItem* HotkeyEditorModel::findKeySequence(const QString& keySequenceString)
{
  for (int i = 0; i < rootItem->childCount(); ++i) {
    HotkeyEditorModelItem* firstLevel = rootItem->child(i);
    for (int j = 0; j < firstLevel->childCount(); ++j) {
      HotkeyEditorModelItem* secondLevel = firstLevel->child(j);
      const QVariant secondLevelHotkey = secondLevel->data(static_cast<int>(Column::Hotkey));
      if (keySequenceString == secondLevelHotkey.toString()) {
        return secondLevel;
      }
    }
  }

  return nullptr;
}

void HotkeyEditorModel::resetAll()
{
  for (int i = 0; i < rootItem->childCount(); ++i) {
    HotkeyEditorModelItem* firstLevel = rootItem->child(i);
    for (int j = 0; j < firstLevel->childCount(); ++j) {
      HotkeyEditorModelItem* secondLevel = firstLevel->child(j);
      secondLevel->setData(static_cast<int>(Column::Hotkey), secondLevel->data(static_cast<int>(Column::DefaultHotkey)));
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

std::vector<std::vector<int>> keyboardLayout {
    { Qt::Key_1, Qt::Key_2, Qt::Key_3, Qt::Key_4, Qt::Key_5, Qt::Key_6,
      Qt::Key_7, Qt::Key_8, Qt::Key_9, Qt::Key_0, Qt::Key_Backspace },

    { Qt::Key_Tab, Qt::Key_Q, Qt::Key_W, Qt::Key_E, Qt::Key_R, Qt::Key_T,
      Qt::Key_Z, Qt::Key_U, Qt::Key_I, Qt::Key_O, Qt::Key_P },

    { Qt::Key_A, Qt::Key_S, Qt::Key_D, Qt::Key_F, Qt::Key_G, Qt::Key_H,
      Qt::Key_J, Qt::Key_K, Qt::Key_L },

    { Qt::Key_Shift, Qt::Key_Y, Qt::Key_X, Qt::Key_C, Qt::Key_V, Qt::Key_B,
      Qt::Key_N, Qt::Key_M, Qt::Key_Enter },

    { Qt::Key_Control, Qt::Key_Meta, Qt::Key_Alt, Qt::Key_Space }
};

KeyboardWidget::KeyboardWidget(QWidget *parent)
  : QWidget(parent)
{
  QGridLayout *gridLayout = new QGridLayout(this);
  gridLayout->setSpacing(0);
  gridLayout->setColumnStretch(0, 0);
  gridLayout->setRowStretch(0, 0);
  gridLayout->setContentsMargins(0, 0, 0, 0);
  QSignalMapper *mapper = new QSignalMapper(this);
  // connect(mapper, &QSignalMapper::mapped, this, &KeyboardWidget::buttonClicked);

  int row = 0;
  int column = 0;

  for (auto& keyboardRow : keyboardLayout) {
    for (auto& key : keyboardRow) {
      QKeySequence keySequence(key);
      QString keySequenceString = keySequence.toString(QKeySequence::NativeText);
      QPushButton *button = new QPushButton(keySequenceString);
      button->setFixedWidth(40);
      button->setContentsMargins(0, 0, 0, 0);

      // mapper->setMapping(button, keyString.first);
      // connect(button, &QPushButton::clicked, mapper, &QSignalMapper::map);

      gridLayout->addWidget(button, row, column);
      ++column;
    }
    ++row;
    column = 0;
  }
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void KeyboardWidget::buttonClicked(int key)
{
  if ((key == Qt::Key_Enter) || (key == Qt::Key_Backspace)) {
    emit specialKeyClicked(key);
  }
  else {
    // emit keyClicked(keyToCharacter(key));
  }
}

//---------------------------------------------------------------------------------------------------------------------------------

HotkeyEditorWidget::HotkeyEditorWidget(const char* objName, QWidget* parent) :
  QWidget(parent)
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
  _view->setAlternatingRowColors(true);
  _view->setMinimumSize(400, 300);
  _view->setSelectionMode(QAbstractItemView::ExtendedSelection);

  // Note: this would need to be applied after filling in the data. But also,
  // this only seems to adjust based on the header content, not body. So,
  // probably not good.
  // _view->resizeColumnToContents(0);
  // Note: this does not allow the user to resize the column
  // _view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

  _model = new HotkeyEditorModel(_view);
  setToolTip(_model->hoverTooltipText());
  _filterModel = new QSortFilterProxyModel(this);
  _filterModel->setSourceModel(_model);
  // bool connected = connect(_model, &QAbstractItemModel::dataChanged, this, &HotkeyEditorWidget::hotkeysChanged);
  assert(connected);
  _view->setModel(_filterModel);
  // _view->setModel(_model);
  _delegate = new HotkeyEditorDelegate(_view);
  _view->setItemDelegateForColumn(1, _delegate);

  connect(_search, &QLineEdit::textChanged, _filterModel, &QSortFilterProxyModel::setFilterFixedString);
  // connect(_search, &QLineEdit::textChanged, _filterModel, &QSortFilterProxyModel::setFilterRegExp);

  QItemSelectionModel* selectionModel = _view->selectionModel();
  connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &HotkeyEditorWidget::selectionChanged);

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0); // fill out to the entire widget area, no insets
  setLayout(layout);

  layout->addWidget(_search);
  layout->addWidget(_view);

  QHBoxLayout* buttonLayout = new QHBoxLayout();
  layout->addLayout(buttonLayout);

  _resetAllButton = new QPushButton("Reset All", this);
  connect(_resetAllButton, &QAbstractButton::clicked, this, &HotkeyEditorWidget::resetAll);
  buttonLayout->addWidget(_resetAllButton);

  _resetButton = new QPushButton("Reset", this);
  connect(_resetButton, &QAbstractButton::clicked, this, &HotkeyEditorWidget::reset);
  buttonLayout->addWidget(_resetButton);

  buttonLayout->addStretch(0);

  _importButton = new QPushButton("Import", this);
  connect(_importButton, &QAbstractButton::clicked, this, &HotkeyEditorWidget::importHotkeys);
  buttonLayout->addWidget(_importButton);

  _exportButton = new QPushButton("Export", this);
  connect(_exportButton, &QAbstractButton::clicked, this, &HotkeyEditorWidget::exportHotkeys);
  buttonLayout->addWidget(_exportButton);

  layout->addWidget(new KeyboardWidget());

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

std::vector<HotkeyEntry> HotkeyEditorWidget::getHotkeys() const
{
  return _model->getHotkeys();
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




