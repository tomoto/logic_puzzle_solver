import { HatanError } from '../../../common/data/HatanError'
import { or } from '../../../util/Utils'
import { Directions, Vector } from '../../../util/Vector'
import { CellType } from '../../data/Cell'
import { SlilinBoard } from '../../data/SlilinBoard'
import { CellRule } from './CellRule'

export class NumberCornerRule implements CellRule {
  readonly cellType: CellType = 'number'
  readonly skipOutside = true
  readonly skipUnmodifiedCells = true

  public apply(b: SlilinBoard, x: number, y: number): boolean {
    // 数字の周りの線がすべて確定していれば何もしない
    if (Directions[4].every(v => b.lineCellAt(x + v.x, y + v.y).fixed)) {
      return false
    }

    const n = b.numberAt(x, y)

    const f =
      n === null ? processEmpty : [null, processOne, null, processThree][n]

    if (f) {
      const p = new Vector(x, y)
      return or(...Directions[4].map(v => f(b, p, v)))
    } else {
      return false
    }
  }
}

function processEmpty(b: SlilinBoard, p0: Vector, v0: Vector): boolean {
  const v1 = v0.lrot()
  const pc = p0.addP(v0).addP(v1)
  const p1 = pc.addP(v0)
  const p2 = pc.addP(v1)
  const lc1 = b.lineCellAt(p1.x, p1.y)
  const lc2 = b.lineCellAt(p2.x, p2.y)

  return (
    lc1.forbidden &&
    lc2.forbidden &&
    or(
      b.lineCellAtP(p0.addP(v1, -1)).exists &&
        b.lineCellAtP(p0.addP(v0, -1)).exists &&
        or(
          b.lineCellAtP(p0.addP(v0)).setForbidden(),
          b.lineCellAtP(p0.addP(v1)).setForbidden(),
        ),
      b.lineCellAtP(p1.addP(v1, -2)).forbidden &&
        b.lineCellAtP(p2.addP(v1, -4)).forbidden &&
        b.lineCellAtP(p1.addP(v0, -4)).forbidden &&
        b.lineCellAtP(p2.addP(v0, -2)).forbidden &&
        or(
          b.lineCellAtP(p0.addP(v0)).setForbidden(),
          b.lineCellAtP(p0.addP(v1)).setForbidden(),
          b.lineCellAtP(p0.addP(v0, -1)).setForbidden(),
          b.lineCellAtP(p0.addP(v1, -1)).setForbidden(),
        ),
    )
  )
}

function processOne(b: SlilinBoard, p0: Vector, v0: Vector): boolean {
  const v1 = v0.lrot()
  const pc = p0.addP(v0).addP(v1)
  const p1 = pc.addP(v0)
  const p2 = pc.addP(v1)
  const lc1 = b.lineCellAtP(p1)
  const lc2 = b.lineCellAtP(p2)

  return (
    lc1.forbidden &&
    lc2.forbidden &&
    or(
      b.lineCellAtP(p0.addP(v0)).setForbidden(),
      b.lineCellAtP(p0.addP(v1)).setForbidden(),
    )
  )
}

function processThree(b: SlilinBoard, p0: Vector, v0: Vector): boolean {
  return processThreeInternal(b, p0, p0, v0)
}

function processThreeInternal(
  b: SlilinBoard,
  p0: Vector,
  p: Vector,
  v0: Vector,
): boolean {
  const v1 = v0.lrot()
  const pc = p.addP(v0).addP(v1)
  const p1 = pc.addP(v0)
  const p2 = pc.addP(v1)
  const lc1 = b.lineCellAtP(p1)
  const lc2 = b.lineCellAtP(p2)

  if (lc1.forbidden && lc2.forbidden) {
    return or(
      b.lineCellAtP(p0.addP(v0)).setExists(),
      b.lineCellAtP(p0.addP(v1)).setExists(),
    )
  } else if (lc1.exists && lc2.exists) {
    throw new HatanError(
      'NumberCornerRule: the constraints are broken around 3',
    )
  } else if (lc1.exists || lc2.exists) {
    return or(
      b.lineCellAtP(p0.addP(v0, -1)).setExists(),
      b.lineCellAtP(p0.addP(v1, -1)).setExists(),
    )
  } else {
    p = p1.addP(v1)
    return b.numberAtP(p) === 2 && processThreeInternal(b, p0, p, v0)
  }
}
