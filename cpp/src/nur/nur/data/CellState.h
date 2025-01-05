#pragma once

#include "cmn/data/HatanException.h"

#include <cassert>

namespace nur {

  /// マスの状態を表す値
  /// データとしてはintひとつであることが確定しているので、値コピーで引き回せる
  struct CellState {
    static constexpr int outOfBoundsValue = -0x10000000;
    static constexpr int reservedShimaBase = 0x10000000;

    auto operator<=>(const CellState&) const = default;

    /// 0: open
    /// <0: 黒マス、値はカベ(連続した黒マスのグループ)ごとに振られるID
    /// >0 && <reservedShimaBase: シマが確定した白マス、値はシマのID
    /// >=reservedShimaBase: 白マスであることが確定しているが、シマが確定していない(???)
    ///
    int value = 0;

    /// シマが確定した白マスならtrue
    bool confirmedShima() const {
      return value > 0 && value < reservedShimaBase;
    }

    // シマが確定していない白マスならtrue
    bool reservedShima() const {
      return value >= reservedShimaBase;
    }

    // シマが確定していようがいまいが白マスであればtrue
    bool anyShima() const {
      return value > 0;
    }

    // カベかシマかもわからない状態ならtrue
    bool open() const {
      return value == 0;
    }

    // カベまたは範囲外ならtrue
    bool kabeOrOOB() const {
      return value < 0;
    }

    // 実際の(盤面上の)壁ならtrue
    bool kabe() const {
      return value < 0 && value != outOfBoundsValue;
    }

    // シマのIDを返す
    // TODO: unfixedShimaの場合の処理
    int shimaId() const {
      assert(value > 0);
      return value;
    }

    // 実際のカベならカベIDを、そうでなければ0を返す
    int kabeId() const {
      return kabe() ? value : 0;
    }
  };

  namespace CellStates {
    static constexpr CellState outOfBounds = { CellState::outOfBoundsValue };
    static constexpr CellState shima(int id) {
      assert(id > 0);
      return { id };
    }
    static constexpr CellState kabe(int id) {
      assert(id < 0);
      return { id };
    }
  }
}
