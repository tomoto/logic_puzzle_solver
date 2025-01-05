import { Point } from '../../../util/Point'

export class Track {
  readonly distanceBetweenEnds: number

  constructor(
    readonly ends: [Point, Point],
    readonly legs: number,
  ) {
    this.distanceBetweenEnds = Point.manhattanDistance(ends[0], ends[1])
  }

  get closed(): boolean {
    return this.distanceBetweenEnds === 0
  }
}

export class TrackTopology {
  readonly totalLeg: number

  constructor(readonly tracks: Track[]) {
    this.totalLeg = tracks.reduce((total, t) => total + t.legs, 0)
  }
}
