#ifndef HOTKEYEDITORWIDGET_H
#define HOTKEYEDITORWIDGET_H

#include <QtWidgets/QKeySequenceEdit>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QWidget>

#include <QtCore/QEvent>

#include <string>

class QAction;
class QLineEdit;
class QPushButton;
class QSortFilterProxyModel;
class QTreeView;

// Key: category name
// Value: List of actions (each action having a name, hotkey, description,
//        default hotkey value.
using HotkeyEntry = std::pair<QString, std::vector<std::tuple<QString, QAction*, QString, QString>>>;

enum class Column : uint8_t {
  Name,
  Hotkey,
  Description,
  DefaultHotkey
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

  void setHotkeys(const std::vector<HotkeyEntry>& hotkeys);
  std::vector<HotkeyEntry> getHotkeys() const;

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

class KeyboardWidget : public QWidget
{
  Q_OBJECT

public:
  explicit KeyboardWidget(QWidget *parent = Q_NULLPTR);

Q_SIGNALS:
    void specialKeyClicked(int key);
    void keyClicked(const QString &text);

private Q_SLOTS:
    void buttonClicked(int key);
};

class KeySequenceEdit : public QKeySequenceEdit
{
  Q_OBJECT
  public:
    KeySequenceEdit(QWidget* parent) : QKeySequenceEdit(parent) {}

    bool event(QEvent* event) override {
      if (event->type() == QEvent::ShortcutOverride) {
        return true;
      }

      return QWidget::event(event);
    }
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
  std::vector<HotkeyEntry> getHotkeys() const;

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
  QSortFilterProxyModel* _filterModel;
  QLineEdit* _search;
  QTreeView* _view;
  QPushButton* _resetAllButton;
  QPushButton* _resetButton;
  QPushButton* _importButton;
  QPushButton* _exportButton;
};

#endif


