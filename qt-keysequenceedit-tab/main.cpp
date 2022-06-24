#include <QApplication>
#include <QKeySequenceEdit>
#include <QPushButton>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QWidget mainWindow;

  QVBoxLayout* layout = new QVBoxLayout;

  QPushButton *okButton = new QPushButton("Ok");
  layout->addWidget(okButton);

  QKeySequenceEdit* keySequenceEdit = new QKeySequenceEdit();
  layout->addWidget(keySequenceEdit);

  QPushButton *cancelButton = new QPushButton("Cancel");
  layout->addWidget(cancelButton);

  mainWindow.setLayout(layout);
  mainWindow.show();
  return app.exec();
}
