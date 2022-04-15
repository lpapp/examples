#include <QApplication>
#include <QKeySequenceEdit>

#include <shiboken2/autodecref.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QKeySequenceEdit keySequenceEdit;
    keySequenceEdit.show();
    return app.exec();
}
