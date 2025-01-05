#pragma once

#include "CellRule.h"

namespace sli {

  struct DotRule {
    static constexpr CellType cellType = CellType::dot;
    static constexpr bool skipOutside = false;
    static constexpr bool skipUnmodifiedCells = true;

    ///
    /// 点に関わる基本的なルール
    ///
    bool apply(Board& b, int x, int y) const {
      auto& ss = b.surroundingStatsAt(x, y);

      if (ss.allFixed()) {
        // 点の周辺の線の状態がすべて決まっていれば何もしない
        return false;
      } else if (ss.forbidden == 3 || ss.exists == 2) {
        // 3方をxに囲まれているか、線が既に2本あれば、これ以上この点に線は入ってこない
        return b.setForbiddenAround(x, y);
      } else if (ss.forbidden == 2 && ss.exists == 1) {
        // 一本線が入ってきている状態で、残り候補がひとつなので、そこに線をつなげる
        return b.setExistsAround(x, y);
      } else if (ss.exists > 2) {
        // 3本以上線が入っていたらおかしい
        throw HatanException("DotRule: more than two lines coming into a dot");
      }

      return false;
    }
  };

  static_assert(CellRule<DotRule>);

} // namespace sli