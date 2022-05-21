#include <iostream>

#include "libfoo.h"

namespace bar {
  using foo::operator<<;
  void method() {
    foo::TimeValueSet tvs{0.4, 1.2};
    std::cout << tvs << std::endl;
  }
}
