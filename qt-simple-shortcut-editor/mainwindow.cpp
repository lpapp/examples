#include "mainwindow.h"

#include "shortcuteditorwidget.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow()
{
  QVBoxLayout *vBoxLayout = new QVBoxLayout;
  QHBoxLayout *hBoxLayout = new QHBoxLayout;
  m_leftPushButton = new QPushButton("Left");
  m_rightPushButton = new QPushButton("Right");
  hBoxLayout->addWidget(m_leftPushButton);
  hBoxLayout->addWidget(m_rightPushButton);
  vBoxLayout->addLayout(hBoxLayout);
  vBoxLayout->addWidget(new ShortcutEditorWidget);
  setLayout(vBoxLayout);

  setMinimumSize(960, 640);
  resize(1920, 1280);
}
