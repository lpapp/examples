#include <iostream>

#include "libfoo.h"

namespace bar {
  void method(const foo::TimeValueSet& tvs) {
    std::cout << tvs << std::endl;
  }
}
