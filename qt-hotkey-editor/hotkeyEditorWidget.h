#ifndef OTKEYEDITORWIDGET_H
#define OTKEYEDITORWIDGET_H

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QWidget>

#include <string>
#include <tuple>
#include <utility>

class QAction;
class QLineEdit;
class QPushButton;
class QTreeView;

using HotkeyEntry = std::pair<QString, std::vector<std::tuple<QString, QAction*, QString>>>;

enum class Column : uint8_t {
  Name,
  Hotkey,
  Description,
};

class HotkeyEditorModelItem
{
public:
    explicit HotkeyEditorModelItem(const std::vector<QVariant> &data, HotkeyEditorModelItem *parentItem = nullptr);
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

  QVariant data(const QModelIndex& index, int role) const override;
  Qt::ItemFlags flags(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  int rowCount(const QModelIndex& index = QModelIndex()) const override;
  int columnCount(const QModelIndex& index = QModelIndex()) const override;

  bool setData(const QModelIndex& index, const QVariant& value, int role) override;

  void resetAll();
  QModelIndex reset(const QModelIndexList& selectedItems);

  void setHoverTooltipText(const QString& hoverTooltipText);
  const QString& hoverTooltipText();

  void setHotkeys(const std::vector<HotkeyEntry>& hotkeys);

private:
  void setupModelData(HotkeyEditorModelItem* parent);

  HotkeyEditorModelItem* rootItem;
  std::vector<HotkeyEntry> _hotkeys;
  QString _hoverTooltip;
};

class HotkeyEditorDelegate : public QStyledItemDelegate
{
  Q_OBJECT

public:
  HotkeyEditorDelegate(QObject* parent = nullptr);
  QWidget *createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                        const QModelIndex& index) const override;

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

  void setHotkeys(const std::vector<HotkeyEntry>& hotkeys);

public Q_SLOTS:
  void resetAll();
  void reset();

  void importHotkeys();
  void exportHotkeys();

  void selectionChanged();

Q_SIGNALS:
  void hotkeysChanged();

private:

  HotkeyEditorDelegate* _delegate;
  HotkeyEditorModel* _model;
  QLineEdit* _search;
  QTreeView* _view;
  QPushButton* _resetAllButton;
  QPushButton* _resetButton;
  QPushButton* _importButton;
  QPushButton* _exportButton;
};

#endif


