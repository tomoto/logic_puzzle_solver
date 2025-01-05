#include "NumberCornerRule.h"

#include "sli/data/Board.h"
#include "utl/Utils.h"
#include "cmn/data/HatanException.h"

namespace sli {
  ///
  /// 1マスの小ループ禁止(これにより1x1の盤面は解けない)
  ///
  /// Rule 1:
  ///   x       x
  /// + +x    +X+x
  /// |    => | X
  /// +-+     +-+
  ///
  /// Rule 2:
  ///  x x      x x
  /// x+ +x    x+X+x
  ///       =>  X X
  ///  + +x     +X+x
  ///    x        x
  ///
  /// @param p 現在のマス
  /// @param v 角の方向
  ///
  inline bool processEmpty(Board& b, const Point& p, const Point& v) {
    auto [v1, v2] = Helper::vectorsToCornerLines(v);
    auto pl1 = p.add(v1);
    auto pl2 = p.add(v2);

    bool changed = false;
    if (b.lineAt(pl1).forbidden && b.lineAt(pl2).forbidden) {
      // Rule 1
      if (b.lineAt(p.x - v.x, p.y).exists && b.lineAt(p.x, p.y - v.y).exists) {
        changed |= b.setForbiddenAt(p.addX(v)) | b.setForbiddenAt(p.addY(v));
      }

      // Rule 2
      if (b.lineAt(p.add(v1.lrot())).forbidden
        && b.lineAt(p.add(v2.lrot())).forbidden
        && b.lineAt(p.add(v1.rrot())).forbidden
        && b.lineAt(p.add(v2.rrot())).forbidden) {
        changed |= Directions::applyToAdjacents([&](const Point& v) {
          return b.setForbiddenAt(p.add(v));
          });
      }
    }
    return changed;
  }

  ///
  /// 1のマスの角が押さえられた場合
  ///
  ///   x       x
  /// + +x    +X+x
  ///  1   =>  1X
  /// + +     + +
  ///
  /// @param p 現在のマス
  /// @param v 角の方向
  ///
  inline bool process1(Board& b, const Point& p, const Point& v) {
    auto [pl1, pl2] = Helper::pointsAtCornerLines(p, v);

    if (b.lineAt(pl1).forbidden && b.lineAt(pl2).forbidden) {
      return b.setForbiddenAt(p.addX(v)) | b.setForbiddenAt(p.addY(v));
    }

    return false;
  }

  ///
  /// 3のマスの角のルール
  ///
  /// Rule 1:
  ///   x       x
  /// + +x    +-+x
  ///  3   =>  3|
  /// + +     + +
  ///
  /// Rule 2:
  ///   |
  /// + +-
  ///  3  => Hatan
  /// + +
  ///
  /// Rule 3:
  ///   |      |
  /// + +    + +x
  ///  3  => |3
  /// + +    +-+
  ///
  /// 対角方向に2が連続する場合、同じルールが2の先にも推移的に適用される
  ///
  /// Rule 1:
  ///     x         x
  /// + + +x    + +-+x
  ///    2         2|
  /// + + +  => +-+ +
  ///  3         3|
  /// + + +     + + +
  ///
  /// Rule 2:
  ///     |
  /// + + +-
  ///    2
  /// + + +  => Hatan
  ///  3
  /// + + +
  /// 
  /// Rule 3:
  ///     |        |
  /// + + +    + + +X
  ///    2        2 
  /// + + + => + + +
  ///  3       |3
  /// + + +    +-+ +
  /// 
  /// @param p0 出発点のマス
  /// @param v 角の方向
  ///
  struct Process3Result {
    bool changed = false;
    enum Rule {
      none = 0,
      rule1 = 1,
      rule3 = 3,
    } rule = none;

    void setRule(Rule newRule) {
      if (rule > 0 && rule != newRule) {
        throw HatanException("NumberCornerRule: the constraints are broken around 3");
      }
      rule = newRule;
    }
  };

  void process3Internal(Board& b, const Point& p, const Point& v, Process3Result& result) {
    // 対角の2があれば再帰的に処理
    if (auto q = p.add<2>(v); b.numberAt(q) == 2) {
      process3Internal(b, q, v, result);
    }

    auto [pl1, pl2] = Helper::pointsAtCornerLines(p, v);
    auto& line1 = b.lineAt(pl1);
    auto& line2 = b.lineAt(pl2);

    if ((line1.forbidden && line2.forbidden) // Rule1: 遠い側の角が塞がれている場合
      || result.rule == Process3Result::rule1) { // (バックトラック中にも適用)
      result.changed |= b.setExistsAt(p.addX(v)) | b.setExistsAt(p.addY(v));
      result.setRule(Process3Result::rule1);
    } else if (line1.exists && line2.exists) { // Rule2: 遠い側の角を線が通過している場合
      throw HatanException("NumberCornerRule: the constraints are broken around 3");
    } else if (line1.exists) { // Rule3: 遠い側の角に線が侵入している場合
      result.changed |= b.setForbiddenAt(pl2);
      result.setRule(Process3Result::rule3);
    } else if (line2.exists) { // Rule3: 同上
      result.changed |= b.setForbiddenAt(pl1);
      result.setRule(Process3Result::rule3);
    }
  }

  inline bool process3(Board& b, const Point& p, const Point& v) {
    // 出発点の3と対角の2に対して再帰的に処理
    Process3Result result;
    process3Internal(b, p, v, result);

    // ルール3が適用された場合、出発点の3(のvの反対側)に線を引く
    if (result.rule == 3) {
      result.changed |= b.setExistsAt(p.subX(v)) | b.setExistsAt(p.subY(v));
    }
    return result.changed;
  }

  /// 数字のマスの角からわかる条件
  bool NumberCornerRule::apply(Board& b, int x, int y) const {
    // 周りの線がすべて確定していれば何もしない
    if (b.surroundingStatsAt(x, y).allFixed()) {
      return false;
    }

    using namespace Directions;

    // 空/1/3に応じて場合分け
    Point p = { x, y };
    switch (b.numberAt(x, y)) {
    case NumberCell::empty:
      return applyToCorners([&](const Point& v) { return processEmpty(b, p, v); });
    case 1:
      return applyToCorners([&](const Point& v) { return process1(b, p, v); });
    case 3:
      return applyToCorners([&](const Point& v) { return process3(b, p, v); });
    default:
      return false; // nothing to do
    }
  }

} // namespace sli