#pragma once

#include "Base.h"

#include <ranges>
#include <numeric>
#include <sstream>
#include <cctype>
#include <algorithm> 
#include <locale>

namespace utl::StrUtils {

  inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](auto ch) {
      return !std::isspace(ch);
      }));
  }

  inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](auto ch) {
      return !std::isspace(ch);
      }).base(), s.end());
  }

  inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
  }

  template <typename T>
  inline constexpr string join(const T& iterable, const string& separator) {
    return std::accumulate(
      std::next(std::cbegin(iterable)),
      std::cend(iterable),
      *std::cbegin(iterable),
      [&separator](const string& a, const string& b) {
        return a + separator + b;
      });
  }

  inline vector<string> split(const string& s) {
    vector<string> tokens;
    std::istringstream tokenStream(s);
    while (!tokenStream.eof()) {
      string token;
      tokenStream >> token;
      tokens.push_back(token);
    }
    return tokens;
  }

  inline string alignRight(const string& s, int width, char c = ' ') {
    return string(std::max(int(width - s.length()), 0), c) + s;
  }
} // namespace utl::StrUtils