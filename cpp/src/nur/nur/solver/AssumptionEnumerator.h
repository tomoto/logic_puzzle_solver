#pragma once

#include "nur/data/Board.h"
#include "nur/data/Assumption.h"
#include "nur/solver/topology/Topology.h"

namespace nur {

  struct AssumptionMetrics {
    Point p;
    int shimaCellsAround = 0;
    int kabeCellsAround = 0;
    float effectiveness = 0.0f;
    int fixedCellsAround() const {
      return shimaCellsAround + kabeCellsAround;
    }
  };

  struct AssumptionEnumerator {
    static vector<Assumption> enumerate(Board& b, const Topology& topology) {
      vector possiblePointsSortedBySize = utl::transform(topology.shimaOpenPoints, [](auto& p) {
        return std::pair{ p.first, p.second.size() };
        });
      std::sort(RANGE(possiblePointsSortedBySize), [](auto& a, auto& b) {
        return a.second < b.second;
        });

      vector<Assumption> result;
      for (auto& [id, size] : possiblePointsSortedBySize) {
        for (auto& p : topology.shimaOpenPoints.at(id)) {
          result.push_back({ p, false });
        }
      }
      for (auto& [id, points] : topology.kabeOpenPoints) {
        for (auto& p : points) {
          result.push_back({ p, true });
        }
      }
      return result;
    }

    // static vector<Assumption> enumerate(Board& b, const Topology& topology) {
    //   std::set<Point> possiblePoints;
    //   for (auto& [id, points] : topology.shimaOpenPoints) {
    //     possiblePoints.insert(points.begin(), points.end());
    //   }
    //   for (auto& [id, points] : topology.kabeOpenPoints) {
    //     possiblePoints.insert(points.begin(), points.end());
    //   }

    //   auto assumptionMetricsList = utl::transform(possiblePoints, [&](auto& p) {
    //     int shimaCellsAround = 0;
    //     int kabeCellsAround = 0;
    //     float effectiveness = 0.0f;
    //     for (auto& d : Directions::adjacents) {
    //       auto cell = b.cellAt(p.add(d));
    //       if (cell.kabe()) {
    //         kabeCellsAround++;
    //       } else if (cell.anyShima()) {
    //         shimaCellsAround++;
    //         effectiveness += 1.0f / topology.shimaOpenPoints.at(cell.shimaId()).size();
    //       }
    //     }
    //     return AssumptionMetrics{ p, shimaCellsAround, kabeCellsAround, effectiveness };
    //     });

    //   std::sort(assumptionMetricsList.begin(), assumptionMetricsList.end(),
    //     [](auto& a, auto& b) {
    //       return (a.fixedCellsAround() == b.fixedCellsAround())
    //         ? a.effectiveness > b.effectiveness
    //         : a.fixedCellsAround() > b.fixedCellsAround();
    //     });

    //   vector<Assumption> result(assumptionMetricsList.size());
    //   for (auto& m : assumptionMetricsList) {
    //     if (m.kabeCellsAround) result.push_back({ m.p, true });
    //     if (m.shimaCellsAround) result.push_back({ m.p, false });
    //   }
    //   return result;
    // }

  };

} // namespace nur