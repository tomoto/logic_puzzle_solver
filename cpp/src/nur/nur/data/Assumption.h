#pragma once

#include "nur/data/Base.h"
#include "utl/Point.h"

namespace nur {
  struct Assumption {
    Point p;
    bool kabe; // true if kabe, false if shima

    auto operator<=>(const Assumption&) const = default;
  };
} // namespace nur
