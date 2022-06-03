#include "pushbutton.h"

#include <QtWidgets/QApplication>

#include <QtGui/QDrag>
#include <QtGui/QMouseEvent>

#include <QtCore/QMimeData>
#include <QtCore/QString>

#include <iostream>

PushButton::PushButton(const QString& text, QWidget* parent)
  : QPushButton(text, parent)
{
}

void PushButton::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    dragStartPosition = event->pos();
  }
  QPushButton::mousePressEvent(event);
}

void PushButton::mouseMoveEvent(QMouseEvent* event)
{
  if (!(event->buttons() & Qt::LeftButton)) {
    return;
  }

  if ((event->pos() - dragStartPosition).manhattanLength()
       < QApplication::startDragDistance()) {
    return;
  }

  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;
  mimeData->setText(text());
  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction);
  // setDown(false);
}

void PushButton::mouseReleaseEvent(QMouseEvent* event)
{
  std::cout << "TEST MOUSE RELEASE EVENT" << std::endl;
  QPushButton::mouseReleaseEvent(event);
}
