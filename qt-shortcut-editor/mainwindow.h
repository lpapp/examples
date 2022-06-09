#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>

class QAction;
class QActionGroup;
class QLabel;
class QMenu;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow();

private Q_SLOTS:
  void showPreferences();

private:
  void createActions();
  void createMenus();

  QMenu* fileMenu;
  QMenu* editMenu;
  QMenu* formatMenu;
  QMenu* helpMenu;
  QActionGroup* alignmentGroup;
  QAction* preferencesAct;
  QAction* newAct;
  QAction* openAct;
  QAction* saveAct;
  QAction* printAct;
  QAction* exitAct;
  QAction* undoAct;
  QAction* redoAct;
  QAction* cutAct;
  QAction* copyAct;
  QAction* pasteAct;
  QAction* boldAct;
  QAction* italicAct;
  QAction* leftAlignAct;
  QAction* rightAlignAct;
  QAction* justifyAct;
  QAction* centerAct;
  QAction* setLineSpacingAct;
  QAction* setParagraphSpacingAct;
  QAction* aboutAct;
  QAction* aboutQtAct;
};

#endif
