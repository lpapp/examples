#include <QApplication>
#include <QHBoxLayout>
#include <QPushButton>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QWidget mainWindow;
  QHBoxLayout* layout = new QHBoxLayout;
  QPushButton *restoreButton = new QPushButton("Restore");
  restoreButton->setFocusPolicy(Qt::StrongFocus);
  layout->addWidget(restoreButton);
  QPushButton *okButton = new QPushButton("Ok");
  okButton->setFocusPolicy(Qt::TabFocus);
  layout->addWidget(okButton);
  QPushButton *cancelButton = new QPushButton("Cancel");
  cancelButton->setFocusPolicy(Qt::StrongFocus);
  layout->addWidget(cancelButton);
  mainWindow.setLayout(layout);
  mainWindow.show();
  return app.exec();
}
