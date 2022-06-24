#include <QApplication>
#include <QKeySequenceEdit>
#include <QPushButton>
#include <QVBoxLayout>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QWidget mainWindow;

  QVBoxLayout* layout = new QVBoxLayout;

  // If I comment the button out, I can assign tab to keySequenceEdit
  QPushButton *okButton = new QPushButton("Ok");
  layout->addWidget(okButton);

  QKeySequenceEdit* keySequenceEdit = new QKeySequenceEdit();
  layout->addWidget(keySequenceEdit);

  mainWindow.setLayout(layout);
  mainWindow.show();
  return app.exec();
}
