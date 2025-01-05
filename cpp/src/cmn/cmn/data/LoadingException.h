#pragma once

#include "Base.h"
#include "utl/Exception.h"

namespace cmn {

  struct LoadingException : public Exception {
    using Exception::Exception;
  };

} // namespace cmn