#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QPushButton;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

private:
  QPushButton *m_leftPushButton;
  QPushButton *m_rightPushButton;
};

#endif
