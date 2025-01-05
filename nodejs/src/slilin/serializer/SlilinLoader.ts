import assert from 'assert'
import { NumberCellValue } from '../data/Cell'
import { SlilinBoard } from '../data/SlilinBoard'
import { SlilinHelper } from '../data/SlilinHelper'

/**
 * 以下のような文字列配列を読み込んでSlilinBoardを生成する。
 * 数字のマスを表すには0-3の数字を使う。空白のマスを表すにはそれ以外の文字を使う。
 *
 * [
 *   '----02----',
 *   '230----223',
 *   '---3--3---',
 *   '3---22---1',
 *   '-2-2--0-2-',
 *   '-2-3--3-3-',
 *   '3---10---2',
 *   '---2--3---',
 *   '303----331',
 *   '----02----',
 * ]
 *
 * @param text
 * @returns
 */
function loadCompact(text: string[]): SlilinBoard {
  const h = text.length
  const w = text[0].length
  const b = new SlilinBoard(w, h)
  text.forEach((line, y) => {
    line.split('').forEach((c, x) => {
      const p = SlilinHelper.physicalPointFromLogical(x, y)
      const n = parseInt(c)
      b.numberCellAt(p.x, p.y).value =
        n >= 0 && n <= 3 ? (n as NumberCellValue) : null
    })
  })
  return b
}

/**
 * 以下のような文字列配列を読み込んでSlilinBoardを生成する。
 * つまりasciiFormatの逆。
 * 
 * [
      '+ + + + + + +-+-+-+-+-+-+-+ +-+-+ + + +-+-+-+-+-+',
      ' 1   0   0  |  2   2      |3|   |  0  |  1 1 1 2|',
      '+-+ + +-+ + +-+-+ +-+-+-+ +-+ + +-+ +-+ + + + + +',
      '| |   | |      2| |2   3|      1  | |    1 1 1 1|',
      '+ + +-+ + +-+ + +-+ + +-+ +-+ +-+ +-+ +-+-+-+-+ +',
      '| |2|3 1|3|3|  1   0  |   |3| | |  1  |       | |',
      '+ + + + +-+ +-+-+ + +-+ +-+ +-+ +-+ +-+ +-+ +-+ +',
      ...
* ]
 */
function loadFull(text: string[]): SlilinBoard {
  const h = text.length >> 1
  const w = text[0].length >> 1
  const b = new SlilinBoard(w, h)
  text.forEach((line, y) => {
    line.split('').forEach((c, x) => {
      switch (c) {
        case '-':
        case '|':
          b.lineCellAt(x + 1, y + 1).setExists()
          break
        case 'x':
          b.lineCellAt(x + 1, y + 1).setForbidden()
          break
        case '0':
        case '1':
        case '2':
        case '3':
          b.numberCellAt(x + 1, y + 1).value = parseInt(c) as NumberCellValue
          break
        case '+':
          assert(
            SlilinHelper.cellTypeAt(x + 1, y + 1) === 'dot',
            `Invalid '+' at (${x + 1}, ${y + 1})`,
          )
          break
      }
    })
  })
  return b
}

export const SlilinLoader = {
  loadCompact,
  loadFull,
} as const
