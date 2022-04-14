#include <QApplication>
#include <QKeySequenceEdit>

#include "QtWidgets/pyside2_qtwidgets_python.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QKeySequenceEdit keySequenceEdit;
    keySequenceEdit.show();
    return app.exec();
}
