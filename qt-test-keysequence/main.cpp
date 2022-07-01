#include <QKeySequence>
#include <QTest>

int main()
{
  return QTest::toString(QKeySequence()) == nullptr;
}
