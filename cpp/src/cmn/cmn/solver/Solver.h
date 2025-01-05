#pragma once

#include "cmn/data/Base.h"
#include "cmn/data/HatanException.h"
#include "cmn/solver/BoardState.h"
#include "cmn/solver/topology/TrackAnalyzer.h"
#include "utl/Utils.h"

#include <concepts>
#include <iostream>

namespace cmn::Solver {

  template <typename S>
  concept Traits = requires(
    S s,
    S::B b,
    S::P topology,
    S::A assumption,
    BoardState<typename S::B, typename S::P> state,
    vector<BoardStateTrial<typename S::B, typename S::P, typename S::A>> trialList)
  {
    typename S::B;
    typename S::P;
    typename S::A;
    typename S::Fingerprint;

    { s.applyInitialRules(b) } -> std::convertible_to<bool>;
    { s.applyBoardRules(b) } -> std::convertible_to<bool>;
    { s.applyTopologicalRules(b, topology) } -> std::convertible_to<bool>;
    { s.analyzeTopology(b) } -> std::convertible_to<typename S::P>;
    { s.checkCompletion(b, topology) } -> std::convertible_to<CompletionStatus>;
    { s.enumerateAssumptions(b, topology) } -> std::convertible_to<std::vector<typename S::A>>;
    { s.duplicateBoard(b) } -> std::convertible_to<typename S::B>;
    { s.calcFingerprint(b) } -> std::convertible_to<typename S::Fingerprint>;
    s.applyAssumption(b, assumption);
    { s.rejectAssumption(b, assumption) } -> std::convertible_to<bool>;
    { s.invalidAssumption(trialList, state, assumption) } -> std::convertible_to<bool>;
    s.sortTrialList(trialList);
    s.snapshotDirtyFlags(b);
    s.reportProgress(state, 0);
  };

  template <Traits S>
  struct Engine {
    typedef S::B B;
    typedef S::P P;
    typedef S::A A;
    typedef S::Fingerprint Fingerprint;
    typedef BoardState<B, P> State;
    typedef BoardStateTrial<B, P, A> Trial;

    S& traits;

    std::unordered_set<Fingerprint> seenFingerprints;

    Engine(S& traits) : traits(traits) {}
    DELETE_COPY_CTORS(Engine);

    State solve(B& board) {
      State state = { traits.duplicateBoard(board) };
      solveRecursively(state, 0);
      return std::move(state);
    }

    void solveRecursively(State& state, int depth) {
      // 定石だけで解けるところまで進める
      solveWithoutTrialAndError(state, depth);

      if (state.status == CompletionStatus::incomplete) {
        // 終了していなければトライアンドエラーに入る
        assumeAndSolveRecursively(state, depth + 1);
      }
    }

    void solveWithoutTrialAndError(State& state, int depth) {
      try {
        auto& b = state.board;
        if (depth == 0) {
          // 初期ルールを適用
          while (traits.applyInitialRules(b)) {
            // 変化がなくなるまで繰り返す
            traits.reportProgress(state, depth);
          }
        }

        while (true) {
          // セルルールを適用
          traits.snapshotDirtyFlags(b);
          while (traits.applyBoardRules(b)) {
            // 変化がなくなるまで繰り返す
            traits.snapshotDirtyFlags(b);
            traits.reportProgress(state, depth);
          }

          // 落ち着いたらトポロジーを解析
          state.topology = traits.analyzeTopology(b);

          // トポロジールールを適用し、変化がなければ終了
          if (!traits.applyTopologicalRules(b, state.topology)) {
            // 完成状態をチェックし、戻り値とする
            // (incomplete だったら呼び出し側はこの後トライアンドエラーに入る)
            state.status = traits.checkCompletion(b, state.topology);
            return;
          }

          // 変化があれば再びセルルールの適用に戻る
          traits.reportProgress(state, depth);
        }
      } catch (HatanException&) {
        // 現在の盤面を破綻として返す
        state.status = CompletionStatus::hatan;
      }
    }

