#pragma once

#include "cmn/data/Base.h"
#include "utl/Point.h"

#include <numeric>

namespace cmn {

  struct Track {
    Point start;
    Point end;
    int legs;
    int distanceBetweenEnds = 0;

    Track(const Point& start, const Point& end, int legs) : start(start), end(end), legs(legs) {
      distanceBetweenEnds = start.manhattanDistance(end);
    }

    DEFAULT_MOVE_CTORS(Track); // move only, just in case

    bool closed() const {
      return distanceBetweenEnds == 0;
    }

    bool valid() const {
      return legs > 0;
    }
  };

  struct TrackTopology {
    int totalLeg;
    vector<Track> tracks;

    TrackTopology() : totalLeg(0) {}

    TrackTopology(vector<Track>&& tracks) :
      totalLeg(calcTotalLeg(tracks)),
      tracks(std::move(tracks)) {}

    DEFAULT_MOVE_CTORS(TrackTopology);

    static int calcTotalLeg(const vector<Track>& tracks) {
      return std::accumulate(
        tracks.begin(),
        tracks.end(),
        0,
        [](int total, const Track& t) { return total + t.legs; });
    }
  };

} // namespace cmn