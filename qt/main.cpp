#include <QApplication>
#include <QKeySequenceEdit>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QKeySequenceEdit keySequenceEdit;
    keySequenceEdit.show();
    return app.exec();
}