    void assumeAndSolveRecursively(BoardState<B, P>& state, int depth) {
      // 出発点の状態は、ルールにより解き切ったincomplete状態であること
      assert(state.status == CompletionStatus::incomplete);

      auto& b = state.board;

      // 厳選した仮説と、その過程で得られた新しい盤面の状態を取得
      auto trialList = selectAssumptions(state, depth);

      if (state.status != CompletionStatus::incomplete) {
        return; // 完成また破綻したのでそのまま返す
      }

      // 有望なものから順に並べ替える
      traits.sortTrialList(trialList);

      // 再帰的に解く
      for (auto& trial : trialList) {
        assert(trial.status == CompletionStatus::incomplete);
        if (hasAlreadySeen(trial.board)) {
          // 既に見たことのある盤面なのでスキップ
          continue;
        }

        // 再帰に潜る
        assumeAndSolveRecursively(trial, depth + 1);
        assert(trial.status != CompletionStatus::incomplete);

        // 破綻の場合は仮説がダメなことをマークして解き進める
        // 完成の場合はそのまま返す
        postprocessAssumptionTrial(state, trial, depth);
        if (state.status != CompletionStatus::incomplete) {
          return;
        }
      }

      // 仮説が尽きたので破綻を返す
      state.status = CompletionStatus::hatan;
    }

    vector<Trial> selectAssumptions(BoardState<B, P>& state, int depth) {
    retry:
      if (state.status != CompletionStatus::incomplete) {
        // 盤面が完成または破綻しているのでそのまま返す
        return {};
      }

      // 仮説を列挙
      auto& b = state.board;
      auto assumptions = traits.enumerateAssumptions(b, state.topology);

      // 仮説を評価
      vector<Trial> trialList;
      bool changed = false;
      for (auto& assumption : assumptions) {
        if (traits.invalidAssumption(trialList, state, assumption)) {
          // 同じ結果をもたらす仮説が既にあればスキップ
          continue;
        }

        // 盤面を複製して仮説を適用
        auto newBoard = traits.duplicateBoard(b);

        try {
          traits.applyAssumption(newBoard, assumption);
        } catch (HatanException&) {
          // 仮説設定時に破綻が発覚したのでスキップ
          continue;
        }

        // 既に見たことのある盤面なのでスキップ
        if (hasAlreadySeen(newBoard)) {
          continue;
        }

        // greedyな深さ優先探索であれば、ここでsolveRecursivelyを呼べばいい
        // solveRecursively(newState, depth);

        // しかし、ここでは広さ優先的に行いたいので、
        // 一旦 solveWithoutTrialAndError で途中まで解を進める
        Trial trial = { { std::move(newBoard) }, std::move(assumption) };
        solveWithoutTrialAndError(trial, depth);

        if (trial.status == CompletionStatus::incomplete) {
          // 検討の余地のある仮説なのでリストに追加
          trialList.push_back(std::move(trial));
        } else {
          // 破綻の場合は仮説がダメなことをマークして解き進める
          // 完成の場合はそのまま返す
          changed |= postprocessAssumptionTrial(state, trial, depth);
          if (state.status != CompletionStatus::incomplete) {
            return {};
          }
        }
      }

      if (changed) {
        // オリジナルの盤面に新情報が増えたので再評価
        goto retry;
      }

      // 仮説の列挙完了、good job!
      return std::move(trialList);
    }

    bool postprocessAssumptionTrial(State& state, Trial& trial, int depth) {
      switch (trial.status) {
      case CompletionStatus::complete:
        // 完成したので終了
        state = std::move(trial);
        return false;
      case CompletionStatus::hatan:
        // 破綻したのでオリジナルの盤面にこの仮説はダメなことをマークし、さらに解き進める
        try {
          if (!traits.rejectAssumption(state.board, trial.assumption)) {
            return false; // unchanged
          }
        } catch (HatanException&) {
          state.status = CompletionStatus::hatan;
          return false;
        }
        solveWithoutTrialAndError(state, depth);
        return true;
      }
      return false;
    }

    bool hasAlreadySeen(B& b) {
      return hasAndAdd(seenFingerprints, traits.calcFingerprint(b));
    }
  };

} // namespace cmn::Solver
