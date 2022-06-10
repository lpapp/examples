#include <QAction>
#include <QApplication>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QShortcut>

#include <iostream>

class MainWindow : public QMainWindow
{
  public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
      QAction* exitAct = new QAction(tr("Exit"), this);
      exitAct->setShortcut(QKeySequence(Qt::Key_Right));
      connect(exitAct, &QAction::triggered, this, &QWidget::close);
      QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
      fileMenu->addAction(exitAct);
    }

    void keyPressEvent(QKeyEvent *event) override {
      std::cout << std::hex << event->modifiers() << std::endl;
      QMainWindow::keyPressEvent(event);
    }
};

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  MainWindow window;

  QShortcut keypadHome(QKeySequence("Num+7"), &window);
  QObject::connect(&keypadHome, &QShortcut::activated, []() {
    std::cout << "TEST KEYPAD HOME" << std::endl;
  });

  QShortcut home(QKeySequence("7"), &window);
  QObject::connect(&home, &QShortcut::activated, []() {
    std::cout << "TEST HOME" << std::endl;
  });

  window.show();
  return app.exec();
}
