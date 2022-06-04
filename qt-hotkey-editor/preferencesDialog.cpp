#include "preferencesDialog.h"

#include "actionManager.h"
#include "hotkeyEditorWidget.h"

#include <QAction>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QTreeView>
#include <QVBoxLayout>

PreferencesLayout::PreferencesLayout(QWidget* parent)
  : QFormLayout(parent)
{
  setSpacing(4);
}

void PreferencesLayout::addRowWidgets(const QString& labelText, const QList<QWidget*>& widgets)
{
  QHBoxLayout* hBoxLayout = new QHBoxLayout();
  hBoxLayout->setContentsMargins(0, 0, 0, 0);
  for (QWidget* widget : widgets) {
    hBoxLayout->addWidget(widget);
  }
  addRow(labelText, hBoxLayout);
}

void PreferencesLayout::addDivider(const QString& text, int row)
{
  QFrame* divider = new QFrame();
  divider->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  divider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  if (row >= 0) {
    insertRow(row, text, divider);
  }
  else {
    addRow(text, divider);
  }

  QWidget* labelWidget = labelForField(divider);
  QFont labelFont = labelWidget->font();
  labelFont.setBold(true);
  labelWidget->setFont(labelFont);
}

void PreferencesLayout::addEmptyRow()
{
  addRow(tr(""), new QLabel());
}

PreferencesPageBase::PreferencesPageBase(QWidget* parent)
  : QWidget(parent)
{
}

void PreferencesPageBase::initialize()
{
}

void PreferencesPageBase::apply()
{
}

void PreferencesPageBase::cancel()
{
}

void PreferencesPageBase::revert()
{
}

const int PreferencesPage::PageIdRole = Qt::UserRole + 2; // + 2 as Qt::UserRole + 1 is the default argument to setData

PreferencesPage::PreferencesPage(QWidget* parent)
  : PreferencesPageBase(parent)
{
}

void PreferencesPage::initialize() {
}

void PreferencesPage::apply() {
}

void PreferencesPage::cancel() {
}

void PreferencesPage::revert() {
}

HotkeysPreferencesPage::HotkeysPreferencesPage(QWidget* parent)
  : PreferencesPage(parent)
{
  constexpr int maxContexts = 5;
  constexpr int maxCategories = 3;
  constexpr int maxActions = 1000;
  for (int contextIndex = 0; contextIndex < maxContexts; ++contextIndex) {
    QString contextName = "Context" + QString::number(contextIndex);
    for (int categoryIndex = 0; categoryIndex < maxCategories; ++categoryIndex) {
      QString categoryName = "Category" + QString::number(categoryIndex);
      for (int actionIndex = 0; actionIndex < maxActions; ++actionIndex) {
        QString actionName = "Action" + QString::number(actionIndex);
        QAction* action = new QAction(actionName, this);
        action->setProperty(kDefaultShortcutPropertyName, QVariant::fromValue(action->shortcut()));
        QStringList stringList{QString::fromStdString(kDomainName), contextName, categoryName, actionName};
        action->setProperty(kIdPropertyName, stringList.join('.'));
        ActionManager::registerAction(action);
      }
    }
  }

  HotkeysMap hotkeys;
  std::vector<QAction*> registeredActions = ActionManager::registeredActions();
  for (QAction* action : registeredActions) {
    QString actionId = action->property(kIdPropertyName).toString();
    QString context = actionId.split('.')[1];
    QString category = actionId.split('.')[2];

    CategoryHotkeysMap categoryHotkeys;
    if (hotkeys.count(context)) {
      categoryHotkeys = hotkeys[context];
    }

    std::vector<QAction*> actions;
    if (categoryHotkeys.count(category)) {
      actions = categoryHotkeys[category];
    }

    actions.push_back(action);

    categoryHotkeys.insert_or_assign(category, actions);
    hotkeys.insert_or_assign(context, categoryHotkeys);
  }

  PreferencesLayout* layout = new PreferencesLayout();
  HotkeyEditorWidget* hotkeyEditorWidget = new HotkeyEditorWidget;
  hotkeyEditorWidget->setHotkeys(hotkeys);

  layout->addRow(tr(""), hotkeyEditorWidget);
  setLayout(layout);
}

PreferencesWidget::PreferencesWidget(QWidget* parent)
  : QWidget(parent)
{
  _treeView = new QTreeView(this);
  _pageStackWidget = new QStackedWidget(this);
  _treeView->header()->setVisible(false);
  _treeView->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

  QHBoxLayout* hBoxLayout = new QHBoxLayout();
  hBoxLayout->setContentsMargins(0, 0, 0, 0);
  hBoxLayout->addWidget(_treeView);
  hBoxLayout->addWidget(_pageStackWidget);
  setLayout(hBoxLayout);
}

void PreferencesWidget::setModel(QAbstractItemModel* model)
{
  _model = model;
  _treeView->setModel(_model);
  _treeView->expandAll();
  QItemSelectionModel* selectionModel = _treeView->selectionModel();
  connect(selectionModel, &QItemSelectionModel::currentChanged, this, &PreferencesWidget::onSelectionChanged);
  selectionModel->select(_model->index(0, 0), QItemSelectionModel::SelectCurrent);
}

