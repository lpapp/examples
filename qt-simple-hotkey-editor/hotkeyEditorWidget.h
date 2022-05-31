#ifndef HOTKEYEDITORWIDGET_H
#define HOTKEYEDITORWIDGET_H

#include <QtWidgets/QWidget>

#include <QtCore/QAbstractItemModel>
#include <QtCore/QString>

#include <string>

class QAction;
class QModelIndex;
class QSortFilterProxyModel;
class QTreeView;

static const char* kDefaultShortcutPropertyName = "defaultShortcut";
static const char* kIdPropertyName = "id";
static const std::string kDomainName = "lpapp";

using CategoryHotkeysMap = std::map<QString, std::vector<QAction*>>;
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
    QAction* action() const;

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

  void setHotkeys(const HotkeysMap& hotkeys);

private:
  void setupModelData(HotkeyEditorModelItem* parent);

  HotkeyEditorModelItem* rootItem;
  HotkeysMap _hotkeys;
};

class HotkeyEditorWidget : public QWidget
{
  Q_OBJECT

public:
  HotkeyEditorWidget(QWidget* parent = nullptr);
  ~HotkeyEditorWidget() override = default;

  void setHotkeys(const HotkeysMap& hotkeys);

private:
  HotkeyEditorModel* _model;
  QSortFilterProxyModel* _filterModel;
  QTreeView* _view;
};

#endif


