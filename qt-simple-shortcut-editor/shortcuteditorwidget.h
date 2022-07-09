#ifndef SHORTCUTEDITORWIDGET_H
#define SHORTCUTEDITORWIDGET_H

#include <QString>
#include <QStyledItemDelegate>
#include <QWidget>

#include <string>
#include <unordered_map>

class QAction;
class QHBoxLayout;
class QItemSelection;
class QTreeView;

// List of actions for all categories
using CategoryActionsMap = std::map<QString, std::vector<QAction*>>;

// List of categories for all contexts
using ActionsMap = std::map<QString, CategoryActionsMap>;

using ShortcutEditorExpandState = std::unordered_map<std::string, bool>;

enum class Column : uint8_t {
  Name,
  Shortcut
};

class ShortcutEditorModelItem
{
public:
    explicit ShortcutEditorModelItem(const std::vector<QVariant>& data,
                                     ShortcutEditorModelItem* parentItem = nullptr);
    ~ShortcutEditorModelItem();

    void appendChild(ShortcutEditorModelItem* child);

    ShortcutEditorModelItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    ShortcutEditorModelItem* parentItem();
    QAction* action() const;

private:
    std::vector<ShortcutEditorModelItem*> _childItems;
    std::vector<QVariant> _itemData;
    ShortcutEditorModelItem* _parentItem;
};

class ShortcutEditorModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit ShortcutEditorModel(QObject* parent = nullptr);
  ~ShortcutEditorModel() override;

  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  int rowCount(const QModelIndex& index = QModelIndex()) const override;
  int columnCount(const QModelIndex& index = QModelIndex()) const override;

  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

  void setActions();

private:
  void setupModelData(ShortcutEditorModelItem* parent);

  ShortcutEditorModelItem* rootItem;
  ActionsMap _actionsMap;
};

class ShortcutEditorDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  ShortcutEditorDelegate(QObject* parent = nullptr);

protected:
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override;

  void setEditorData(QWidget* editor, const QModelIndex& index) const override;
  void setModelData(QWidget* editor, QAbstractItemModel* model,
                    const QModelIndex& index) const override;

  void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                            const QModelIndex& index) const override;

private Q_SLOTS:
  void commitAndCloseEditor();
};

class ShortcutEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ShortcutEditorWidget(QWidget* parent = nullptr);
  ~ShortcutEditorWidget() override = default;

  void setActions();

private:

  void createLayout();
  void createTreeView();

  ShortcutEditorDelegate* _delegate;
  ShortcutEditorModel* _model;
  QTreeView* _view;
};

#endif
