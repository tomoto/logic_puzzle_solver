import { Array2D } from '../../../util/Array2D'
import { range } from '../../../util/Utils'
import { Direction, Vector } from '../../../util/Vector'

export const SegmentAnalyzer = {
  analyze4Way,
} as const

export interface SegmentAnalysisSubject {
  width: number
  height: number
  connected(x: number, y: number, direction: Vector): boolean
  belongsToSegment(x: number, y: number): boolean
}

export class SegmentAnalysisResult {
  segmentIds: Array2D<number>

  constructor(
    readonly width: number,
    readonly height: number,
  ) {
    this.segmentIds = new Array2D(width, height)
  }
}

function analyze4Way(g: SegmentAnalysisSubject): SegmentAnalysisResult {
  const result = new SegmentAnalysisResult(g.width, g.height)
  let segmentId = 1
  for (const y of range(0, g.height)) {
    for (const x of range(0, g.width)) {
      if (g.belongsToSegment(x, y)) {
        const isNorthConnected = y > 0 && g.connected(x, y, Direction.N)
        const isWestConnected = x > 0 && g.connected(x, y, Direction.W)
        const northId = isNorthConnected ? result.segmentIds.get(x, y - 1) : 0
        const westId = isWestConnected ? result.segmentIds.get(x - 1, y) : 0

        let n: number
        if (isNorthConnected) {
          n = northId
          if (isWestConnected && northId !== westId) {
            replaceSegmentIds(result, westId, northId, x, y)
          }
        } else {
          if (isWestConnected) {
            n = westId
          } else {
            n = segmentId++
          }
        }
        result.segmentIds.set(x, y, n)
      }
    }
  }
  return result
}

function replaceSegmentIds(
  result: SegmentAnalysisResult,
  from: number,
  to: number,
  x: number,
  y: number,
): void {
  for (let i = 0; i < x + y * result.width; i++) {
    if (result.segmentIds.elements[i] === from) {
      result.segmentIds.elements[i] = to
    }
  }
}
