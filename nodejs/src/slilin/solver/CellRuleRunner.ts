import _ from 'lodash'
import { Rule } from '../../common/solver/SolverEngine'
import { BoolHolder } from '../../util/BoolHolder'
import { SlilinBoard } from '../data/SlilinBoard'
import { CellRule } from './rules/CellRule'

export class CellRuleRunner implements Rule<SlilinBoard> {
  private rule: CellRule

  constructor(rule: CellRule) {
    this.rule = rule
  }

  public apply(b: SlilinBoard): boolean {
    const r = this.rule
    const [xrange, yrange] = calcRange(b, r)

    const changed = new BoolHolder()
    for (const y of yrange) {
      if (r.skipUnmodifiedCells) {
        if (b.dirtyFlags.line[y]) {
          for (const x of xrange) {
            if (b.dirtyFlags.cell.get(x, y)) {
              changed.accumulate(r.apply(b, x, y))
            }
          }
        }
      } else {
        for (const x of xrange) {
          changed.accumulate(r.apply(b, x, y))
        }
      }
    }
    return changed.value
  }
}

const cellTypeRangeMetricsMap = {
  number: { xmin: 0, ymin: 0, step: 2 },
  dot: { xmin: 1, ymin: 1, step: 2 },
  hline: { xmin: 0, ymin: 1, step: 2 },
  vline: { xmin: 1, ymin: 0, step: 2 },
  null: { xmin: 0, ymin: 0, step: 1 },
} as const

function calcRange(b: SlilinBoard, rule: CellRule): [number[], number[]] {
  // eslint-disable-next-line prefer-const
  let { xmin, ymin, step } = cellTypeRangeMetricsMap[rule.cellType ?? 'null']

  let xmax = b.physicalWidth
  let ymax = b.physicalHeight
  if (rule.skipOutside) {
    if (xmin === 0) {
      xmin += 2
      xmax -= 1
    }
    if (ymin === 0) {
      ymin += 2
      ymax -= 1
    }
  }

  return [_.range(xmin, xmax, step), _.range(ymin, ymax, step)]
}
