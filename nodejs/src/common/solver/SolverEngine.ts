import assert from 'assert'
import { LimitedDeduplicator } from '../../util/Deduplicator'
import { Optional, evalAllOr } from '../../util/Utils'
import { HatanError } from '../data/HatanError'
import { BoardStateTrial, BoardState } from './BoardState'
import { CompletionStatus } from './BoardState'

export interface Rule<Board> {
  apply(b: Board): boolean
}

export interface TopologicalRule<B, P> {
  apply(b: B, p: P): boolean
}

export type Fingerprint = string | number | symbol | bigint

export interface SolverEngineOptions<B, P, A> {
  readonly snapshotDirtyFlags: (b: B) => void
  readonly duplicateBoard: (b: B) => B
  readonly analyzeTopology: (b: B) => P
  readonly checkCompletion: (b: B, p: P) => boolean
  readonly makeAssumptions: (b: B, p: P, lastAssumption: Optional<A>) => A[]
  readonly excludeAssumption: (b: B, a: A) => void
  readonly applyAssumption: (b: B, a: A) => void
  readonly invalidAssumption: (
    trialList: BoardStateTrial<B, P, A>[],
    boardState: BoardState<B, P>,
    assumption: A,
  ) => boolean
  readonly sortTrialList: (trialList: BoardStateTrial<B, P, A>[]) => void
  readonly calcFingerprint: (b: B) => Fingerprint
  readonly logger?: SolverEngineLogger<B, P, A>
}

export interface SolverEngineLogger<B, P, A> {
  reportProgress?: (state: BoardState<B, P>, depth: number) => void
  onHatanDetected?: (b: B, e: HatanError) => void
  onAssumptionEvaluated?: (evaluatedTrial: BoardStateTrial<B, P, A>) => void
}

export class SolverEngine<B, P, A> {
  readonly initialRules: Rule<B>[] = []
  readonly cellRules: Rule<B>[] = []
  readonly topologicalRules: TopologicalRule<B, P>[] = []

  readonly fingerprintSet = new LimitedDeduplicator<Fingerprint>(10000)

  constructor(readonly opt: SolverEngineOptions<B, P, A>) {}

  private applyInitialRules(b: B): boolean {
    return evalAllOr(this.initialRules, rule => rule.apply(b))
  }

  private applyCellRules(b: B): boolean {
    return evalAllOr(this.cellRules, rule => rule.apply(b))
  }

  private applyTopologicalRules(b: B, p: P): boolean {
    return evalAllOr(this.topologicalRules, rule => rule.apply(b, p))
  }

  solve(board: B): BoardState<B, P> {
    const state: BoardState<B, P> = {
      board,
      topology: {} as P,
      status: 'incomplete',
    }
    this.solveRecursively(state, 0)
    return state
  }

  private solveRecursively(state: BoardState<B, P>, depth: number): void {
    this.solveWithoutTrialAndError(state, depth)

    if (state.status === 'incomplete') {
      // 終了していなければトライアンドエラーに入る
      this.assumeAndSolveRecursively(state, depth + 1)
    }
  }

  /**
   * トライアンドエラーなしにルールだけで行けるところまで解を進める
   *
   * @param board 盤面; 状態が直接変更される
   * @param depth 再帰の深さ; 初期盤面は0
   */
  private solveWithoutTrialAndError(
    state: BoardState<B, P>,
    depth: number,
  ): void {
    const board = state.board
    try {
      if (depth === 0) {
        // 初期ルールを適用
        while (this.applyInitialRules(board)) {
          // 変化がなくなるまで繰り返す
          this.opt.logger?.reportProgress?.(state, depth)
        }
      }

      while (true) {
        // セルルールの適用を変化がなくなるまで繰り返す
        this.opt.snapshotDirtyFlags(board)
        while (this.applyCellRules(board)) {
          this.opt.logger?.reportProgress?.(state, depth)
          this.opt.snapshotDirtyFlags(board)
        }

        // 落ち着いたらトポロジーを解析
        state.topology = this.opt.analyzeTopology(board)

        // トポロジールールを適用し、変化がなければ終了
        if (!this.applyTopologicalRules(board, state.topology)) {
          break
        }

        // 変化があれば再びセルルールの適用に戻る
        this.opt.logger?.reportProgress?.(state, depth)
      }

      // 完成状態をチェックし、戻り値とする
      // (incomplete だったら呼び出し側はこの後トライアンドエラーに入る)
      state.status = this.opt.checkCompletion(board, state.topology)
        ? 'complete'
        : 'incomplete'
    } catch (e) {
      state.status = this.translateHatanError(e, board)
    }
  }

