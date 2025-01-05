#pragma once

#include "Base.h"

#include <string>

namespace utl::Ansi {

  const auto Header = "\x1b[";
  const auto Reset = "\x1b[0m";
  const auto BgBlue = "\x1b[44m";
  const auto Red = "\x1b[31m";
  const auto Green = "\x1b[32m";
  const auto Yellow = "\x1b[33m";
  const auto White = "\x1b[37m";
  const auto Bold = "\x1b[1m";
  const auto Dim = "\x1b[2m";

  inline string red(const string& s) {
    return Red + s + Reset;
  }

  inline string green(const string& s) {
    return Green + s + Reset;
  }

  inline string yellow(const string& s) {
    return Yellow + s + Reset;
  }

  inline string white(const string& s) {
    return White + s + Reset;
  }

  inline string bold(const string& s) {
    return Bold + s + Reset;
  }

  inline string dim(const string& s) {
    return Dim + s + Reset;
  }

  inline string up(int n) {
    return Header + std::to_string(n) + "A";
  }

} // namespace utl::Ansi