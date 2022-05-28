#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>

class QSlider;
class QPushButton;

class GLWidget;
class MainWindow;

class Window : public QWidget
{
  Q_OBJECT

public:
  Window(MainWindow *mw);

protected:
  void keyPressEvent(QKeyEvent *event) override;

private:
  QSlider *createSlider();

  GLWidget *glWidget;
  QSlider *xSlider;
  QSlider *ySlider;
  QSlider *zSlider;
  MainWindow *mainWindow;
};

#endif
