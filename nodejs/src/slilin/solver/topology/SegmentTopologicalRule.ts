import { HatanError } from '../../../common/data/HatanError'
import { TopologicalRule } from '../../../common/solver/SolverEngine'
import { SegmentAnalyzer } from '../../../common/solver/topology/SegmentAnalyzer'
import { TrackTopology } from '../../../common/solver/topology/TrackTopology'
import { Point } from '../../../util/Point'
import { SlilinBoard } from '../../data/SlilinBoard'

export class SegmentTopologicalRule
  implements TopologicalRule<SlilinBoard, TrackTopology>
{
  apply(b: SlilinBoard, topology: TrackTopology): boolean {
    const { segmentIds } = SegmentAnalyzer.analyze4Way({
      width: b.logicalWidth + 1,
      height: b.logicalHeight + 1,
      connected: (x, y, direction): boolean =>
        !b.lineCellAtP(direction.add(x * 2 + 1, y * 2 + 1)).fixed,
      belongsToSegment: (_x, _y): boolean => true,
    })

    const endPointCountMap = new Map<number, number>()
    const countUp = (p: Point): void => {
      const n = segmentIds.get((p.x - 1) >> 1, (p.y - 1) >> 1)
      endPointCountMap.set(n, (endPointCountMap.get(n) ?? 0) + 1)
    }

    for (const track of topology.tracks) {
      countUp(track.ends[0])
      countUp(track.ends[1])
    }

    for (const [, count] of endPointCountMap) {
      if (count & 1) {
        throw new HatanError(
          'The number of the incoming tracks in a segment should be even',
        )
      }
    }

    return false
  }
}
