#pragma once

#include "CellRule.h"

namespace sli {

  template <bool NoSmallLoop>
  struct ThreePairInitialRule {
    static constexpr CellType cellType = CellType::number;
    static constexpr bool skipOutside = true;
    static constexpr bool skipUnmodifiedCells = false;
    static constexpr bool noSmallLoop = NoSmallLoop;

    ///
    /// 隣接する、あるいは対角に向かい合う3のペアのルール(初回のみ)
    ///
    /// 隣接する3(間の線は「小ループ禁止」の前提が成り立つときのみ引かれる):
    /// + + + +    + +-+ +
    ///    3          3
    /// + + + + => +X+-+X+
    ///    3          3
    /// + + + +    + +-+ +
    /// 
    /// 対角の3:
    /// + + +      +-+ +
    ///  3         |3
    /// + + +   => + + +
    ///    3          3|
    /// + + +      + +-+
    ///
    /// 対角の間に任意個の2を挟んでもよい:
    /// + + + +    +-+ + +
    ///  3         |3
    /// + + + +    + + + +
    ///    2    =>    2   
    /// + + + +    + + + +
    ///      3          3|
    /// + + + +    + + +-+
    ///
    bool apply(Board& b, int x, int y) const {
      if (b.numberAt(x, y) != 3) {
        // 3でなければ何もしない
        return false;
      }

      Point p = { x, y };
      return processAdjacentNeighbor(b, p, Directions::E)
        | processAdjacentNeighbor(b, p, Directions::S)
        | processDiagonalNeighbor(b, p, Directions::NE)
        | processDiagonalNeighbor(b, p, Directions::SE);
    }

  private:
    ///
    /// 隣接する3のパターン
    ///
    /// @param v チェックする方向(E or S)
    ///
    static bool processAdjacentNeighbor(Board& b, const Point& p, const Point& v) {
      // 隣接するマスが3でなければ何もしない
      if (b.numberAt(p.add<2>(v)) != 3) {
        return false;
      }

      // 線とバツを書く
      //
      //   + pl1 +
      //      3 <= p, v=S
      // x + plc +  x
      //      3
      //   + pl2 +
      auto plc = p.add(v); // 挟まれた線の位置
      auto pl1 = p.sub(v); // 近い側の線の位置
      auto pl2 = plc.add<2>(v); // 遠い側の線の位置
      return b.setExistsAt(pl1)
        | b.setExistsAt(pl2)
        | b.setForbiddenAt(plc.add<2>(v.lrot()))
        | b.setForbiddenAt(plc.add<2>(v.rrot()))
        | (noSmallLoop && b.setExistsAt(plc));
    }

    ///
    /// 対角の3のパターン
    ///
    /// @param v1 チェックする方向(NE or SE)
    ///
    static bool processDiagonalNeighbor(Board& b, const Point& p, const Point& v1) {
      // 対角の3を探す
      // p = もとの3の位置、q = 対角の3の位置
      auto q = p;
      while (true) {
        q = q.add<2>(v1);
        switch (b.numberAt(q)) {
        case 2:
          continue; // 2のマスがあれば次のマスを調べる
        case 3:
          break; // 3のマスがあればそれが対角の3
        default:
          return false; // 2や3以外に出会ったら条件不成立
        }
        break;
      }

      // 線とバツを書き込む
      return b.setExistsAt(p.subX(v1))
        | b.setExistsAt(p.subY(v1))
        | b.setExistsAt(q.addX(v1))
        | b.setExistsAt(q.addY(v1));
    }
  };

  static_assert(CellRule<ThreePairInitialRule<true>>);

} // namespace sli