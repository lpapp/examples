#ifndef HOTKEYEDITORWIDGET_H
#define HOTKEYEDITORWIDGET_H

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QWidget>

#include <QtCore/QString>

#include <string>
#include <unordered_map>

class QAction;
class QCheckBox;
class QComboBox;
class QLineEdit;
class QMenu;
class QPushButton;
class QSortFilterProxyModel;
class QToolButton;
class QTreeView;

static const char* kDefaultShortcutPropertyName = "defaultShortcut";
static const char* kIdPropertyName = "id";
static const std::string kDomainName = "lpapp";

// List of hotkey actions for all categories
using CategoryHotkeysMap = std::map<QString, std::vector<QAction*>>;

// List of categories for all contexts
using HotkeysMap = std::map<QString, CategoryHotkeysMap>;

using HotkeyEditorExpandState = std::unordered_map<std::string, bool>;

enum class Column : uint8_t {
  Name,
  Hotkey,
  DefaultHotkey
};

struct SearchToolButtonState
{
  QString _actionGroupName;
  QString _matchGroupName;
  bool _allContexts;
  bool _defaultHotkeyChecked;
  bool _nonDefaultHotkeyChecked;
  std::map<std::string, bool> _contextActionsState;
};

class HotkeyEditorModelItem
{
public:
    explicit HotkeyEditorModelItem(const std::vector<QVariant>& data,
                                   const QString& id,
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
    const QString& id() const;
    QAction* action() const;

private:
    std::vector<HotkeyEditorModelItem*> m_childItems;
    std::vector<QVariant> m_itemData;
    HotkeyEditorModelItem *m_parentItem;
    QString m_id;
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

  void setHotkeys(const HotkeysMap& hotkeys);
  HotkeysMap getHotkeys() const;

private:
  void setupModelData(HotkeyEditorModelItem* parent);

  HotkeyEditorModelItem* rootItem;
  HotkeysMap _hotkeys;
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

  void setHotkeys(const HotkeysMap& hotkeys);
  HotkeysMap getHotkeys() const;

Q_SIGNALS:
  void hotkeysChanged();

private:
  HotkeyEditorDelegate* _delegate;
  HotkeyEditorModel* _model;
  QSortFilterProxyModel* _filterModel;
  QTreeView* _view;
};

#endif


