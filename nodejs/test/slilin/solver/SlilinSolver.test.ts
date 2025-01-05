import assert from 'assert'
import { HatanError } from '../../../src/common/data/HatanError'
import { BoardStateTrial } from '../../../src/common/solver/BoardState'
import { TrackTopology } from '../../../src/common/solver/topology/TrackTopology'
import { SlilinBoard } from '../../../src/slilin/data/SlilinBoard'
import { SlilinFormatter as fmt } from '../../../src/slilin/serializer/SlilinFormatter'
import { SlilinLoader } from '../../../src/slilin/serializer/SlilinLoader'
import {
  SlilinSolver,
  SlilinAssumption,
  SlilinBoardState,
} from '../../../src/slilin/solver/SlilinSolver'
import { range } from '../../../src/util/Utils'

describe('SlilinSolverGachiTest', () => {
  // test('should solve empty', () => {
  //   testForDebug(['+-+-+', '|   |', '+-+-+'])
  // })

  test('should solve sl-0001', () => {
    // 2009年頃の二コリの問題 by Casty氏
    testFast([
      '+-+-+ + + + +-+ + + +',
      '|   |    0 2| |      ',
      '+ +-+ + + +-+ +-+ +-+',
      '|2|3 0    |    2|2|3|',
      '+ +-+ +-+ + +-+ + + +',
      '|   | |3| | |3| | | |',
      '+-+ +-+ +-+ + + +-+ +',
      ' 3|      2 2| |    1|',
      '+-+ +-+-+-+-+ +-+-+ +',
      '|  2|  2     0   2| |',
      '+-+-+ +-+ +-+ +-+ + +',
      '   2  |3| | |3| |3| |',
      '+-+-+-+ +-+ +-+ +-+ +',
      '|3       1 0       2|',
      '+-+ +-+-+ + +-+-+-+-+',
      '  | |  2|   |3       ',
      '+-+ +-+ +-+ +-+ +-+ +',
      '|3 0 3|   |   |3|3|1 ',
      '+-+ +-+ + +-+ +-+ + +',
      '  | |    0 2|     |  ',
      '+ +-+ + + + +-+-+-+ +',
    ])
  })

  test('should solve sl-0007', () => {
    // 2009年頃の二コリの問題 by ももてれう氏
    testFast([
      '+ + + + + + +-+-+-+-+-+-+-+ +-+-+ + + +-+-+-+-+-+',
      ' 1   0   0  |  2   2      |3|   |  0  |  1 1 1 2|',
      '+-+ + +-+ + +-+-+ +-+-+-+ +-+ + +-+ +-+ + + + + +',
      '| |   | |      2| |2   3|      1  | |    1 1 1 1|',
      '+ + +-+ + +-+ + +-+ + +-+ +-+ +-+ +-+ +-+-+-+-+ +',
      '| |2|3 1|3|3|  1   0  |   |3| | |  1  |       | |',
      '+ + +-+ +-+ +-+-+ + +-+ +-+ +-+ +-+ +-+ +-+ +-+ +',
      '| |   |        3|  2|  2|      0  | |2  |3| |  1|',
      '+ +-+-+ +-+ + +-+ +-+ +-+ +-+ + + +-+ + + +-+ + +',
      '|  1 1 1|3|1  |3  |2  |   | |        0  |1      |',
      '+-+ + + + + + +-+ + + + + + +-+ + +-+ +-+ + +-+ +',
      '  |     | |  0  | |   |  1|1 3|1 2| |3|  0  | |3|',
      '+ +-+ +-+ + + +-+ +-+ +-+ + +-+ +-+ +-+ + +-+ +-+',
      ' 0  | |3  |1  |2 0 2|2 3| | |   |    1   2|      ',
      '+ +-+ +-+ + + + + + +-+-+ + +-+-+ +-+ + +-+ + + +',
      '  |    3| |1  |          1|1 1 1 2| |2  |3     0 ',
      '+-+ +-+-+ + +-+ + + + + + + + + +-+ +-+ +-+-+-+ +',
      '|3  |  1  |2|  0 1 1 1 1  |     |  0  |       |  ',
      '+-+ +-+ + + + + +-+-+-+-+-+ +-+-+ + + +-+-+-+ +-+',
      '  |   |2  |3|   |           |2   1   1 1 1 2|2  |',
      '+ +-+ +-+ +-+ +-+ + +-+ + + + + +-+-+-+ + + +-+ +',
      ' 0  |  2|  2  |  0  | |  0  |1  |2    |       | |',
      '+ + +-+ +-+-+ + + +-+ +-+ +-+ + + +-+ +-+ +-+ + +',
      '      |     |3|   |  0  | |  0  |3| |2 2|3|3|3| |',
      '+-+-+ +-+-+ +-+ + +-+ +-+ +-+ + +-+ +-+ +-+ +-+ +',
      '|2 2|2 2  |      0  | |  0  |1   1    |         |',
      '+ + +-+-+-+ +-+ + +-+ +-+ +-+ + + +-+-+ +-+-+-+-+',
      '|2 1 2 2    |3|   |  0  | |  0   1|    2|  1   1 ',
      '+-+-+-+-+-+-+ +-+-+ + + +-+ + + + +-+-+-+ + + + +',
    ])
  })

  test('should solve sl-0008', () => {
    // 2009年頃の二コリの問題 by 339氏
    testFast([
      '+-+-+-+-+ +-+-+ + +-+-+-+-+-+-+-+-+ + + + + +-+-+',
      '|  1 1  | |2  |  1|    1 1     1  |  0     1|2  |',
      '+-+ + + +-+ +-+ + +-+-+ + +-+-+ +-+ + +-+-+ + + +',
      ' 3|    0    |3 0   1 3|   |2 2| |2 1  |  3| |  1|',
      '+-+ + + +-+ +-+ + + +-+ + + + + + +-+-+ +-+ + + +',
      '|3     1| |   |     |     |   | | |     |3  |  1|',
      '+-+-+-+ + + +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +-+ +',
      '   1 3| | |3|   |3|   |3|2  |  0    |3|   |2 2| |',
      '+ + +-+ + +-+ +-+ +-+ +-+ +-+ + +-+-+ + + +-+ + +',
      '    |   |     |3    |2    |3    |2    |     | | |',
      '+-+-+ + +-+ + +-+-+ +-+-+ +-+ +-+ +-+-+ +-+ +-+ +',
      '|2       3|       |2    |   |3|   |    2| |    1|',
      '+ +-+ +-+-+ + + + +-+ +-+ + +-+ + +-+ +-+ +-+ + +',
      '| |3| |2     1 1    | |3 1       0 2| |  0  |2  |',
      '+ + +-+ + +-+-+-+-+-+ +-+-+ + +-+ + + +-+ + +-+-+',
      '| |1   0  |  1 1       1 3|   | |2 1|   |1   1   ',
      '+ + + + +-+ + + +-+ +-+ +-+ + + +-+ +-+ + + + + +',
      '|2|     |3      | |3| | |    1|   |   |2|      0 ',
      '+ +-+ + +-+ + +-+ +-+ + + +-+ + + +-+ + +-+ + + +',
      '|   |     |   |3     2| | |3| |  0  | |   |      ',
      '+ + + +-+-+ + +-+-+ +-+ +-+ + +-+ +-+ +-+ +-+ + +',
      '|  1|3|    0     3| |  0 2  |  3| |  0  |  3|1   ',
      '+ + +-+ + + + + +-+ + + +-+ +-+-+ +-+ + + +-+ +-+',
      '|1     0        |   |   | |         |   |2|   |3|',
      '+ + + + +-+-+ +-+ + +-+ + +-+-+-+ + +-+-+ +-+-+ +',
      '|1     1|   |2|3   0 3| |  1 1  |2 0     0     1|',
      '+ +-+-+ + +-+ +-+-+ +-+ + + + + +-+ +-+ + +-+-+ +',
      '| |2 2| | |3     3| |  2|1     0  | |3|   |2 2| |',
      '+-+ + +-+ +-+-+-+-+ +-+-+ + + + + +-+ +-+-+ + +-+',
    ])
  })
})

