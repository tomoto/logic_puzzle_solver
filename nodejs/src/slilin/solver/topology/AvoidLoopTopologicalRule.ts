import { TopologicalRule } from '../../../common/solver/SolverEngine'
import {
  Track,
  TrackTopology,
} from '../../../common/solver/topology/TrackTopology'
import { Point } from '../../../util/Point'
import { or, evalAllOr } from '../../../util/Utils'
import { Vector } from '../../../util/Vector'
import { SlilinBoard } from '../../data/SlilinBoard'

export class AvoidLoopTopologicalRule
  implements TopologicalRule<SlilinBoard, TrackTopology>
{
  apply(b: SlilinBoard, topology: TrackTopology): boolean {
    if (topology.tracks.length < 2) {
      // 線が2本未満の場合はチェック不要
      return false
    }

    return evalAllOr(topology.tracks, track => applyToTrack(b, track))
  }
}

function applyToTrack(b: SlilinBoard, track: Track): boolean {
  const p = Vector.atP(track.ends[0])
  const v = Vector.atP(track.ends[1]).addP(p, -1)
  const d = track.distanceBetweenEnds

  if (track.legs >= 3 && d === 2) {
    // 端点同士が隣接している場合、その間をバツにする
    const v1 = new Vector(v.x >> 1, v.y >> 1)
    return b.lineCellAtP(p.addP(v1)).setForbidden()
  } else if (d === 4) {
    // 端点同士が2マス離れている場合、その間をバツにするかどうか決める
    if (v.x === 0 || v.y === 0) {
      const v1 = new Vector(v.x >> 2, v.y >> 2)
      return checkLoopAndSetForbidden(b, p, v1, v1)
    } else {
      const v1 = new Vector(v.x >> 1, 0)
      const v2 = new Vector(0, v.y >> 1)
      return or(
        checkLoopAndSetForbidden(b, p, v1, v2),
        checkLoopAndSetForbidden(b, p, v2, v1),
      )
    }
  } else {
    return false
  }
}

function checkLoopAndSetForbidden(
  b: SlilinBoard,
  p0: Vector,
  v1: Vector,
  v2: Vector,
): boolean {
  const p = p0.addP(v1)
  const c1 = b.lineCellAtP(p)
  if (c1.fixed) {
    return false
  }

  if (isOnlyOneChoice(b, p, v1, v2)) {
    const c2 = b.lineCellAtP(p.addP(v1).addP(v2))
    if (c2.fixed) {
      return false
    }
    return or(c1.setForbidden(), c2.setForbidden())
  }

  return false
}

function isOnlyOneChoice(
  b: SlilinBoard,
  p0: Vector,
  v1: Vector,
  v2: Vector,
): boolean {
  const p1 = p0.addP(v1)
  const p2 = p1.addP(v2)
  return Point.adjacent4(p1.x, p1.y).every(
    p =>
      Point.equals(p, p0) || Point.equals(p, p2) || b.lineCellAtP(p).forbidden,
  )
}
