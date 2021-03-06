#ifndef PUSHBUTTON_H
#define PUSHBUTTON_H

#include <QPoint>
#include <QPushButton>
#include <QWidget>

class QString;

class PushButton : public QPushButton
{
  Q_OBJECT

public:
  PushButton(const QString& text, QWidget* parent = nullptr);

private:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;

  QPoint dragStartPosition;
};

#endif


