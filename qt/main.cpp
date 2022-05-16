#include <QApplication>
#include <QLineEdit>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QLineEdit lineEdit;
    QPushButton pushButton;
    QObject::connect(&lineEdit, &QLineEdit::returnPressed, [&pushButton]() {emit pushButton.clicked(true);});
    return app.exec();
}
