import { or } from '../../../util/Utils'
import { Direction, Vector } from '../../../util/Vector'
import { CellType } from '../../data/Cell'
import { SlilinBoard } from '../../data/SlilinBoard'
import { CellRule } from './CellRule'

export class ThreePairInitialRule implements CellRule {
  readonly cellType: CellType = 'number'
  readonly skipOutside = true
  readonly skipUnmodifiedCells = false

  public apply(b: SlilinBoard, x: number, y: number): boolean {
    if (b.numberAt(x, y) !== 3) {
      return false
    }

    const p = new Vector(x, y)

    return or(
      processDirectNeighbor(b, p, Direction.E),
      processDirectNeighbor(b, p, Direction.S),
      processDiagonalNeighbor(b, p, Direction.NE, Direction.S),
      processDiagonalNeighbor(b, p, Direction.SE, Direction.W),
    )
  }
}

/**
 * 隣接する3のパターン(下記)を処理する
 *
 *   x
 * . . .
 * |3|3|
 * . . .
 *   x
 */
function processDirectNeighbor(b: SlilinBoard, p0: Vector, v: Vector): boolean {
  // 隣接するマスが3でなければ何もしない
  if (b.numberAtP(p0.addP(v, 2)) !== 3) {
    return false
  }

  // 線とバツを書き込む
  const p1 = p0.addP(v)
  return or(
    b.lineCellAtP(p0.addP(v, -1)).setExists(),
    b.lineCellAtP(p1).setExists(),
    b.lineCellAtP(p1.addP(v, 2)).setExists(),
    b.lineCellAtP(p1.addP(v.lrot(), 2)).setForbidden(),
    b.lineCellAtP(p1.addP(v.rrot(), 2)).setForbidden(),
  )
}

/**
 * 対角に接する3のパターン(下記)を処理する
 * - 間に2を挟んでもよい。
 *
 * . .-.
 *    3|
 * . . .
 * |3
 * .-. .
 *
 * @param b
 * @param p0
 * @param v
 * @param w
 * @returns
 */
function processDiagonalNeighbor(
  b: SlilinBoard,
  p0: Vector,
  v: Vector,
  w: Vector,
): boolean {
  // 対角の3を探す
  let p1 = p0
  while (true) {
    p1 = p1.addP(v, 2)
    switch (b.numberAtP(p1)) {
      case 2:
        continue // 2のマスがあれば次のマスを調べる
      case 3:
        break // 3のマスがあればそれが対角の3
      default:
        return false // 2や3以外に出会ったら条件不成立
    }
    break
  }

  // 線とバツを書き込む
  return or(
    b.lineCellAtP(p0.addP(w)).setExists(),
    b.lineCellAtP(p0.addP(w.rrot())).setExists(),
    b.lineCellAtP(p1.addP(w, -1)).setExists(),
    b.lineCellAtP(p1.addP(w.lrot())).setExists(),
  )
}
