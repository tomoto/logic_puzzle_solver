#pragma once

#include "nur/data/Board.h"
#include "Topology.h"

namespace nur {
  struct KabeTopologicalRule {
    /// カベ全体がひとつにつながっていなければならないルール
    bool apply(Board& b, Topology& topology) {
      if (topology.kabeOpenPoints.size() <= 1) {
        // カベ(連続する黒マスのグループ)が1つ以下の場合は何もしない
        return false;
      }

      bool changed = false;
      for (auto& [id, points] : topology.kabeOpenPoints) {
        if (points.size() == 0) {
          // 開放点がない＝孤立⇒破綻
          throw cmn::HatanException("Kabes are separated");
        } else if (points.size() == 1) {
          // 開放点が唯一⇒その開放点をカベに確定
          changed |= b.confirmKabe(*points.begin());
        }
      }
      return changed;
    }
  };
}