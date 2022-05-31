#include "hotkeyEditorWidget.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QKeySequenceEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QVBoxLayout>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QDebug>
#include <QtCore/QSortFilterProxyModel>

#include <iostream>

HotkeyEditorModelItem::HotkeyEditorModelItem(const std::vector<QVariant>& data, const QString& id, HotkeyEditorModelItem* parent)
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
  if (column != static_cast<int>(Column::Hotkey) || columnVariant.canConvert<QString>()) {
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
    std::cout << "TEST ITEM SET DATA: " << value.toString().toStdString() << std::endl;
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

QAction* HotkeyEditorModelItem::action() const
{
  QVariant actionVariant = m_itemData.at(static_cast<int>(Column::Hotkey));
  return static_cast<QAction*>(actionVariant.value<void*>());
}

HotkeyEditorModel::HotkeyEditorModel(QObject* parent)
  : QAbstractItemModel(parent)
{
  rootItem = new HotkeyEditorModelItem({tr("Name"), tr("Hotkey")}, QString("root"));
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
    modelFlags |= Qt::ItemIsEditable | Qt::ItemIsDropEnabled;
  }
  else if (index.column() == static_cast<int>(Column::Name)) {
    modelFlags |= Qt::ItemIsDragEnabled;
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
  const QString contextIdPrefix = "root";
  // Go through each context, one context - many categories each iteration
  for (const auto& contextLevel : _hotkeys) {
    // TODO: make it "tr()".
    HotkeyEditorModelItem* contextLevelItem = new HotkeyEditorModelItem({contextLevel.first, QVariant::fromValue(nullAction), QString()}, contextIdPrefix + contextLevel.first, parent);
    parent->appendChild(contextLevelItem);
    // Go through each category, one category - many actions each iteration
    for (const auto& categoryLevel : contextLevel.second) {
      HotkeyEditorModelItem* categoryLevelItem = new HotkeyEditorModelItem({categoryLevel.first, QVariant::fromValue(nullAction), QString()}, contextLevel.first + categoryLevel.first, contextLevelItem);
      contextLevelItem->appendChild(categoryLevelItem);
      for (const auto& action : categoryLevel.second) {
        QString name = action->text();
        if (name.isEmpty() || action == nullptr) {
          continue;
        }
        QString defaultHotkey = action->property(kDefaultShortcutPropertyName).value<QKeySequence>().toString(QKeySequence::NativeText);
        HotkeyEditorModelItem* actionLevelItem = new HotkeyEditorModelItem({name, QVariant::fromValue(reinterpret_cast<void*>(action)), defaultHotkey}, categoryLevel.first + name, categoryLevelItem);
        categoryLevelItem->appendChild(actionLevelItem);
      }
    }
  }
}

bool HotkeyEditorModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  std::cout << "TEST HOTKEY EDITOR MODEL SET DATA" << std::endl;
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
      Q_EMIT dataChanged(index, index);
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

HotkeyEditorWidget::HotkeyEditorWidget(QWidget* parent) :
  QWidget(parent)
{
  _view = new QTreeView(this);
  _model = new HotkeyEditorModel(_view);

  _filterModel = new QSortFilterProxyModel(this);
  _filterModel->setSourceModel(_model);
  _filterModel->setFilterKeyColumn(-1);
  _filterModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
  _filterModel->setRecursiveFilteringEnabled(true);
  _filterModel->setDynamicSortFilter(true);
  _view->setModel(_filterModel);

  _view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  _view->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
  _view->setAlternatingRowColors(true);
  _view->setSelectionBehavior(QTreeView::SelectRows);
  _view->setSelectionMode(QAbstractItemView::ExtendedSelection);

  connect(_view, &QTreeView::expanded, [this](){
    qDebug() << "TEST EXPANDED: " << sender();
  });

  _view->setAllColumnsShowFocus(true);
  _view->header()->resizeSection(0, 250);

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0); // fill out to the entire widget area, no insets
  setLayout(layout);
  layout->addWidget(_view);
}

void HotkeyEditorWidget::setHotkeys(const HotkeysMap& hotkeys)
{
  _model->setHotkeys(hotkeys);
}
