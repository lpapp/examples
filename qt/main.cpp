#include <QApplication>
#include <QKeySequenceEdit>

#include <iostream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QKeySequenceEdit keySequenceEdit;
    QKeySequence keySequence("Shift+6");
    for (auto k : QKeySequence::listFromString(keySequence.toString())) {
      std::cout << "TEST: " << k.toString().toStdString() << std::endl;
    }
    keySequenceEdit.show();
    return app.exec();
}
