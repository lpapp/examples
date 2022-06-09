#include <QApplication>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMainWindow>
#include <QShortcut>

#include <iostream>

class MainWindow : public QMainWindow
{
  public:
    void keyPressEvent(QKeyEvent *event) override {
      std::cout << std::hex << event->modifiers() << std::endl;
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
