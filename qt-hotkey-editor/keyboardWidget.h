#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QtWidgets/QWidget>

#include <QtGui/QKeySequence>

#include <QtCore/QString>

#include <map>
#include <vector>

class QColor;
class QPushButton;

class KeyboardWidget : public QWidget
{
	Q_OBJECT

public:
	KeyboardWidget(QWidget *parent = nullptr);
  void setHotkeys(const std::vector<QKeySequence>& hotkeys, const QColor& color);

  void resizeEvent(QResizeEvent *event) override;

private:
  void resizeButtons();

  std::map<QString, QPushButton*> _buttonsMap;
  std::vector<std::vector<QPushButton*>> _buttons;
};

#endif


