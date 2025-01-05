#pragma once

#include "cmn/data/Base.h"
#include "utl/Point.h"
#include "utl/Array2D.h"
#include "TrackTopology.h"

#include <numeric>
#include <concepts>

namespace cmn::SegmentAnalyzer {

  template <typename S>
  concept Subject = requires (S & s, int x, int y, Point d) {
    { s.width } -> std::convertible_to<int>;
    { s.height } -> std::convertible_to<int>;
    { s.connected(x, y, d) } ->std::convertible_to<bool>;
    { s.belongsToSegment(x, y) } -> std::convertible_to<bool>;
  };

  struct Result {
    Array2D<int> segmentIds;
    int maxSegmentId = 0;

    Result(int width, int height) : segmentIds(width, height, 0) {  }
  };

  template<Subject S>
  inline void _expandSegment(S&& g, int x, int y, int& segmentId, Result& result) {
    bool northConnected = y > 0 && g.connected(x, y, Directions::N);
    bool westConnected = x > 0 && g.connected(x, y, Directions::W);
    int northId = northConnected ? result.segmentIds.at(x, y - 1) : 0;
    int westId = westConnected ? result.segmentIds.at(x - 1, y) : 0;

    int n;
    if (northConnected) {
      n = northId;
      if (westConnected && northId != westId) {
        auto p = result.segmentIds.data.begin();
        int n = x + y * g.width;
        while (n-- > 0) {
          if (*p == westId) {
            *p = northId;
          }
          p++;
        }
      }
    } else {
      if (westConnected) {
        n = westId;
      } else {
        n = ++segmentId;
      }
    }
    result.segmentIds.at(x, y) = n;
  }

  template <Subject S>
  Result analyze4Way(S&& g) {
    Result result(g.width, g.height);
    int segmentId = 0;
    for (int y : range(0, g.height)) {
      for (int x : range(0, g.width)) {
        if (g.belongsToSegment(x, y)) {
          _expandSegment(g, x, y, segmentId, result);
        }
      }
    }
    result.maxSegmentId = segmentId;
    return std::move(result);
  }

} // namespace cmn::SegmentAnalyzer

