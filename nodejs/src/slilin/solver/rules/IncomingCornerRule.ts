import { BoolHolder } from '../../../util/BoolHolder'
import { Point } from '../../../util/Point'
import { or, evalAllOr } from '../../../util/Utils'
import { Directions, Vector } from '../../../util/Vector'
import { CellType } from '../../data/Cell'
import { SlilinBoard } from '../../data/SlilinBoard'
import { CellRule } from './CellRule'

export class IncomingCornerRule implements CellRule {
  readonly cellType: CellType = 'number'
  readonly skipOutside = false
  readonly skipUnmodifiedCells = true

  public apply(b: SlilinBoard, x: number, y: number): boolean {
    const changed = new BoolHolder()

    const p0 = new Vector(x, y)
    changed.accumulate(processGeneral(b, p0))

    const f = numberSpecificProcesses[b.numberAt(x, y) ?? 0]

    if (f) {
      changed.accumulate(
        or(...Directions[4].map(v => f.apply(this, [b, p0, v]))),
      )
    }

    return changed.value
  }
}

const numberSpecificProcesses = [null, process1, process2, process3]

function processGeneral(b: SlilinBoard, p0: Vector): boolean {
  const cells = Point.adjacent4(p0.x, p0.y).map(p =>
    Point.inRange(p.x, p.y, 1, b.physicalWidth - 1, 1, b.physicalHeight - 1)
      ? b.lineCellAtP(p)
      : null,
  )
  return evalAllOr(cells, (c1, i) => {
    const c2 = cells[(i + 3) % 4]
    const v = Directions[4][i]
    return (
      !!c1 &&
      !!c2 &&
      ((c1.forbidden && c2.exists) || (c1.exists && c2.forbidden)) &&
      processIncoming(b, p0, v)
    )
  })
}

function process1(b: SlilinBoard, p0: Vector, v: Vector): boolean {
  const s1 = b.lineCellAtP(p0.addP(v, -1))
  const s2 = b.lineCellAtP(p0.addP(v.rrot()))
  return s1.forbidden && s2.forbidden && processIncoming(b, p0, v)
}

function process2(b: SlilinBoard, p0: Vector, v0: Vector): boolean {
  const vl = v0.lrot()
  const vr = v0.rrot()
  const vi = v0.mul(-1)

  const f = (v1: Vector, v2: Vector): boolean => {
    const s1 = b.lineCellAtP(p0.addP(v1, 2).addP(v2))
    const s2 = b.lineCellAtP(p0.addP(v2, 2).addP(v1))
    return s1.forbidden && s2.forbidden && processIncoming(b, p0, v0)
  }

  return f(vi, vl) || f(v0, vr)
}

function process3(b: SlilinBoard, p0: Vector, v: Vector): boolean {
  const s1 = b.lineCellAtP(p0.addP(v, -1))
  const s2 = b.lineCellAtP(p0.addP(v.rrot()))
  return s1.exists && s2.exists && processIncoming(b, p0, v)
}

function processIncoming(b: SlilinBoard, p0: Vector, v: Vector): boolean {
  const p1 = p0.addP(v, 2).addP(v.lrot(), 2)

  const c1 = b.lineCellAtP(p1.addP(v, -1))
  const c2 = b.lineCellAtP(p1.addP(v.rrot()))

  const changed = new BoolHolder()
  changed.accumulate(
    c1.forbidden
      ? c2.setExists()
      : c2.forbidden
        ? c1.setExists()
        : c1.exists
          ? c2.setForbidden()
          : c2.exists
            ? c1.setForbidden()
            : false,
  )
  changed.accumulate(() => {
    switch (b.numberAtP(p1)) {
      case 1:
        return or(
          b.lineCellAtP(p1.addP(v)).setForbidden(),
          b.lineCellAtP(p1.addP(v.lrot())).setForbidden(),
        )
      case 2:
        return processIncoming(b, p1, v)
      case 3:
        return or(
          b.lineCellAtP(p1.addP(v)).setExists(),
          b.lineCellAtP(p1.addP(v.lrot())).setExists(),
        )
      default:
        return false
    }
  })
  return changed.value
}
