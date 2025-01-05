import { Point } from '../../../util/Point'
import { LineCell } from '../../data/Cell'
import { SlilinBoard } from '../../data/SlilinBoard'

export interface LineStats {
  forbidden: number
  exists: number
  unfixedCells: LineCell[]
}

export const LineStats = {
  at,
  around,
} as const

function at(b: SlilinBoard, points: Point[]): LineStats {
  const stats: LineStats = {
    forbidden: 0,
    exists: 0,
    unfixedCells: [],
  }
  for (const p of points) {
    const c = b.lineCellAtP(p)
    if (c.forbidden) {
      stats.forbidden++
    } else if (c.exists) {
      stats.exists++
    } else {
      stats.unfixedCells.push(b.lineCellAt(p.x, p.y))
    }
  }
  return stats
}

// performant version of at(b, Point.adjacent4(x, y))
function around(b: SlilinBoard, x: number, y: number): LineStats {
  const cells = [
    b.lineCellAt(x - 1, y),
    b.lineCellAt(x + 1, y),
    b.lineCellAt(x, y - 1),
    b.lineCellAt(x, y + 1),
  ]
  return {
    forbidden:
      Number(cells[0].forbidden) +
      Number(cells[1].forbidden) +
      Number(cells[2].forbidden) +
      Number(cells[3].forbidden),
    exists:
      Number(cells[0].exists) +
      Number(cells[1].exists) +
      Number(cells[2].exists) +
      Number(cells[3].exists),
    unfixedCells: cells.filter(c => !c.fixed),
  }
}
