import { Point } from '../../util/Point'
import { CellType } from './Cell'

export const SlilinHelper = {
  cellTypeAt,
  physicalPointFromLogical,
} as const

const cellTypeTable = ['number', 'vline', 'hline', 'dot'] as const

function cellTypeAt(x: number, y: number): CellType {
  return cellTypeTable[(x & 1) + (y & 1) * 2]
}

function physicalPointFromLogical(x: number, y: number): Point {
  return { x: x * 2 + 2, y: y * 2 + 2 }
}
