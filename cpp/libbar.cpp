#include <iostream>

#include "libfoo.h"

namespace bar {
  using foo::operator<<;
  void method() {
    const foo::FooSet fs{0.4, 1.2};
    std::cout << fs << std::endl;
  }
}
