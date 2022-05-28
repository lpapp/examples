#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

#include <QtGui/QKeySequence>

#include <QtCore/QPoint>
#include <QtCore/QString>

#include <map>
#include <vector>

class QAction;
class QColor;
class QPushButton;

class KeyButton : QPushButton
{
public:
  KeyButton(const QString& text, QWidget* parent);

private:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
  void dragEnterEvent(QDragEnterEvent *event) override;
  void dragMoveEvent(QDragMoveEvent *event) override;
  void dropEvent(QDropEvent *event) override;

  QPoint dragStartPosition;
};

class KeyboardWidget : public QWidget
{
	Q_OBJECT

public:
	KeyboardWidget(QWidget *parent = nullptr);
  void setButtonColor(const QColor& color);
  void setActions(const std::vector<QAction*> hotkeys);

public Q_SLOTS:
  void highlightHotkeys();

private:
  void dropEvent(QDropEvent *event) override;
  void resizeEvent(QResizeEvent *event) override;

  void resizeButtons();
  void resetHighlights();

  std::map<int, QPushButton*> _buttonsMap;
  std::vector<std::vector<QPushButton*>> _buttons;

  Qt::KeyboardModifiers _modifiers;
  QColor _color;
  std::vector<QAction*> _actions;
};

#endif