export function testFast(boardText: string[]): void {
  const solution = SlilinLoader.loadFull(boardText)
  const board = solution.duplicate(true)

  const solver = new SlilinSolver()
  const result = solver.solve(board)
  expect(result.status).toBe('complete')
  expect(fmt.formatAscii(result.board)).toBe(fmt.formatAscii(solution))
}

export function testForDebug(boardText: string[]): void {
  const solution = SlilinLoader.loadFull(boardText)
  const board = solution.duplicate(true)

  const solver = new SlilinSolver({
    reportProgress(_b: SlilinBoardState, _depth: number) {
      // console.log(`depth=${depth}`)
      // console.log(SlilinSerializer.prettyFormat(b))
    },
    onHatanDetected(b: SlilinBoard, e: HatanError) {
      const comparisonResult = compareBoards(b, solution)
      if (comparisonResult !== 'different') {
        console.log(e)
        console.log(fmt.formatPretty(b))
        assert(false, 'Valid sequence is misjudged as hatan')
      }
    },
    onAssumptionEvaluated(
      boardStateToTry: BoardStateTrial<
        SlilinBoard,
        TrackTopology,
        SlilinAssumption
      >,
    ) {
      console.log(
        `Assumption at ${boardStateToTry.assumption.p.toString()} -> ${boardStateToTry.status}`,
      )
      console.log(fmt.formatPretty(boardStateToTry.board))
    },
  })
  const result = solver.solve(board)
  console.log(fmt.formatPretty(result.board))
  expect(result.status).toBe('complete')
  expect(fmt.formatAscii(result.board)).toBe(fmt.formatAscii(solution))
}

function compareBoards(
  currentBoard: SlilinBoard,
  expectedBoard: SlilinBoard,
): 'idenitcal' | 'onTrack' | 'different' {
  assert(
    currentBoard.cells.elements.length === expectedBoard.cells.elements.length,
  )
  let identical: boolean = true
  let onTrack: boolean = true
  for (const y of range(1, expectedBoard.physicalHeight - 1)) {
    for (const x of range(1 + (y & 1), expectedBoard.physicalWidth - 1, 2)) {
      const c1 = currentBoard.lineCellAt(x, y)
      const c2 = expectedBoard.lineCellAt(x, y)
      if (c1.exists !== c2.exists) {
        identical = false
      }
      if (c1.exists && !c2.exists) {
        onTrack = false
      }
    }
  }
  return identical ? 'idenitcal' : onTrack ? 'onTrack' : 'different'
}
