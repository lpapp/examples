#include "mainwindow.h"

#include "window.h"

#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>

MainWindow::MainWindow()
{
  QMenuBar *menuBar = new QMenuBar;
  QMenu *menuWindow = menuBar->addMenu(tr("&Window"));
  QAction *addNew = new QAction(menuWindow);
  addNew->setText(tr("Add new"));
  menuWindow->addAction(addNew);
  connect(addNew, &QAction::triggered, this, &MainWindow::onAddNew);
  setMenuBar(menuBar);

  onAddNew();
}

void MainWindow::onAddNew()
{
  if (!centralWidget()) {
    setCentralWidget(new Window(this));
  }
  else {
    QMessageBox::information(nullptr, tr("Cannot add new window"),
                             tr("Already occupied. Undock first."));
  }
}
