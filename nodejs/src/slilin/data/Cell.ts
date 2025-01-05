import assert from 'assert'
import { HatanError } from '../../common/data/HatanError'
import { range } from '../../util/Utils'
import { Directions } from '../../util/Vector'
import { SlilinBoard } from './SlilinBoard'

export class Cell {
  constructor(
    readonly board: SlilinBoard,
    readonly x: number,
    readonly y: number,
  ) {}

  copyFrom(_other: Cell): void {}

  get dirty(): boolean {
    return this.board.dirtyFlags.cell.get(this.x, this.y)
  }
}

export class SurroundedCell extends Cell {
  readonly i: number

  constructor(board: SlilinBoard, x: number, y: number) {
    super(board, x, y)
    this.i = x + y * board.surroundingStats.exists.width
  }

  get existsAround(): number {
    return this.board.surroundingStats.exists.elements[this.i]
  }

  addToExistsAround(n: number): void {
    this.board.surroundingStats.exists.elements[this.i] += n
  }

  get forbiddenAround(): number {
    return this.board.surroundingStats.forbidden.elements[this.i]
  }

  addToForbiddenAround(n: number): void {
    this.board.surroundingStats.forbidden.elements[this.i] += n
  }

  get unfixedAround(): LineCell[] {
    return Directions[4]
      .map(d => this.board.lineCellAt(this.x + d.x, this.y + d.y))
      .filter(c => !c.fixed)
  }
}

export class DotCell extends SurroundedCell {
  toString(): string {
    return '.'
  }
}

export type NumberCellValue = null | 0 | 1 | 2 | 3

export class NumberCell extends SurroundedCell implements Cell {
  get value(): NumberCellValue {
    return this.board.numbers.get(this.x >> 1, this.y >> 1)
  }

  set value(n: NumberCellValue) {
    this.board.numbers.set(this.x >> 1, this.y >> 1, n)
  }

  toString(): string {
    return (this.value ?? ' ').toString()
  }
}

export class LineCell extends Cell {
  assumed: boolean = false

  private _exists: boolean = false
  private _forbidden: boolean = false

  get exists(): boolean {
    return this._exists
  }

  set exists(value: boolean) {
    this.setExists(value)
  }

  get forbidden(): boolean {
    return this._forbidden
  }

  set forbidden(value: boolean) {
    this.setForbidden(value)
  }

  get fixed(): boolean {
    return this._exists || this._forbidden
  }

  setExists(value: boolean = true): boolean {
    if (this._exists === value) {
      return false
    } else {
      if (value && this._forbidden) {
        throw new HatanError('Cannot set exists to an forbidden line')
      }
      this._exists = value
      this.dirtify()
      this.updateExistsOnNeighbors(value)
      return true
    }
  }

  setForbidden(value: boolean = true): boolean {
    if (this._forbidden === value) {
      return false
    } else {
      if (value && this._exists) {
        throw new HatanError('Cannot set forbidden to an existing line')
      }
      this._forbidden = value
      this.dirtify()
      this.updateForbiddenOnNeighbors(value)
      return true
    }
  }

  private updateExistsOnNeighbors(value: boolean): void {
    const d = value ? 1 : -1
    if (this.y > 0) {
      this.board.surroundedCellAt(this.x, this.y - 1).addToExistsAround(d)
    }
    if (this.y < this.board.physicalHeight - 1) {
      this.board.surroundedCellAt(this.x, this.y + 1).addToExistsAround(d)
    }
    if (this.x > 0) {
      this.board.surroundedCellAt(this.x - 1, this.y).addToExistsAround(d)
    }
    if (this.x < this.board.physicalWidth - 1) {
      this.board.surroundedCellAt(this.x + 1, this.y).addToExistsAround(d)
    }
  }

  private updateForbiddenOnNeighbors(value: boolean): void {
    const d = value ? 1 : -1
    if (this.y > 0) {
      this.board.surroundedCellAt(this.x, this.y - 1).addToForbiddenAround(d)
    }
    if (this.y < this.board.physicalHeight - 1) {
      this.board.surroundedCellAt(this.x, this.y + 1).addToForbiddenAround(d)
    }
    if (this.x > 0) {
      this.board.surroundedCellAt(this.x - 1, this.y).addToForbiddenAround(d)
    }
    if (this.x < this.board.physicalWidth - 1) {
      this.board.surroundedCellAt(this.x + 1, this.y).addToForbiddenAround(d)
    }
  }

  private dirtify(): void {
    const top = Math.max(this.y - 2, 0)
    const bottom = Math.min(this.y + 2, this.board.physicalHeight - 1)
    const left = Math.max(this.x - 2, 0)
    const right = Math.min(this.x + 2, this.board.physicalWidth - 1)
    for (const y of range(top, bottom + 1)) {
      this.board.dirtyFlags.line[y] = true
      this.board.dirtyFlagsBuffer.line[y] = true
      for (const x of range(left, right + 1)) {
        this.board.dirtyFlags.cell.set(x, y, true)
        this.board.dirtyFlagsBuffer.cell.set(x, y, true)
      }
    }
  }

  copyFrom(other: Cell): void {
    assert(other instanceof LineCell)
    this._exists = other._exists
    this._forbidden = other._forbidden
    this.assumed = other.assumed
  }

  toString(): string {
    return this.exists ? this.lineSymbol() : this.forbidden ? 'x' : ' '
  }

  protected lineSymbol(): string {
    throw new Error('Not implemented')
  }
}

export class HLineCell extends LineCell implements LineCell {
  protected lineSymbol(): string {
    return '-'
  }
}

export class VLineCell extends LineCell implements LineCell {
  protected lineSymbol(): string {
    return '|'
  }
}

export type CellType = 'number' | 'dot' | 'vline' | 'hline'
export type CellClass =
  | typeof NumberCell
  | typeof DotCell
  | typeof HLineCell
  | typeof VLineCell

export const CellClasses = {
  number: NumberCell,
  dot: DotCell,
  hline: HLineCell,
  vline: VLineCell,
} as const
