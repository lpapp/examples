#include "mainwindow.h"

#include "actionmanager.h"
#include "shortcuteditorwidget.h"

#include <QAction>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow()
{
  QVBoxLayout *vBoxLayout = new QVBoxLayout;
  QHBoxLayout *hBoxLayout = new QHBoxLayout;

  m_leftPushButton = new QPushButton("Left");
  m_rightPushButton = new QPushButton("Right");
  for (auto nameShortcut : std::vector<std::vector<const char *>>{{"red", "r", "shift+r"}, {"green", "g", "shift+g"}, {"blue", "b", "shift+b"}}) {
      QAction *action = ActionManager::registerAction(nameShortcut[0], nameShortcut[1], "left", "color");
      m_leftPushButton->addAction(action);
      connect(action, &QAction::triggered, this, [this, nameShortcut]() {
          m_leftPushButton->setText(nameShortcut[0]);
      });

      action = ActionManager::registerAction(nameShortcut[0], nameShortcut[2], "right", "color");
      m_rightPushButton->addAction(action);
      connect(action, &QAction::triggered, this, [this, nameShortcut]() {
          m_rightPushButton->setText(nameShortcut[0]);
      });
  }

  hBoxLayout->addWidget(m_leftPushButton);
  hBoxLayout->addWidget(m_rightPushButton);
  vBoxLayout->addLayout(hBoxLayout);
  vBoxLayout->addWidget(new ShortcutEditorWidget);

  QWidget *centralWidget = new QWidget;
  centralWidget->setLayout(vBoxLayout);
  setCentralWidget(centralWidget);

  setMinimumSize(960, 640);
  resize(1920, 1280);
}
