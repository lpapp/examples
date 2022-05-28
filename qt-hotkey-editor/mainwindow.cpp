#include <QtWidgets>

#include "mainwindow.h"

#include "preferencesDialog.h"

#include <vector>

MainWindow::MainWindow()
{
  createActions();
  createMenus();

  setMinimumSize(960, 640);
  // setMinimumSize(160, 160);
  // resize(1920, 1280);
}

void MainWindow::createActions()
{
    preferencesAct = new QAction(tr("Preferences"), this);
    preferencesAct->setShortcuts(QKeySequence::Preferences);
    connect(preferencesAct, &QAction::triggered, this, &MainWindow::showPreferences);

    newAct = new QAction(tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);

    openAct = new QAction(tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);

    saveAct = new QAction(tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);

    printAct = new QAction(tr("&Print..."), this);
    printAct->setShortcuts(QKeySequence::Print);

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);

    cutAct = new QAction(tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);

    copyAct = new QAction(tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);

    pasteAct = new QAction(tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);

    boldAct = new QAction(tr("&Bold"), this);
    boldAct->setShortcut(QKeySequence::Bold);

    QFont boldFont = boldAct->font();
    boldAct->setFont(boldFont);

    italicAct = new QAction(tr("&Italic"), this);
    italicAct->setShortcut(QKeySequence::Italic);

    QFont italicFont = italicAct->font();
    italicFont.setItalic(true);
    italicAct->setFont(italicFont);

    setLineSpacingAct = new QAction(tr("Set &Line Spacing..."), this);

    setParagraphSpacingAct = new QAction(tr("Set &Paragraph Spacing..."), this);

    aboutAct = new QAction(tr("&About"), this);
    aboutQtAct = new QAction(tr("About &Qt"), this);

    leftAlignAct = new QAction(tr("&Left Align"), this);
    leftAlignAct->setShortcut(tr("Ctrl+L"));

    rightAlignAct = new QAction(tr("&Right Align"), this);
    rightAlignAct->setShortcut(tr("Ctrl+R"));

    justifyAct = new QAction(tr("&Justify"), this);
    justifyAct->setShortcut(tr("Ctrl+J"));

    centerAct = new QAction(tr("&Center"), this);
    centerAct->setShortcut(tr("Ctrl+E"));

    alignmentGroup = new QActionGroup(this);
    alignmentGroup->addAction(leftAlignAct);
    alignmentGroup->addAction(rightAlignAct);
    alignmentGroup->addAction(justifyAct);
    alignmentGroup->addAction(centerAct);
    leftAlignAct->setChecked(true);
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(preferencesAct);
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);

    fileMenu->addAction(saveAct);
    fileMenu->addAction(printAct);

    fileMenu->addSeparator();

    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);

    formatMenu = editMenu->addMenu(tr("&Format"));
    formatMenu->addAction(boldAct);
    formatMenu->addAction(italicAct);
    formatMenu->addSeparator()->setText(tr("Alignment"));
    formatMenu->addAction(leftAlignAct);
    formatMenu->addAction(rightAlignAct);
    formatMenu->addAction(justifyAct);
    formatMenu->addAction(centerAct);
    formatMenu->addSeparator();
    formatMenu->addAction(setLineSpacingAct);
    formatMenu->addAction(setParagraphSpacingAct);
}

void MainWindow::showPreferences()
{
  PreferencesDialog* dialog = new PreferencesDialog(this);
  connect(dialog, &QDialog::finished, dialog, &QDialog::deleteLater);
  dialog->show();
  dialog->activateWindow();
}
