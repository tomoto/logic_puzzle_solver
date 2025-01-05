import assert from 'assert'
import _ from 'lodash'
import { HatanError } from '../../common/data/HatanError'
import { BoardState, BoardStateTrial } from '../../common/solver/BoardState'
import {
  SolverEngine,
  SolverEngineLogger,
} from '../../common/solver/SolverEngine'
import { TrackAnalyzer } from '../../common/solver/topology/TrackAnalyzer'
import { TrackTopology } from '../../common/solver/topology/TrackTopology'
import { Deduplicator } from '../../util/Deduplicator'
import { Point } from '../../util/Point'
import { range } from '../../util/Utils'
import { LineCell } from '../data/Cell'
import { SlilinBoard } from '../data/SlilinBoard'
import { CellRuleRunner } from './CellRuleRunner'
import { BasicNumberRule } from './rules/BasicNumberRule'
import { DotRule } from './rules/DotRule'
import { IncomingCornerRule } from './rules/IncomingCornerRule'
import { NumberCornerRule } from './rules/NumberCornerRule'
import { ThreePairInitialRule } from './rules/ThreePairInitialRule'
import { AvoidLoopTopologicalRule } from './topology/AvoidLoopTopologicalRule'
import { SegmentTopologicalRule } from './topology/SegmentTopologicalRule'
import { SlilinTrackAnalysisSubject } from './topology/SlilinTrackAnalysisSubject'

export class SlilinAssumption {
  constructor(
    b: SlilinBoard,
    public readonly p: Point,
  ) {
    assert(b.lineCellAtP(p) instanceof LineCell)
    assert(!b.lineCellAtP(p).fixed)
  }
}

export type SlilinBoardState = BoardState<SlilinBoard, TrackTopology>

export class SlilinSolver {
  readonly engine: SolverEngine<SlilinBoard, TrackTopology, SlilinAssumption>

  constructor(
    logger?: SolverEngineLogger<SlilinBoard, TrackTopology, SlilinAssumption>,
  ) {
    this.engine = new SolverEngine({
      snapshotDirtyFlags,
      duplicateBoard: b => b.duplicate(),
      analyzeTopology,
      checkCompletion,
      makeAssumptions,
      excludeAssumption,
      applyAssumption,
      invalidAssumption,
      sortTrialList,
      calcFingerprint,
      logger,
    })

    this.engine.initialRules.push(
      new CellRuleRunner(new ThreePairInitialRule()),
    )

    this.engine.cellRules.push(
      new CellRuleRunner(new BasicNumberRule()),
      new CellRuleRunner(new DotRule()),
      new CellRuleRunner(new NumberCornerRule()),
      new CellRuleRunner(new IncomingCornerRule()),
    )

    this.engine.topologicalRules.push(new AvoidLoopTopologicalRule())
    this.engine.topologicalRules.push(new SegmentTopologicalRule())
  }

  solve(b: SlilinBoard): BoardState<SlilinBoard, TrackTopology> {
    return this.engine.solve(b)
  }
}

function snapshotDirtyFlags(b: SlilinBoard): void {
  // swap the dirty flag buffers
  ;[b.dirtyFlags, b.dirtyFlagsBuffer] = [b.dirtyFlagsBuffer, b.dirtyFlags]

  // clear the second buffer
  b.dirtyFlagsBuffer.cell.elements.fill(false)
  b.dirtyFlagsBuffer.line.fill(false)
}

function analyzeTopology(b: SlilinBoard): TrackTopology {
  return TrackAnalyzer.analyze(new SlilinTrackAnalysisSubject(b))
}

function checkCompletion(b: SlilinBoard, topology: TrackTopology): boolean {
  let complete = true

  // 数字の周りの線の数がぴったりかどうかと、破綻がないかどうかをチェック
  for (const y of range(2, b.physicalHeight - 2, 2)) {
    for (const x of range(2, b.physicalWidth - 2, 2)) {
      const c = b.numberCellAt(x, y)
      const n = c.value
      if (n !== null) {
        const { existsAround, forbiddenAround } = c
        if (existsAround > n || forbiddenAround > 4 - n) {
          throw new HatanError('checkCompletion: inconsistent number and lines')
        } else if (existsAround < n) {
          complete = false
        }
      }
    }
  }
  // 点の周りの線について、中途半端や破綻がないかどうかをチェック
  // (このチェックは不要かもしれない)
  for (const y of range(1, b.physicalHeight - 1, 2)) {
    for (const x of range(1, b.physicalWidth - 1, 2)) {
      const { existsAround, forbiddenAround } = b.surroundedCellAt(x, y)
      if (existsAround > 2 || (existsAround === 1 && forbiddenAround === 3)) {
        throw new HatanError('checkCompletion: inconsistent dot and lines')
      } else if (existsAround === 1) {
        complete = false
      }
    }
  }

  const tracks = topology.tracks

  if (tracks.length >= 2) {
    // 複数の小さいループになっていないかをチェック
    if (tracks.some(t => t.closed)) {
      throw new HatanError('checkCompletion: multiple small loops')
    }
  }

  // 1つのループが完成しているかどうかをチェック
  return complete && tracks.length === 1 && tracks[0].closed
}

function makeAssumptions(
  b: SlilinBoard,
  topology: TrackTopology,
): SlilinAssumption[] {
  if (topology.tracks.length > 0) {
    // 存在する線の端点を1つ延長したものを仮説として採用する
    const dedup = new Deduplicator<string>()
    return topology.tracks
      .flatMap(t => t.ends)
      .flatMap(p => Point.adjacent4(p.x, p.y))
      .filter(p => !b.lineCellAt(p.x, p.y).fixed)
      .filter(p => !dedup.hasAndAdd(p.toString()))
      .map(p => new SlilinAssumption(b, p))
  } else {
    // 線がまったくない特殊な状態; すべての選択肢を仮説として採用する
    return _.range(1, b.physicalHeight - 1).flatMap(y =>
      _.range(1 + (y % 2), b.physicalWidth - 1, 2).map(
        x => new SlilinAssumption(b, Point.at(x, y)),
      ),
    )
  }
}

function applyAssumption(b: SlilinBoard, a: SlilinAssumption): void {
  b.lineCellAtP(a.p).setExists()
  b.lineCellAtP(a.p).assumed = true // 仮説であることを記録
}

function excludeAssumption(b: SlilinBoard, a: SlilinAssumption): void {
  b.lineCellAtP(a.p).setForbidden()
}

function invalidAssumption(
  trialList: BoardStateTrial<SlilinBoard, TrackTopology, SlilinAssumption>[],
  state: BoardState<SlilinBoard, TrackTopology>,
  assumption: SlilinAssumption,
): boolean {
  return (
    state.board.lineCellAtP(assumption.p).fixed ||
    trialList.some(t => t.board.lineCellAtP(assumption.p).exists)
  )
}

function sortTrialList(
  trialList: BoardStateTrial<SlilinBoard, TrackTopology, SlilinAssumption>[],
): void {
  // 線の長さの降順並び替え、線を最もたくさん書けたものを優先する
  trialList.sort((a, b) => b.topology.totalLeg - a.topology.totalLeg)
}

function calcFingerprint(b: SlilinBoard): bigint {
  return b.calcFingerprint()
}
