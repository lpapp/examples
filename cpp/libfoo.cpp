#include "libfoo.h"

namespace foo {
  std::ostream& operator <<(std::ostream& o, const FooSet& fooSet) {
    o << "[";
    for (const Foo& foo : fooSet) {
      o << " " << foo;
    }
    o << " ]";
    return o;
  }
}
