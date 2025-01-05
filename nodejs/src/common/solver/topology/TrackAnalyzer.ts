import assert from 'assert'
import { Point } from '../../../util/Point'
import { Optional } from '../../../util/Utils'
import { Track, TrackTopology } from './TrackTopology'

export const TrackAnalyzer = {
  analyze,
}

export interface TrackAnalysisSubject {
  findNextNode(): Optional<Point>
  navigateOverUnvisitedEdgeFrom(p: Point, trackNumber: number): Optional<Point>
}

function analyze(g: TrackAnalysisSubject): TrackTopology {
  let trackNumber = 0
  let p: Optional<Point>
  const tracks: Track[] = []
  while ((p = g.findNextNode())) {
    trackNumber++
    const t0 = findEnd(g, p, trackNumber)
    const t1 = findEnd(g, p, trackNumber)
    const t = connectTracks(t0, t1)
    if (t) {
      tracks.push(t)
    }
  }
  return new TrackTopology(tracks)
}

function findEnd(
  g: TrackAnalysisSubject,
  p0: Point,
  trackNumber: number,
): Optional<Track> {
  let leg = 0
  let p1 = p0
  let p: Optional<Point>
  while ((p = g.navigateOverUnvisitedEdgeFrom(p1, trackNumber))) {
    leg++
    p1 = p
  }
  return leg > 0 ? new Track([p0, p1], leg) : undefined
}

function connectTracks(
  t0: Optional<Track>,
  t1: Optional<Track>,
): Optional<Track> {
  if (!t0) return t1
  if (!t1) return t0
  assert(Point.equals(t0.ends[0], t1.ends[0]))

  return new Track([t0.ends[1], t1.ends[1]], t0.legs + t1.legs)
}
