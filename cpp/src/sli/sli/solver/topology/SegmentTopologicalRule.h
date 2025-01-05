#pragma once

#include "sli/data/Board.h"
#include "cmn/solver/topology/SegmentAnalyzer.h"

#include <map>

namespace sli {

  struct SegmentTopologicalRule {
    bool apply(Board& b, const cmn::TrackTopology& topology) {
      struct {
        Board& b;
        int width = b.logicalWidth + 1;
        int height = b.logicalHeight + 1;
        bool connected(int x, int y, const Point& d) {
          return !b.lineAt(d.add({ x * 2 + 1, y * 2 + 1 })).fixed();
        };
        bool belongsToSegment(int, int) { return true; }
      } subject = { b };

      auto [segmentIds, maxSegmentId] = cmn::SegmentAnalyzer::analyze4Way(subject);
      vector<bool> endpointCountMap(maxSegmentId + 1, false);

      auto flipCountMap = [&endpointCountMap, &segmentIds](const Point& p) {
        int id = segmentIds.at((p.x - 1) / 2, (p.y - 1) / 2);
        endpointCountMap[id] = !endpointCountMap[id];
        };

      for (auto& track : topology.tracks) {
        flipCountMap(track.start);
        flipCountMap(track.end);
      }

      for (int count : endpointCountMap) {
        if (count & 1) {
          throw HatanException("The number of the incoming tracks in a segment should be even");
        }
      }

      return false;
    }
  };

} // namespace sli
