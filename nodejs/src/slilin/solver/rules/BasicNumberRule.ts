import { HatanError } from '../../../common/data/HatanError'
import { evalAllOr } from '../../../util/Utils'
import { CellType } from '../../data/Cell'
import { SlilinBoard } from '../../data/SlilinBoard'
import { CellRule } from './CellRule'

export class BasicNumberRule implements CellRule {
  readonly cellType: CellType = 'number'
  readonly skipOutside = true
  readonly skipUnmodifiedCells = true

  public apply(b: SlilinBoard, x: number, y: number): boolean {
    const c = b.numberCellAt(x, y)
    const n = c.value

    // 数値が入っていなければ何もしない
    if (n === null) {
      return false
    }

    // 周辺の線の状態を調べる
    const { existsAround, forbiddenAround } = c

    // すべての線が決まっていれば何もしない
    if (existsAround + forbiddenAround === 4) {
      return false
    }

    if (existsAround === n) {
      // 線の数が指定に達したので、空いている箇所を禁止で確定する
      return evalAllOr(c.unfixedAround, line => line.setForbidden())
    } else if (4 - forbiddenAround === n) {
      // 禁止の数が指定に達したので、空いている箇所を線で確定する
      return evalAllOr(c.unfixedAround, line => line.setExists())
    } else if (existsAround > n || forbiddenAround > 4 - n) {
      // 線の数がおかしい
      throw new HatanError(
        'BasicNumberRule: the number of lines around a number does not satisfy the constraint',
      )
    } else {
      return false
    }
  }
}
