#pragma once

#include "sli/data/Board.h"
#include "cmn/solver/topology/TrackTopology.h"

namespace sli {

  struct AvoidLoopTopologicalRule {
    bool apply(Board& b, const cmn::TrackTopology& topology) {
      if (topology.tracks.size() < 2) {
        // 線が2本未満の場合はチェック不要
        return false;
      }

      return evalAllOr(topology.tracks, [&b](const auto& track) {
        return applyToTrack(b, track);
        });
    }

  private:
    static bool applyToTrack(Board& b, const cmn::Track& track) {
      auto p = track.start;
      auto v = track.end.sub(p);
      auto d = track.distanceBetweenEnds;

      if (track.legs >= 3 && d == 2) {
        // 端点同士が隣接している場合、その間をバツにする
        Point v1 = { v.x / 2, v.y / 2 };
        return b.setForbiddenAt(p.add(v1));
      } else if (d == 4) {
        // 端点同士が2マス離れている場合、その間をバツにするかどうか決める 
        if (v.x == 0 || v.y == 0) {
          // ストレート
          Point v1 = { v.x / 4, v.y / 4 };
          return checkLoopAndSetForbidden(b, p, v1, v1);
        } else {
          // 折れ曲がり
          Point v1 = { v.x / 2, 0 };
          Point v2 = { 0, v.y / 2 };
          return checkLoopAndSetForbidden(b, p, v1, v2)
            | checkLoopAndSetForbidden(b, p, v2, v1);
        }
      } else {
        return false;
      }
    };

    static bool checkLoopAndSetForbidden(
      Board& b,
      const Point& p0,
      const Point& v1,
      const Point& v2)
    {
      auto p1 = p0.add(v1);
      if (b.lineAt(p1).fixed()) {
        return false;
      }

      if (isOnlyOneChoice(b, p1, v1, v2)) {
        auto p2 = p1.add(v1).add(v2);
        if (b.lineAt(p2).fixed()) {
          return false;
        }
        return b.setForbiddenAt(p1) | b.setForbiddenAt(p2);
      }

      return false;
    }

    static bool isOnlyOneChoice(
      Board& b,
      const Point& p0,
      const Point& v1,
      const Point& v2)
    {
      auto p1 = p0.add(v1);
      auto p2 = p1.add(v2);
      for (auto& d : Directions::adjacents) {
        auto p = p1.add(d);
        if (p0 != p && p2 != p && !b.lineAt(p).forbidden) {
          return false;
        }
      }
      return true;
    }
  };

} // namespace sli