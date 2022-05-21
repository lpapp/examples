#ifndef LIBFOO_H
#define LIBFOO_H

#include <iostream>
#include <set>

namespace foo {
  using TimeValue = double;
  using TimeValueSet = std::set<TimeValue>;

  std::ostream& operator<<(std::ostream& o, const TimeValueSet&);
}

#endif
