#pragma once

#include "Base.h"
#include "utl/Exception.h"

namespace cmn {

  struct HatanException : public Exception {
    using Exception::Exception;
  };

} // namespace cmn