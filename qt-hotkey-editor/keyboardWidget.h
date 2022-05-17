#ifndef KEYBOARDWIDGET_H
#define KEYBOARDWIDGET_H

#include <QtWidgets/QWidget>

#include <QtGui/QKeySequence>

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
private:
  std::map<QString, QPushButton*> _buttons;
};

#endif


