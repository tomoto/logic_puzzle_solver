#include "ShimaPotentialReachabilityRule.h"

namespace nur {

  bool ShimaPotentialReachabilityRule::apply(Board& b) const {
    bool changed = false;

    // TODO いちいち計算するのは遅い

    for (auto& shima : b.activeShimaList()) {
      if (!shima.dirty) continue;

      // フェルミ準位の計算
      int fermiPotential = calcFermiPotential(shima);

      for (int n = fermiPotential; n < shima.number; ++n) {
        if (shima.confirmedPointsByPotential[n].size() == 0) {
          // Rule 1: ポテンシャルごとに唯一の選択肢なら確定
          if (shima.possiblePointsByPotential[n].size() == 1) {
            auto p = *shima.possiblePointsByPotential[n].begin(); // Pointのコピーを作る
            changed |= b.confirmShima(p, shima.id);
          }
          // Rule 2: 選択肢が2つでポテンシャル0のマスを挟んで隣接する場合、挟まれたマスはカベに確定
          if (shima.possiblePointsByPotential[n].size() == 2) {
            auto it = shima.possiblePointsByPotential[n].begin();
            auto& p1 = *it;
            auto& p2 = *++it;
            auto confirmKabeIfPotentialIsZero = [&](const Point& p) {
              return shima.potentialAt(p) == 0 && b.confirmKabe(p);
              };

            if (p1.manhattanDistance(p2) == 2) { // マスを挟んで隣接している
              if (p1.x == p2.x || p1.y == p2.y) { // 水平または垂直に相対
                changed |= confirmKabeIfPotentialIsZero({ (p1.x + p2.x) / 2, (p1.y + p2.y) / 2 });
              } else { // 角で隣接
                changed |= confirmKabeIfPotentialIsZero({ p1.x, p2.y })
                  | confirmKabeIfPotentialIsZero({ p2.x, p1.y });
              }
            }
          }
        }
      }

      // Rule 2
      // if (fermiPotential == 1 && shima.possiblePointsByPotential[1].size() == 2) {
      //   auto it = shima.possiblePointsByPotential[1].begin();
      //   auto& p1 = *it;
      //   auto& p2 = *++it;
      //   if (std::abs(p1.x - p2.x) == 1 && std::abs(p1.y - p2.y) == 1) {
      //     if (shima.potentialAt({ p1.x, p2.y }) == 0) {
      //       changed |= b.confirmKabe({ p1.x, p2.y });
      //     } else {
      //       changed |= b.confirmKabe({ p2.x, p1.y });
      //     }
      //   }
      // }

      // Rule 3
      for (auto& points : shima.confirmedPointsByPotential) {
        auto pointsCopy = points;
        for (auto& p : pointsCopy) {
          int n = shima.potentialAt(p);
          auto flags = std::accumulate(
            Directions::adjacents.begin(),
            Directions::adjacents.end(),
            0,
            [&](int f, const Point& d) {
              return (f << 1) | (shima.potentialAt(p.add(d)) > 0);
            });
          auto d = flags == 8 ? Directions::N
            : flags == 4 ? Directions::E
            : flags == 2 ? Directions::S
            : flags == 1 ? Directions::W
            : Point{ 0,0 };
          if (d != Point{ 0,0 }) {
            changed |= b.confirmShima(p.add(d), shima.id);
          }
        }
      }

    }

    return changed;
  }

  int ShimaPotentialReachabilityRule::calcFermiPotential(const Shima& shima) const {
    int remainingArea = shima.number;
    int maxReachablePotential = shima.number;
    while (maxReachablePotential > 1) {
      remainingArea -= shima.possiblePointsByPotential[maxReachablePotential].size()
        + shima.confirmedPointsByPotential[maxReachablePotential].size();
      if (remainingArea <= 0) break;
      maxReachablePotential--;
    }
    return maxReachablePotential;
  }

} // namespace nur
