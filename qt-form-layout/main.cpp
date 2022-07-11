#include <QApplication>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  QGroupBox *formGroupBox = new QGroupBox("Form layout");
  QFormLayout *layout = new QFormLayout;
  layout->addRow("Line 1:", new QLineEdit);
  QFrame* divider = new QFrame();
  divider->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  divider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
  layout->addRow("Line 2, long text:", divider);
  layout->addRow("Line 3:", new QSpinBox);
  formGroupBox->setLayout(layout);
  formGroupBox->show();

  return app.exec();
}
