#include <QtWidgets/QApplication>

#include <QtGui/QPalette>

#include "mainwindow.h"

void setApplication(QApplication& application)
{
    QPalette palette(QColor(50, 50, 50));
    palette.setBrush(QPalette::Button, QColor(50, 50, 50));
    palette.setBrush(QPalette::Base, QColor(70, 70, 70));
    palette.setBrush(QPalette::Highlight, QColor(76, 76, 76));

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
    QApplication application(argc, argv);
    setApplication(application);
    application.setAttribute(Qt::AA_MacDontSwapCtrlAndMeta);
    MainWindow window;
    window.show();
    return application.exec();
}