void PreferencesWidget::addPage(Page page, PreferencesPage* preferencesPage)
{
  _pageToStackIndex[page] = _pageStackWidget->addWidget(preferencesPage);
  preferencesPage->initialize();
}

void PreferencesWidget::setCurrentPage(const QModelIndex& modelIndex)
{
  _treeView->setCurrentIndex(modelIndex);
}

void PreferencesWidget::setExpanded(const QModelIndex& modelIndex, bool expanded)
{
  _treeView->setExpanded(modelIndex, expanded);
}

void PreferencesWidget::onSelectionChanged(const QModelIndex& current, const QModelIndex& previous)
{
  Page page = static_cast<Page>(current.data(PreferencesPage::PageIdRole).toInt());
  if (_pageToStackIndex.contains(page)) {
    _pageStackWidget->setCurrentIndex(_pageToStackIndex[page]);
    Q_EMIT pageChanged(current);
  }
}

void PreferencesWidget::applyPreferences()
{
  for (int i = 0; i < _pageStackWidget->count(); ++i) {
    static_cast<PreferencesPage*>(_pageStackWidget->widget(i))->apply();
  }
}

void PreferencesWidget::cancelPreferences()
{
  for (int i = 0; i < _pageStackWidget->count(); ++i) {
    static_cast<PreferencesPage*>(_pageStackWidget->widget(i))->cancel();
  }
}

void PreferencesWidget::revertPreferences()
{
  /* DefaultPreferences defaults;
  for (int i = 0; i < _pageStackWidget->count(); ++i) {
    static_cast<PreferencesPage*>(_pageStackWidget->widget(i))->loadSettings(&defaults);
  } */
}

Page PreferencesDialog::_previousPage = Page::Hotkeys;
const int PreferencesDialog::PageIdRole = Qt::UserRole + 2; // + 2 as Qt::UserRole + 1 is the default argument to setData

PreferencesDialog::PreferencesDialog(QWidget* parent)
  : QDialog(parent, Qt::Tool)
{
  setAttribute(Qt::WA_DeleteOnClose, true);

  init();

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(_preferencesWidget);
  QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
  buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults);
  buttonBox->setFocusPolicy(Qt::TabFocus);
  mainLayout->addWidget(buttonBox);
  setLayout(mainLayout);

  connect(buttonBox, &QDialogButtonBox::rejected, this, &PreferencesDialog::reject);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &PreferencesDialog::accept);
  connect(buttonBox->button(QDialogButtonBox::RestoreDefaults), &QPushButton::clicked, this, &PreferencesDialog::revert);

  adjustSize();
}

QString PreferencesDialog::pageTitle(Page page) const
{
  QString title;
  switch (page)
  {
    case Page::Hotkeys: title = tr("Keyboard shortcuts"); break;
    default:
    {
      assert("Page was not found");
      break;
    }
  }

  return title;
}

PreferencesPage* PreferencesDialog::createPageWidget(Page page)
{
  PreferencesPage* pageWidget = nullptr;
  switch (page)
  {
    case Page::Hotkeys:
      pageWidget = new HotkeysPreferencesPage();
      break;
    default:
      assert("Page was not found");
      break;
  }
  return pageWidget;
}

QStandardItem* PreferencesDialog::createTreeItem(Page page, QStandardItem* parent) const
{
  const QString title = pageTitle(page);
  QStandardItem* item = new QStandardItem(title);
  item->setEditable(false);
  item->setData(static_cast<int>(page), PreferencesPage::PageIdRole);
  if (parent) {
    parent->appendRow(item);
  }
  return item;
}

QStandardItem* PreferencesDialog::createPage(Page page, QStandardItem* parent)
{
  QStandardItem* item = nullptr;
  if (_preferencesWidget) {
    item = createTreeItem(page, parent);
    _preferencesWidget->addPage(page, createPageWidget(page));
  }
  return item;
}

void PreferencesDialog::init()
{
  _preferencesWidget = new PreferencesWidget(this);
  _preferencesWidget->setFocusPolicy(Qt::ClickFocus);

  QStandardItemModel* standardItemModel = new QStandardItemModel(this);
  QStandardItem* hotkeysItem = createPage(Page::Hotkeys, nullptr);
  standardItemModel->appendRow(hotkeysItem);

  _preferencesWidget->setModel(standardItemModel);

  connect(_preferencesWidget, &PreferencesWidget::pageChanged, this, &PreferencesDialog::onPageChanged);

  // Find the last selected page id using the PageIdRole
  QModelIndexList matchedIndices = standardItemModel->match(standardItemModel->index(0, 0), PreferencesPage::PageIdRole, static_cast<int>(_previousPage), 1, Qt::MatchRecursive);
  if (!matchedIndices.isEmpty()) {
    _preferencesWidget->setCurrentPage(matchedIndices[0]);
  }
}

void PreferencesDialog::accept()
{
  _preferencesWidget->applyPreferences();
  QDialog::accept();
}

void PreferencesDialog::reject()
{
  _preferencesWidget->cancelPreferences();
  QDialog::reject();
}

void PreferencesDialog::revert()
{
  _preferencesWidget->revertPreferences();
}

void PreferencesDialog::onPageChanged(QModelIndex index)
{
  _previousPage = static_cast<Page>(index.data(PreferencesPage::PageIdRole).toInt());
}

void PreferencesDialog::SetPage(Page page)
{
  _previousPage = page;
}

