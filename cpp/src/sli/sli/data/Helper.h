#pragma once

#include "sli/data/Cell.h"
#include "utl/Range.h"
#include "utl/Utils.h"
#include "utl/Point.h"

namespace sli {
  struct Board;
}

namespace sli::Helper {

  using namespace sli;
  using enum CellType;

  inline CellType cellTypeAt(int x, int y) {
    // performance critical
    // NOTE: dependent on CellType order
    return static_cast<CellType>((x & 1) + (y & 1) * 2);
  }

  typedef std::pair<Range<int>, Range<int>> RangePair;

  RangePair cellRangeFor(Board& b, CellType cellType, bool skipOutside);

  inline int numberIndex(int x, int y, int w) {
    return (y / 2) * (w / 2 + 1) + (x / 2);
  }

  inline int lineIndex(int x, int y, int w) {
    return (y * w + x) / 2;
  }

  ///
  /// 指定の角に進入する線の相対位置を求める計算
  ///
  ///   |v1
  /// +-+-v2
  /// |0|  v=NEの場合
  /// +-+
  ///
  inline std::pair<Point, Point> vectorsToCornerLines(const Point& v) {
    return { { v.x, v.y * 2 }, { v.x * 2, v.y } };
  }

  ///
  /// 指定の角に進入する線の位置を求める計算
  ///
  ///   |p1
  /// +-+-p2
  /// |p|  v=NEの場合
  /// +-+
  ///
  inline std::pair<Point, Point> pointsAtCornerLines(const Point& p, const Point& v) {
    auto [v1, v2] = vectorsToCornerLines(v);
    return { p.add(v1), p.add(v2) };
  }

} // namespace sli::Helper