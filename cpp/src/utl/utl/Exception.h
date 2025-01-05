#pragma once

#include "Base.h"

#include <exception>
#include <string>

namespace utl {

  struct Exception : public std::exception {
    const string message;

    Exception(const string& message) : message(message) {}

    virtual const char* what() const noexcept override {
      return message.c_str();
    }
  };

} // namespace utl
