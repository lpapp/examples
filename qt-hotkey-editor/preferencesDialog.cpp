#include "preferenceswidget.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTreeView>

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
  selectionModel()->select(_model->index(0, 0), QItemSelectionModel::SelectCurrent);
}

void PreferencesWidget::addPage(int pageId, SettingsPage* page)
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
  const int pageId = current.data(SettingsPage::PageIdRole).toInt();
  std::map<int, int>::const_iterator it = _pageIdToStackIndex.find(pageId);
  if (it != _pageIdToStackIndex.end()) {
    _pageStackWidget->setCurrentIndex(it->second);
    Q_EMIT pageChanged(current);
  }
}

void PreferencesWidget::applyPreferences()
{
  for (int i = 0; i < _pageStackWidget->count(); ++i) {
    static_cast<SettingsPage*>(_pageStackWidget->widget(i))->apply();
  }
}

void PreferencesWidget::cancelPreferences()
{
  for (int i = 0; i < _pageStackWidget->count(); ++i) {
    static_cast<SettingsPage*>(_pageStackWidget->widget(i))->cancel();
  }
}

void PreferencesWidget::revertPreferences()
{
  /* DefaultPreferences defaults;
  for (int i = 0; i < _pageStackWidget->count(); ++i) {
    static_cast<SettingsPage*>(_pageStackWidget->widget(i))->loadSettings(&defaults);
  } */
}

