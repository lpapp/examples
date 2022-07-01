#include <QKeySequence>
#include <QTest>

#include <iostream>

int main()
{
  std::cout << QTest::toString(QKeySequence()) << std::endl;
  return 0;
}
