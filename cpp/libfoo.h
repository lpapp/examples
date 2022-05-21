#ifndef LIBFOO_H
#define LIBFOO_H

#include <iostream>
#include <set>

namespace foo {
  using Foo = double;
  using FooSet = std::set<Foo>;

  std::ostream& operator<<(std::ostream& o, const FooSet& fooSet);
}

#endif
