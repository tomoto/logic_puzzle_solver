#pragma once

#include "nur/data/Board.h"
#include "nur/solver/topology/Topology.h"
#include "cmn/solver/topology/SegmentAnalyzer.h"

namespace nur {
  struct NoBlankShimaAllowedTopologicalRule {
    /// 数字の入らないシマがカベに囲まれてはいけないルール

    bool apply(Board& b, Topology& topology) {
      struct {
        Board& b;
        int width = b.width;
        int height = b.height;
        bool connected(int x, int y, const Point& d) {
          return !b.cellAt({ x, y }).kabe() && !b.cellAt({ x + d.x, y + d.y }).kabe();
        };
        bool belongsToSegment(int, int) { return true; }
      } subject = { b };

      auto result = cmn::SegmentAnalyzer::analyze4Way(subject);

      std::map<int, bool> numberExistsMap;
      for (auto& y : range(0, b.height)) {
        for (auto& x : range(0, b.width)) {
          if (!b.cellAt({ x, y }).kabe()) {
            int n = result.segmentIds.at(x, y);
            if (!numberExistsMap.contains(n)) {
              numberExistsMap[n] = false;
            }
          }
        }
      }

      for (Shima& shima : b.activeShimaList()) {
        int n = result.segmentIds.at(shima.center.x, shima.center.y);
        numberExistsMap[n] = true;
      }

      auto setKabeOnSegment = [&](int targetId) {
        bool changed = false;
        for (auto& y : range(0, b.height)) {
          for (auto& x : range(0, b.width)) {
            if (result.segmentIds.at(x, y) == targetId) {
              changed |= b.confirmKabe({ x, y });
            }
          }
        }
        return changed;
        };

      bool changed = false;
      for (auto& [id, exists] : numberExistsMap) {
        if (!exists) {
          changed |= setKabeOnSegment(id);
        }
      }
      return changed;

    }
  };

}