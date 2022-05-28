#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QtWidgets/QDialog>
#include <QtWidgets/QWidget>

#include <QtCore/QModelIndex>

#include <map>

class QAbstractItemModel;
class QStackedWidget;
class QTreeView;

class PreferencesWidget : public QWidget
{
  Q_OBJECT

public:
  explicit PreferencesWidget(QWidget* parent = nullptr);
  PreferencesWidget(QTreeView* treeView, QWidget* parent = nullptr);

  void setModel(QAbstractItemModel* model);
  void applyPreferences();
  void cancelPreferences();
  void revertPreferences();
  void addPage(int pageId, SettingsPage* page);
  void setCurrentPage(const QModelIndex& modelIndex);
  void setExpanded(const QModelIndex& modelIndex, bool expanded);

Q_SIGNALS:
  void pageChanged(QModelIndex);

private Q_SLOTS:
  void onSelectionChanged(const QModelIndex& current, const QModelIndex& previous);

private:
  void init();

  QAbstractItemModel* _model; // not owned
  QTreeView* _treeView;
  QStackedWidget* _pageStackWidget;
  std::map<int, int> _pageIdToStackIndex;
};

class PreferencesDialog : public QDialog
{
  Q_OBJECT

public:
  enum class Page
  {
    Hotkeys,

    // Values greater than ePageUser are reserved for user pages
    User
  };

  PreferencesDialog(QWidget* parent);

public Q_SLOTS:
  void accept() override;
  void reject() override;
  void revert();

  void onPageChanged(QModelIndex);

private:
  void init();
  void addUserPreferences(QStandardItemModel& model);

  QStandardItem* createPage(Page page, QStandardItem* parent);
  QString pageTitle(Page page) const;
  SettingsPage* createPageWidget(Page page);
  QStandardItem* createTreeItem(Page page, QStandardItem* parent = nullptr) const;

  SettingsTreeWidget* _preferencesWidget;
  Page _previousPage;
  static const int PageIdRole;
};

#endif
