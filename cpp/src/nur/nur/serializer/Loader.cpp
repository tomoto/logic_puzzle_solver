#include "Loader.h"
#include "cmn/data/LoadingException.h"
#include "utl/StrUtils.h"

#include <algorithm>

namespace nur {

  static void check(bool condition, const string& message) {
    if (!condition) {
      throw cmn::LoadingException(message);
    }
  }

  Board Loader::load(vector<string> lines) {
    int h = lines.size();
    check(h > 0, "The height of the board should be positive");
    for (auto& s : lines) {
      StrUtils::trim(s);
    }
    int w = StrUtils::split(lines[0]).size();
    check(w > 0, "The width of the board should be positive");

    Board b(w, h);
    for (int y : range(0, h)) {
      auto fields = StrUtils::split(lines[y]);
      auto numbers = transform(fields, [](const string& s) { return std::stoi(s); });
      check(numbers.size() == w, "The width of the board should be consistent");
      for (int x : range(0, w)) {
        auto n = numbers[x];
        if (n > 0) {
          b.createShima({ x, y }, n);
        }
      }
    }

    return std::move(b);
  }
}