#include "window.h"

#include "borderLayout.h"

#include <QLabel>
#include <QTextBrowser>

Window::Window()
{
  QTextBrowser *centralWidget = new QTextBrowser;
  centralWidget->setPlainText(tr("Central widget"));

  BorderLayout *layout = new BorderLayout;
  layout->addWidget(centralWidget, BorderLayout::Center);
  layout->addWidget(createLabel("North"), BorderLayout::North);
  layout->addWidget(createLabel("West"), BorderLayout::West);
  layout->addWidget(createLabel("East 1"), BorderLayout::East);
  layout->addWidget(createLabel("East 2") , BorderLayout::East);
  layout->addWidget(createLabel("South"), BorderLayout::South);
  setLayout(layout);
}

QLabel *Window::createLabel(const QString &text)
{
  QLabel *label = new QLabel(text);
  label->setFrameStyle(QFrame::Box | QFrame::Raised);
  return label;
}
