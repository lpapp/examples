#include <QApplication>
#include <QKeySequenceEdit>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QKeySequenceEdit keySequenceEdit;
    keySequenceEdit.show();
    return app.exec();
}
