#pragma once

#include "CellRule.h"

namespace sli {

  struct BasicNumberRule {
    static constexpr CellType cellType = CellType::number;
    static constexpr bool skipOutside = true;
    static constexpr bool skipUnmodifiedCells = true;

    ///
    /// 「数字のマスの周りの線の数は数字と等しい」ルール
    ///
    bool apply(Board& b, int x, int y) const {
      int n = b.numberAt(x, y);
      if (n == NumberCell::empty) {
        // 数値が入っていなければ何もしない
        return false;
      }

      auto& ss = b.surroundingStatsAt(x, y);
      if (ss.allFixed()) {
        // すべての線が決まっていれば何もしない
        return false;
      } else if (ss.exists == n) {
        // 線の数が指定に達したので、空いている箇所を禁止で確定する
        return b.setForbiddenAround(x, y);
      } else if (4 - ss.forbidden == n) {
        // 禁止の数が指定に達したので、空いている箇所を線で確定する
        return b.setExistsAround(x, y);
      } else if (ss.exists > n || ss.forbidden > 4 - n) {
        // 線の数がおかしい
        throw HatanException("BasicNumberRule: the number of lines around a number does not satisfy the constraint");
      }

      return false;
    }
  };

  static_assert(CellRule<BasicNumberRule>);

} // namespace sli