#ifndef OTKEYEDITORWIDGET_H
#define OTKEYEDITORWIDGET_H

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QWidget>

#include <QtCore/QString>

#include <string>

class QAction;
class QComboBox;
class QLineEdit;
class QMenu;
class QPushButton;
class QSortFilterProxyModel;
class QToolButton;
class QTreeView;

class KeyboardWidget;

static const char* kDefaultShortcutPropertyName = "defaultShortcut";
static const char* kIdPropertyName = "id";
static const std::string kDomainName = "lpapp";

// List of hotkey actions for all categories
using CategoryHotkeysMap = std::map<QString, std::vector<QAction*>>;

// List of categories for all contexts
using HotkeysMap = std::map<QString, CategoryHotkeysMap>;

enum class Column : uint8_t {
  Name,
  Hotkey,
  DefaultHotkey
};

class HotkeyEditorModelItem
{
public:
    explicit HotkeyEditorModelItem(const std::vector<QVariant>& data,
                                   HotkeyEditorModelItem* parentItem = nullptr);
    ~HotkeyEditorModelItem();

    void appendChild(HotkeyEditorModelItem *child);

    HotkeyEditorModelItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    bool setData(int column, const QVariant& value);
    int row() const;
    HotkeyEditorModelItem *parentItem();

private:
    std::vector<HotkeyEditorModelItem*> m_childItems;
    std::vector<QVariant> m_itemData;
    HotkeyEditorModelItem *m_parentItem;
};

class HotkeyEditorModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit HotkeyEditorModel(QObject* parent = nullptr);
  ~HotkeyEditorModel() override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  int rowCount(const QModelIndex& index = QModelIndex()) const override;
  int columnCount(const QModelIndex& index = QModelIndex()) const override;

  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  HotkeyEditorModelItem* findKeySequence(const QString& keySequenceString);

  void resetAll();
  QModelIndex reset(const QModelIndexList& selectedItems);

  void setHoverTooltipText(const QString& hoverTooltipText);
  const QString& hoverTooltipText();

  void setHotkeys(const HotkeysMap& hotkeys);
  HotkeysMap getHotkeys() const;

private:
  void setupModelData(HotkeyEditorModelItem* parent);

  HotkeyEditorModelItem* rootItem;
  HotkeysMap _hotkeys;
  QString _hoverTooltip;
};

class HotkeyEditorDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  HotkeyEditorDelegate(QObject* parent = nullptr);
  QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override;

  void commitAndCloseEditor();

  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const override;

  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                            const QModelIndex& index) const override;
};

class HotkeyEditorWidget : public QWidget
{
  Q_OBJECT

public:

  // specify the object name if you want settings for this widget to be autosaved and loaded (like the column widths)
  HotkeyEditorWidget(const char* objectName = nullptr, QWidget* parent = nullptr);
  ~HotkeyEditorWidget() override;

  void setHoverTooltipText(const QString& hoverTooltipText);

  void setHotkeys(const HotkeysMap& hotkeys);
  HotkeysMap getHotkeys() const;

public Q_SLOTS:
  void resetAll();
  void reset();

  void expandRecursively(const QModelIndex& index, QTreeView* view);

  void importHotkeys();
  void exportHotkeys();

  void selectionChanged();

Q_SIGNALS:
  void hotkeysChanged();

private:

  HotkeyEditorDelegate* _delegate;
  HotkeyEditorModel* _model;
  QSortFilterProxyModel* _filterModel;
  QToolButton* _searchToolButton;
  QMenu* _searchToolButtonMenu;
  QLineEdit* _search;
  QTreeView* _view;
  QComboBox* _contextComboBox;
  KeyboardWidget* _keyboardWidget;
  QPushButton* _resetAllButton;
  QPushButton* _resetButton;
  QPushButton* _importButton;
  QPushButton* _exportButton;

  QAction* _nameAction;
  QAction* _hotkeyAction;
  QAction* _allContextsAction;
  std::vector<QAction*> contextActions;
  QAction* _defaultHotkeyAction;
  QAction* _nonDefaultHotkeyAction;
  QAction* _matchContainsAction;
  QAction* _matchExactlyAction;
  QAction* _matchStartsWithAction;
  QAction* _matchEndsWithAction;
  QAction* _matchWildcardAction;
  QAction* _matchRegularExpressionAction;

};

#endif


