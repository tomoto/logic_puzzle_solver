#pragma once

#include "nur/data/Board.h"
#include "Topology.h"

namespace nur {
  struct ShimaTopologicalRule {
    /// 面積が十分でないシマがカベに閉じ込められてはいけないルール
    bool apply(Board& b, Topology& topology) {
      bool changed = false;
      for (auto& [id, points] : topology.shimaOpenPoints) {
        if (CellStates::shima(id).confirmedShima() && b.shima(id).complete()) {
          // 完成したシマならスキップ
          continue;
        }

        if (points.size() == 1) {
          // 開放点が唯一⇒その開放点を取る
          changed |= b.reserveShima(*points.begin());
        } else if (points.size() == 0) {
          // 完成前に閉じ込められた⇒破綻
          throw cmn::HatanException("Shima is closed before completion.");
        }
      }
      return changed;
    }
  };
}