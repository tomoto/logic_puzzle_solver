#pragma once

#include "sli/data/Base.h"
#include "sli/data/Board.h"
#include "cmn/solver/topology/TrackAnalyzer.h"
#include "utl/Array2D.h"

namespace sli {

  struct TrackAnalysisSubject {
    Board& board;
    int x = 1;
    int y = 1;
    Array2D<bool> visitedFlags;

    TrackAnalysisSubject(Board& b) :
      board(b), visitedFlags(b.physicalWidth, b.physicalHeight, false) {}

    Point findNextNode() {
      while (y < board.physicalHeight) {
        Point p = { x, y };
        x += 2;
        if (x >= board.physicalWidth) {
          x = 1;
          y += 2;
        }

        if (!visitedFlags.at(p.x, p.y) && board.surroundingStatsAt(p.x, p.y).exists > 0) {
          visitedFlags.at(p.x, p.y) = true;
          return p;
        }
      }
      return { Point::invalid };
    }

    Point navigateOverUnvisitedEdgeFrom(const Point& p, int) {
      // for (auto& d : Directions::adjacents) {
      //   auto q = p.add(d);
      //   auto b = visitedFlags(q.x, q.y);
      //   if (!b && board.lineAt(q.x, q.y).exists) {
      //     visitedFlags(q.x, q.y) = true;
      //     return q.add(d);
      //   }
      // }

      // performance critical
      auto [x, y] = p;
      if (!visitedFlags.at(x, y - 1) && board.lineAt(x, y - 1).exists) {
        visitedFlags.at(x, y - 1) = true;
        return { x, y - 2 };
      }
      if (!visitedFlags.at(x + 1, y) && board.lineAt(x + 1, y).exists) {
        visitedFlags.at(x + 1, y) = true;
        return { x + 2, y };
      }
      if (!visitedFlags.at(x, y + 1) && board.lineAt(x, y + 1).exists) {
        visitedFlags.at(x, y + 1) = true;
        return { x, y + 2 };
      }
      if (!visitedFlags.at(x - 1, y) && board.lineAt(x - 1, y).exists) {
        visitedFlags.at(x - 1, y) = true;
        return { x - 2, y };
      }
      return { Point::invalid };
    }
  };

  static_assert(cmn::TrackAnalyzer::Subject<TrackAnalysisSubject>);

} // namespace sli