  /**
   * 仮定を列挙し、解を探索する
   */
  private assumeAndSolveRecursively(
    state: BoardState<B, P>,
    depth: number,
  ): void {
    // 出発点の状態は、ルールにより解き切ったincomplete状態であること
    assert(state.status === 'incomplete')

    // 厳選した仮説と、その過程で得られた新しい盤面の状態を取得
    const trialList = this.selectAssumptions(state, depth)
    if (state.status !== 'incomplete') {
      return // 完成または破綻したのでそのまま返す
    }

    // 有望なものから順に並べ替える
    this.opt.sortTrialList(trialList)

    // 再帰的に解く
    for (const trial of trialList) {
      assert(trial.status !== 'incomplete')
      if (this.hasAlreadySeen(trial.board)) {
        // 既に見たことのある盤面なのでスキップ
        continue
      }

      // 再帰に潜る
      this.assumeAndSolveRecursively(trial, depth + 1)

      // 破綻の場合は仮説がダメなことをマークして解き進める
      // 完成の場合はそのまま返す
      this.postprocessAssumptionTrial(state, trial, depth)
      if (state.status !== 'incomplete') {
        return
      }
    }

    // 仮説が尽きたので破綻を返す
    state.status = 'hatan'
  }

  private selectAssumptions(
    state: BoardState<B, P>,
    depth: number,
  ): BoardStateTrial<B, P, A>[] {
    while (true) {
      if (state.status !== 'incomplete') {
        // 盤面が完成または破綻しているのでそのまま返す
        return []
      }

      // 仮説を列挙
      const assumptions = this.opt.makeAssumptions(
        state.board,
        state.topology,
        undefined, // TODO: lastAssumptionどうするか決める
      )

      const trialList: BoardStateTrial<B, P, A>[] = []
      let changed = false
      for (const assumption of assumptions) {
        if (this.opt.invalidAssumption(trialList, state, assumption)) {
          // 同じ結果をもたらす仮説が既にあればスキップ
          continue
        }

        // 盤面を複製して仮説を適用
        const newBoard = this.opt.duplicateBoard(state.board)

        this.opt.applyAssumption(newBoard, assumption)

        // 既に見たことのある盤面なのでスキップ
        if (this.hasAlreadySeen(newBoard)) {
          continue
        }

        // greedyな深さ優先探索であれば、ここでsolveRecursivelyを呼ぶだけでよい。
        // const newState = this.solveRecursively(newBoard, depth + 1)
        // assert(newState.completionState !== 'incomplete')

        // しかし、ここでは広さ優先的に行いたいので、
        // 一旦 solveWithoutTrialAndError で途中まで解を進める
        const trial: BoardStateTrial<B, P, A> = {
          board: newBoard,
          topology: {} as P,
          status: 'incomplete',
          assumption,
        }
        this.solveWithoutTrialAndError(trial, depth + 1)

        this.opt.logger?.onAssumptionEvaluated?.(trial)
        if (trial.status === 'incomplete') {
          // 検討の余地のある仮説なのでリストに追加
          trialList.push(trial)
        } else {
          // 破綻の場合は仮説がダメなことをマークして解き進める
          // 完成の場合はそのまま返す
          if (this.postprocessAssumptionTrial(state, trial, depth)) {
            changed = true
          }
          if (state.status !== 'incomplete') {
            return []
          }
        }
      }

      if (changed) {
        // オリジナルの盤面に新情報が増えたので再評価
        continue
      }

      // 仮説の列挙完了, good job!
      return trialList
    }
  }

  private postprocessAssumptionTrial(
    state: BoardState<B, P>,
    trial: BoardStateTrial<B, P, A>,
    depth: number,
  ): boolean {
    switch (trial.status) {
      case 'complete':
        Object.assign(state, trial)
        return false
      case 'hatan':
        // 破綻したのでオリジナルの盤面にこの仮説はダメなことをマークし、さらに解き進める
        try {
          this.opt.excludeAssumption(state.board, trial.assumption)
        } catch (e) {
          state.status = this.translateHatanError(e, state.board)
          return false
        }
        this.solveWithoutTrialAndError(state, depth)
        return true
    }
    return false
  }

  private hasAlreadySeen(board: B): boolean {
    const fingerprint = this.opt.calcFingerprint(board)
    return this.fingerprintSet.hasAndAdd(fingerprint)
  }

  private translateHatanError(e: unknown, b: B): CompletionStatus {
    if (e instanceof HatanError) {
      this.opt.logger?.onHatanDetected?.(b, e)
      return 'hatan'
    } else {
      throw e // unexpected error, just rethrow
    }
  }
}
