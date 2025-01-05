#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define DELETE_COPY_CTORS(classname) \
  classname(const classname&) = delete; \
  classname& operator=(const classname&) = delete; \
  classname(classname&&) = delete; \
  classname& operator=(classname&&) = delete;

#define DEFAULT_COPY_CTORS(classname) \
  classname(const classname&) = default; \
  classname& operator=(const classname&) = default;

#define DEFAULT_MOVE_CTORS(classname) \
  classname(classname&&) = default; \
  classname& operator=(classname&&) = default;

namespace utl {

  using std::string;
  using std::vector;

} // namespace utl