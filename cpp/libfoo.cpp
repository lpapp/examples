#include "libfoo.h"

namespace foo {
  std::ostream& operator <<(std::ostream& o, const TimeValueSet& times) {
    o << "[";
    for (const TimeValue& time : times) {
      o << " " << time;
    }
    o << " ]";
    return o;
  }
}
