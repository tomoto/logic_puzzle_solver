#pragma once

#include "CellRule.h"

namespace sli {

  struct IncomingCornerRule {
    static constexpr CellType cellType = CellType::number;
    static constexpr bool skipOutside = false;
    static constexpr bool skipUnmodifiedCells = true;

    ///
    /// マスへの線の進入に関するルール(詳細はprivateメソッドの方に記述)
    /// @see processIncoming
    ///
    bool apply(Board& b, int x, int y) const {
      using Directions::applyToCorners;

      bool changed = false;

      Point p = { x, y };
      changed |= processAny(b, p);

      switch (b.numberAt(x, y)) {
      case 1:
        changed |= applyToCorners([&](const Point& v) { return process1(b, p, v); });
        break;
      case 2:
        changed |= applyToCorners([&](const Point& v) { return process2(b, p, v); });
        break;
      case 3:
        changed |= applyToCorners([&](const Point& v) { return process3(b, p, v); });
        break;
      }
      return changed;
    }

  private:
    ///
    /// マスへの線の進入に関するルール
    ///
    ///    | <- pの中にこの角から線が進入するとわかっている場合に、
    ///  + +X <- 出ていく線を禁止する(または禁止されていない方に線を引く)
    ///   p v=NE
    ///  + +
    ///
    /// 相対するマスが1の場合、さらに禁止できる:
    ///   +X+ 
    ///   |1X
    /// + +X+
    ///  p
    /// + +
    ///
    /// 相対するマスが3の場合、さらに線が決まる:
    ///   +-+ 
    ///   |3|
    /// + +X+
    ///  p
    /// + +
    ///
    /// 相対するマスが2の場合、同じルールを先へ先へと適用できる:
    ///     | <- 同じルール
    ///   + +X <-
    ///   |2
    /// + +X+
    ///  p
    /// + +
    ///
    static bool processIncoming(Board& b, const Point& p0, const Point& v) {
      // 角の先にある数字マスと、その隣の線の位置を計算
      auto p1 = p0.add<2>(v);
      auto q1 = p1.subX(v);
      auto q2 = p1.subY(v);
      auto& line1 = b.lineAt(q1);
      auto& line2 = b.lineAt(q2);

      bool changed = false;

      // 進入してくる線の状態を確定
      changed |= line1.forbidden && b.setExistsAt(q2);
      changed |= line2.forbidden && b.setExistsAt(q1);
      changed |= line1.exists && b.setForbiddenAt(q2);
      changed |= line2.exists && b.setForbiddenAt(q1);

      // 数値マスの状態に応じてさらに確定
      switch (b.numberAt(p1)) {
      case 1:
        changed |= b.setForbiddenAt(p1.addX(v)) | b.setForbiddenAt(p1.addY(v));
        break;
      case 2:
        changed |= processIncoming(b, p1, v);
        break;
      case 3:
        changed |= b.setExistsAt(p1.addX(v)) | b.setExistsAt(p1.addY(v));
        break;
      }

      return changed;
    }

    ///
    /// マスへの線の進入を検出し、ルールを適用する(全数値共通)
    /// 
    /// +x+ <-この点から線が進入すると判断
    ///  p| v=NE
    /// + +
    ///
    static bool processAny(Board& b, const Point& p) {
      // 呼び出し回数が多いので最適化する
      static LineCellInfo emptyLine;
      auto& line1 = p.y > 0 ? b.lineAt(p.x, p.y - 1) : emptyLine;
      auto& line2 = p.x < b.physicalWidth - 1 ? b.lineAt(p.x + 1, p.y) : emptyLine;
      auto& line3 = p.y < b.physicalHeight - 1 ? b.lineAt(p.x, p.y + 1) : emptyLine;
      auto& line4 = p.x > 0 ? b.lineAt(p.x - 1, p.y) : emptyLine;

      auto onlyOneExists = [](const LineCellInfo& line1, const LineCellInfo& line2) {
        return (line1.forbidden && line2.exists) || (line1.exists && line2.forbidden);
        };

      return (onlyOneExists(line1, line2) && processIncoming(b, p, Directions::NE))
        | (onlyOneExists(line2, line3) && processIncoming(b, p, Directions::SE))
        | (onlyOneExists(line3, line4) && processIncoming(b, p, Directions::SW))
        | (onlyOneExists(line4, line1) && processIncoming(b, p, Directions::NW));
    }

    ///
    /// 1のマスへの線の進入を検出し、ルールを適用する
    /// 
    /// + + <-この点から線が進入すると判断
    /// x1 v=NE
    /// +x+
    ///
    static bool process1(Board& b, const Point& p, const Point& v) {
      return (b.lineAt(p.subX(v)).forbidden && b.lineAt(p.subY(v)).forbidden)
        && processIncoming(b, p, v);
    }

    ///
    /// 2のマスへの線の進入を検出し、ルールを適用する
    /// 
    /// + + <-この点から線が進入すると判断
    ///  2 v=NE
    /// + +x
    ///   x
    ///
    static bool process2(Board& b, const Point& p, const Point& v) {
      auto cornerBlocked = [&](const Point& d) {
        auto [pl1, pl2] = Helper::pointsAtCornerLines(p, d);
        return b.lineAt(pl1).forbidden && b.lineAt(pl2).forbidden;
        };
      return (cornerBlocked(v.lrot()) || cornerBlocked(v.rrot()))
        && processIncoming(b, p, v);
    }

    ///
    /// 2のマスへの線の進入を検出し、ルールを適用する
    /// 
    /// + + <-この点から線が進入すると判断
    /// |3 v=NE
    /// +-+
    ///
    static bool process3(Board& b, const Point& p, const Point& v) {
      return (b.lineAt(p.subX(v)).exists && b.lineAt(p.subY(v)).exists)
        && processIncoming(b, p, v);
    }
  };

  static_assert(CellRule<IncomingCornerRule>);

} // namespace sli