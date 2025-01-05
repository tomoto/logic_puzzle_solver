import { TrackAnalysisSubject } from '../../../common/solver/topology/TrackAnalyzer'
import { Array2D } from '../../../util/Array2D'
import { Point } from '../../../util/Point'
import { Optional } from '../../../util/Utils'
import { Directions } from '../../../util/Vector'
import { SlilinBoard } from '../../data/SlilinBoard'

export class SlilinTrackAnalysisSubject implements TrackAnalysisSubject {
  private x: number = 1
  private y: number = 1
  private readonly visitedFlags: Array2D<boolean>

  constructor(readonly board: SlilinBoard) {
    this.visitedFlags = new Array2D<boolean>(
      this.board.physicalWidth,
      this.board.physicalHeight,
    )
  }

  findNextNode(): Optional<Point> {
    while (this.y < this.board.physicalHeight) {
      const p = Point.at(this.x, this.y)
      this.x += 2
      if (this.x >= this.board.physicalWidth) {
        this.x = 1
        this.y += 2
      }

      if (this.board.surroundedCellAt(p.x, p.y).existsAround > 0) {
        return p
      }
    }
    return undefined
  }

  navigateOverUnvisitedEdgeFrom(p: Point): Optional<Point> {
    for (const d of Directions[4]) {
      const x = p.x + d.x
      const y = p.y + d.y
      if (!this.visitedFlags.get(x, y) && this.board.lineCellAt(x, y).exists) {
        this.visitedFlags.set(x, y, true)
        return Point.at(x + d.x, y + d.y)
      }
    }
    return undefined
  }
}
