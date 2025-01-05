import { bgBlue, greenBright, red, white, yellow } from 'ansi-colors'
import _ from 'lodash'
import { throwError } from '../../util/Utils'
import {
  Cell,
  DotCell,
  HLineCell,
  LineCell,
  NumberCell,
  VLineCell,
} from '../data/Cell'
import { SlilinBoard } from '../data/SlilinBoard'

export interface CellVisitor<R> {
  number(cell: NumberCell, b: SlilinBoard, x: number, y: number): R
  dot(cell: DotCell, b: SlilinBoard, x: number, y: number): R
  hline(cell: HLineCell, b: SlilinBoard, x: number, y: number): R
  vline(cell: VLineCell, b: SlilinBoard, x: number, y: number): R
}

function formatCell(
  f: CellVisitor<string>,
  b: SlilinBoard,
  x: number,
  y: number,
): string {
  const c = b.cells.get(x, y)
  return c instanceof NumberCell
    ? f.number(c, b, x, y)
    : c instanceof DotCell
      ? f.dot(c, b, x, y)
      : c instanceof HLineCell
        ? f.hline(c, b, x, y)
        : c instanceof VLineCell
          ? f.vline(c, b, x, y)
          : throwError()
}

function format(
  b: SlilinBoard,
  f: CellVisitor<string>,
  newline = '\n',
): string {
  return _.range(1, b.physicalHeight - 1)
    .map(y =>
      _.range(1, b.physicalWidth - 1)
        .map(x => formatCell(f, b, x, y))
        .join(''),
    )
    .join(newline)
}

function formatPretty(
  b: SlilinBoard,
  opt?: {
    showDirty?: boolean
  },
): string {
  const formatLine = (c: LineCell, s: string): string =>
    c.exists
      ? (c.assumed ? yellow : greenBright).bold(s)
      : c.forbidden
        ? red.dim('x')
        : ' '
  const formatDirty: (c: Cell, s: string) => string = opt?.showDirty
    ? (c, s) =>
        b.dirtyFlags.cell.elements.some(e => !e) &&
        b.dirtyFlags.cell.get(c.x, c.y)
          ? bgBlue(s)
          : s
    : (c, s) => s

  return format(b, {
    number: c => formatDirty(c, white.bold(c.toString())),
    dot: c => formatDirty(c, white.bold('\u00b7')),
    hline: c => formatDirty(c, formatLine(c, '\u2501')),
    vline: c => formatDirty(c, formatLine(c, '\u2503')),
  })
}

function formatAscii(b: SlilinBoard): string {
  return format(b, {
    number: c => c.toString(),
    dot: () => '+',
    hline: c => (c.exists ? '-' : ' '),
    vline: c => (c.exists ? '|' : ' '),
  })
}

export const SlilinFormatter = {
  format,
  formatPretty,
  formatAscii,
}
