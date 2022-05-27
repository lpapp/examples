#include <QApplication>

#include "mainwindow.h"

void setApplication(QApplication& application)
{
    QPalette palette(QColor(50, 50, 50));
    palette.setBrush(QPalette::Button, QColor(77, 77, 77));
    palette.setBrush(QPalette::Base, QColor(58, 58, 58));
    palette.setBrush(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setBrush(QPalette::Highlight, QColor(0xf7, 0x93, 0x1e));
    palette.setBrush(QPalette::HighlightedText, QColor(0x1e, 0x1e, 0x1e));

    QColor textColor(200, 200, 200);
    palette.setBrush(QPalette::Text, textColor);
    palette.setBrush(QPalette::WindowText, textColor);
    palette.setBrush(QPalette::BrightText, textColor);
    palette.setBrush(QPalette::ButtonText, textColor);
    palette.setBrush(QPalette::Active, QPalette::Text, textColor);
    palette.setBrush(QPalette::Active, QPalette::WindowText, textColor);
    palette.setBrush(QPalette::Active, QPalette::BrightText, textColor);
    palette.setBrush(QPalette::Active, QPalette::ButtonText, textColor);

    application.setPalette(palette);
    application.setStyle("Fusion");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    setApplication(app);
    MainWindow window;
    window.show();
    return app.exec();
}
