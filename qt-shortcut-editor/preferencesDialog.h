#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QWidget>

#include <map>

class QAbstractItemModel;
class QStackedWidget;
class QTreeView;

enum class Page : uint8_t
{
  KeyboardShortcuts,

  User
};

class PreferencesLayout : public QFormLayout
{
public:
  PreferencesLayout(QWidget* parent = nullptr);

  void addRowWidgets(const QString& labelText, const QList<QWidget*>& widgets);
  void addDivider(const QString& text, int row = -1);
  void addEmptyRow();
};

class PreferencesPageBase : public QWidget
{
  Q_OBJECT

public:
  PreferencesPageBase(QWidget* parent = nullptr);
  ~PreferencesPageBase() override {}

public Q_SLOTS:
  virtual void initialize();
  virtual void apply();
  virtual void cancel();
  virtual void revert();
};

class PreferencesPage : public PreferencesPageBase
{
  Q_OBJECT

public:
  static const int PageIdRole;

  PreferencesPage(QWidget* parent = nullptr);
  ~PreferencesPage() override = default;

  virtual void loadSettings();
  virtual void saveSettings();
  virtual void cancelSettings();

public Q_SLOTS:
  void initialize() override;
  void apply() override;
  void cancel() override;
  void revert() override;
};

class KeyboardShortcutsPreferencesPage : public PreferencesPage
{
  Q_OBJECT

public:
  KeyboardShortcutsPreferencesPage(QWidget* parent = nullptr);

  void loadSettings() override;
  void saveSettings() override;
};

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
  void addPage(Page page, PreferencesPage* preferencesPage);
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
  std::map<Page, int> _pageToStackIndex;
};

class PreferencesDialog : public QDialog
{
  Q_OBJECT

public:
  PreferencesDialog(QWidget* parent);

public Q_SLOTS:
  void accept() override;
  void reject() override;
  void revert();

  void onPageChanged(QModelIndex);

  static void SetPage(Page page);

private:
  void init();

  QStandardItem* createPage(Page page, QStandardItem* parent);
  QString pageTitle(Page page) const;
  PreferencesPage* createPageWidget(Page page);
  QStandardItem* createTreeItem(Page page, QStandardItem* parent = nullptr) const;

  PreferencesWidget* _preferencesWidget;
  static Page _previousPage;
  static const int PageIdRole;
};

#endif
