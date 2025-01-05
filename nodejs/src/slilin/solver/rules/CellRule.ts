import { CellType } from '../../data/Cell'
import { SlilinBoard } from '../../data/SlilinBoard'

export interface CellRule {
  readonly cellType: CellType | null
  readonly skipOutside: boolean
  readonly skipUnmodifiedCells: boolean

  apply(b: SlilinBoard, x: number, y: number): boolean
}
