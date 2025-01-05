import { HatanError } from '../../../common/data/HatanError'
import { evalAllOr } from '../../../util/Utils'
import { CellType } from '../../data/Cell'
import { SlilinBoard } from '../../data/SlilinBoard'
import { CellRule } from './CellRule'

export class DotRule implements CellRule {
  readonly cellType: CellType = 'dot'
  readonly skipOutside = false
  readonly skipUnmodifiedCells = true

  public apply(b: SlilinBoard, x: number, y: number): boolean {
    const c = b.surroundedCellAt(x, y)
    const { existsAround, forbiddenAround } = c

    // 点の周辺の線の状態がすべて決まっていれば何もしない
    if (existsAround + forbiddenAround === 4) {
      return false
    }

    if (forbiddenAround === 3 || existsAround === 2) {
      // 3方をxに囲まれているか、線が既に2本あれば、これ以上この点に線は入ってこない
      return evalAllOr(c.unfixedAround, c => c.setForbidden())
    } else if (forbiddenAround === 2 && existsAround === 1) {
      // 一本線が入ってきている状態で、残り候補がひとつなので、そこに線をつなげる
      return c.unfixedAround[0].setExists()
    } else if (existsAround > 2) {
      // 3本以上線が入っていたらおかしい
      throw new HatanError('DotRule: more than two lines coming into a dot')
    } else {
      return false
    }
  }
}
