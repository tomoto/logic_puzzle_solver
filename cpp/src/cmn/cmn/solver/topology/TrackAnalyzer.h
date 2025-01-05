#pragma once

#include "cmn/data/Base.h"
#include "utl/Point.h"
#include "TrackTopology.h"

#include <numeric>
#include <concepts>
#include <cassert>

namespace cmn::TrackAnalyzer {

  template <typename S>
  concept Subject = requires (S & s, int trackId) {
    { s.findNextNode() } -> std::convertible_to<Point>;
    { s.navigateOverUnvisitedEdgeFrom(Point{}, trackId) } -> std::convertible_to<Point>;
  };

  template <Subject S>
  inline Track _findEnd(S& g, const Point& p0, int trackId) {
    int leg = 0;
    auto p1 = p0;
    Point p2;
    while (p2 = g.navigateOverUnvisitedEdgeFrom(p1, trackId), p2.valid()) {
      leg++;
      p1 = p2;
    }
    return { p0, p1, leg };
  }

  template <Subject S>
  TrackTopology analyze(S&& g) {
    int trackId = 0;
    vector<Track> tracks;
    Point p;
    while (p = g.findNextNode(), p.valid()) {
      trackId++;
      auto t0 = _findEnd(g, p, trackId);
      auto t1 = _findEnd(g, p, trackId);
      assert(t0.start == t1.start);
      if (auto t = Track(t0.end, t1.end, t0.legs + t1.legs); t.valid()) {
        tracks.push_back(std::move(t));
      }
    }
    return { std::move(tracks) };
  }

} // namespace cmn::TrackAnalyzer