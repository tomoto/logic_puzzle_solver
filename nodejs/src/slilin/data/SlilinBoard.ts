import assert from 'assert'
import { Array2D } from '../../util/Array2D'
import { Point } from '../../util/Point'
import { range } from '../../util/Utils'
import {
  Cell,
  NumberCell,
  NumberCellValue,
  CellClasses,
  LineCell,
  SurroundedCell,
} from './Cell'
import { SlilinHelper } from './SlilinHelper'

export class SlilinBoard {
  readonly logicalWidth: number
  readonly logicalHeight: number
  readonly physicalWidth: number
  readonly physicalHeight: number

  /**
   * セル状態。下記のような配列になっている。一番外側の周囲は番人。
   *  x x x x
   * x.-.-.-.x
   *  |N|N|N|
   * x|-.-.-.x
   *  |N|N|N|
   * x|-.-.-.x
   *  |N|N|N|
   * x.-.-.-.x
   *  x x x x
   */
  readonly cells: Array2D<Cell>

  // 数字(解決中に変化しない)
  readonly numbers: Array2D<NumberCellValue>

  // 数字またはドットの周囲の線の状態
  readonly surroundingStats: {
    exists: Array2D<number> // そのセルの周囲に存在する線の数
    forbidden: Array2D<number> // そのセルの周囲に存在する禁止された線の数
  }

  // セルが最近変更されたかどうか
  // ダブルバッファリングされている
  dirtyFlags: {
    cell: Array2D<boolean>
    line: boolean[]
  }
  dirtyFlagsBuffer: {
    cell: Array2D<boolean>
    line: boolean[]
  }

  constructor(w: number, h: number) {
    this.logicalWidth = w
    this.logicalHeight = h
    this.physicalWidth = w * 2 + 3
    this.physicalHeight = h * 2 + 3

    this.numbers = new Array2D(
      (this.physicalWidth >> 1) + 1,
      (this.physicalHeight >> 1) + 1,
      null,
    )
    this.surroundingStats = {
      exists: new Array2D(this.physicalWidth, this.physicalHeight, 0),
      forbidden: new Array2D(this.physicalWidth, this.physicalHeight, 0),
    }

    this.cells = new Array2D(this.physicalWidth, this.physicalHeight)
    this.dirtyFlags = {
      cell: new Array2D(this.physicalWidth, this.physicalHeight, true),
      line: new Array(this.physicalHeight).fill(true),
    }
    this.dirtyFlagsBuffer = {
      cell: new Array2D(this.physicalWidth, this.physicalHeight, true),
      line: new Array(this.physicalHeight).fill(true),
    }

    for (const y of range(0, this.physicalHeight)) {
      for (const x of range(0, this.physicalWidth)) {
        this.cells.set(
          x,
          y,
          new CellClasses[SlilinHelper.cellTypeAt(x, y)](this, x, y),
        )
      }
    }

    // initialize sentinels
    for (const x of range(1, this.physicalWidth, 2)) {
      this.lineCellAt(x, 0).forbidden = true
      this.lineCellAt(x, this.physicalHeight - 1).forbidden = true
    }
    for (const y of range(1, this.physicalHeight, 2)) {
      this.lineCellAt(0, y).forbidden = true
      this.lineCellAt(this.physicalWidth - 1, y).forbidden = true
    }
  }

  numberCellAt(x: number, y: number): NumberCell {
    const c = this.cells.get(x, y)
    // assert(c instanceof NumberCell)
    return c as NumberCell
  }

  numberAt(x: number, y: number): NumberCellValue {
    return this.numbers.get(x >> 1, y >> 1)
  }

  numberAtP(p: Point): NumberCellValue {
    return this.numberAt(p.x, p.y)
  }

  surroundedCellAt(x: number, y: number): SurroundedCell {
    const c = this.cells.get(x, y)
    assert(c instanceof SurroundedCell)
    return c as SurroundedCell
  }

  lineCellAt(x: number, y: number): LineCell {
    const c = this.cells.get(x, y)
    // assert(c instanceof AbstractLineCell)
    return c as LineCell
  }

  lineCellAtP(p: Point): LineCell {
    return this.lineCellAt(p.x, p.y)
  }

  toString(): string {
    return this.cells
      .slices()
      .map(line =>
        line
          .map(c => c.toString())
          .concat(['\n'])
          .join(''),
      )
      .join('')
  }

  calcFingerprint(): bigint {
    let fp = BigInt(0)
    const one = BigInt(1)
    for (const i of range(
      this.physicalWidth,
      this.physicalWidth * (this.physicalHeight - 1),
      2,
    )) {
      const c = this.cells.elements[i]
      assert(c instanceof LineCell)
      fp <<= one
      c.exists && (fp |= one)
    }
    return fp
  }

  duplicate(numberOnly = false): SlilinBoard {
    const b = new SlilinBoard(this.logicalWidth, this.logicalHeight)

    b.numbers.copyFrom(this.numbers)

    if (!numberOnly) {
      // 線の状態をコピー
      this.cells.elements.forEach((c, i) => {
        b.cells.elements[i].copyFrom(c)
      })

      // その他の状態をコピー
      b.surroundingStats.exists.copyFrom(this.surroundingStats.exists)
      b.surroundingStats.forbidden.copyFrom(this.surroundingStats.forbidden)
      b.dirtyFlags.cell.copyFrom(this.dirtyFlags.cell)
      b.dirtyFlags.line = this.dirtyFlags.line.slice()
      b.dirtyFlagsBuffer.cell.copyFrom(this.dirtyFlagsBuffer.cell)
      b.dirtyFlags.line = this.dirtyFlags.line.slice()
    }
    return b
  }
}
