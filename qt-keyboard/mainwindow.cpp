#include "mainwindow.h"

#include "keyboardWidget.h"

MainWindow::MainWindow()
{
    KeyboardWidget *widget = new KeyboardWidget;
    setCentralWidget(widget);
    setMinimumSize(160, 160);
    resize(1920, 1280);
}

