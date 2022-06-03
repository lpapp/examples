#include <QApplication>

#include "pushbutton.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    PushButton pushButton("Drag me");
    pushButton.show();
    return app.exec();
}
