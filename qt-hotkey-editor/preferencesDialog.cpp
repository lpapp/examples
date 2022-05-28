#include "preferencesDialog.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTreeView>

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

void PreferencesWidget::addPage(int pageId, PreferencesPage* page)
{
  _pageIdToStackIndex[pageId] = _pageStackWidget->addWidget(page);
  page->initialize();
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
  const int pageId = current.data(PreferencesPage::PageIdRole).toInt();
  if (_pageIdToStackIndex.contains(pageId)) {
    _pageStackWidget->setCurrentIndex(_pageIdToStackIndex[pageId]);
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

PreferencesDialog::PreferencePage PreferencesDialog::_previousPage = PreferencesDialog::eHotkeys;
const int PreferencesDialog::PageIdRole = Qt::UserRole + 2; // + 2 as Qt::UserRole + 1 is the default argument to setData

//------------------------------------------------------------------------------

PreferencesDialog::PreferencesDialog(QWidget* parent)
  : QDialog(parent, Qt::Tool)
{
  setAttribute(Qt::WA_DeleteOnClose,true);

  init();

  QVBoxLayout* mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(_preferencesWidget);
  QDialogButtonBox* buttonBox = new QDialogButtonBox(this);
  buttonBox->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::RestoreDefaults);
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
    case PageHot::keys: title = tr("Hotkeys"); break;
    default:
    {
      mFnAssertMsg(page > Page::User, "page was not found");
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
      pageWidget = new HotkeyEditorPreferencesPage();
      break;
    default:
      mFnAssertMsg(page > ePageUser, "page was not found");
      break;
  }
  return pageWidget;
}

QStandardItem* PreferencesDialog::createTreeItem(Page page, QStandardItem* parent) const
{
  const QString title = pageTitle(page);
  QStandardItem* item = new QStandardItem(title);
  item->setEditable(false);
  item->setData(page, SettingsPage::PageIdRole);
  if (parent) {
    parent->appendRow(item);
  }
  return item;
}

QStandardItem* PreferencesDialog::createPage(PreferencePage page, QStandardItem* parent)
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
  _preferencesWidget = new SettingsTreeWidget(this);
  _preferencesWidget->setFocusPolicy(Qt::ClickFocus);

  QStandardItemModel* standardItemModel = new QStandardItemModel(this);
  QStandardItem* hotkeysItem = createPage(Page::Hotkeys, nullptr);
  standardItemModel->appendRow(hotkeysItem);

  _preferencesWidget->setModel(standardItemModel);

  connect(_preferencesWidget, &PreferencesWidget::pageChanged, this, &PreferencesDialog::onPageChanged);

  // Find the last selected page id using the PageIdRole
  QModelIndexList matchedIndices = standardItemModel->match(standardItemModel->index(0, 0), SettingsPage::PageIdRole, _previousPage, 1, Qt::MatchRecursive);
  if (matchedIndices.size() > 0) {
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

void PreferencesDialog::SetPage(PreferencePage page)
{
  _previousPage = page;
}

