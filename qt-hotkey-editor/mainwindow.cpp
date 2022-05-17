#include <QtWidgets>

#include "mainwindow.h"

#include "hotkeyEditorWidget.h"

#include <vector>

MainWindow::MainWindow()
{
    HotkeyEditorWidget *widget = new HotkeyEditorWidget;
    setCentralWidget(widget);

    createActions();
    createMenus();

    QList<QAction*> actions = findChildren<QAction*>();
    std::vector<QAction*> _actions;
    for (QAction* action : actions) {
      action->setProperty(kDefaultShortcutPropertyName, QVariant::fromValue(action->shortcut()));
      CategoryHotkeysMap categoryHotkeys;
      _actions.push_back(action);
    }

    CategoryHotkeysMap categoryHotkeys;
    categoryHotkeys.insert({QString("Category1"), _actions});
    HotkeysMap hotkeys;
    hotkeys.insert({QString("Context1"), categoryHotkeys});
    hotkeys.insert({QString("Context2"), categoryHotkeys});
    hotkeys.insert({QString("Context3"), categoryHotkeys});

    /* hotkeys.insert({
    "Global", // context key
    {
      {"File", {
        {newAct->text(), newAct, newAct->toolTip(), newAct->shortcut().toString(QKeySequence::NativeText)},
        {openAct->text(), openAct, openAct->toolTip(), openAct->shortcut().toString(QKeySequence::NativeText)},
        {saveAct->text(), saveAct, saveAct->toolTip(), saveAct->shortcut().toString(QKeySequence::NativeText)},
        {printAct->text(), printAct, printAct->toolTip(), printAct->shortcut().toString(QKeySequence::NativeText)},
        {exitAct->text(), exitAct, exitAct->toolTip(), exitAct->shortcut().toString(QKeySequence::NativeText)}}
      },

      {"Edit", {
          {undoAct->text(), undoAct, undoAct->toolTip(), undoAct->shortcut().toString(QKeySequence::NativeText)},
        {redoAct->text(), redoAct, redoAct->toolTip(), redoAct->shortcut().toString(QKeySequence::NativeText)},
        {cutAct->text(), cutAct, cutAct->toolTip(), cutAct->shortcut().toString(QKeySequence::NativeText)},
        {copyAct->text(), copyAct, copyAct->toolTip(), copyAct->shortcut().toString(QKeySequence::NativeText)},
        {pasteAct->text(), pasteAct, pasteAct->toolTip(), pasteAct->shortcut().toString(QKeySequence::NativeText)}}
      }
    }
    }); */

    widget->setHotkeys(hotkeys);

    setWindowTitle(tr("Menus"));
    setMinimumSize(160, 160);
    resize(1920, 1280);
}

#ifndef QT_NO_CONTEXTMENU
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(cutAct);
    menu.addAction(copyAct);
    menu.addAction(pasteAct);
    menu.exec(event->globalPos());
}
#endif // QT_NO_CONTEXTMENU

void MainWindow::createActions()
{
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
