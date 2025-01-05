#pragma once

#include "cmn/solver/Solver.h"
#include "nur/data/Board.h"
#include "nur/data/Assumption.h"
#include "nur/solver/topology/Topology.h"
#include "nur/solver/rule/ShimaCompletionRule.h"
#include "nur/solver/rule/ShimaBorderRule.h"
#include "nur/solver/rule/SPOverwrapRule.h"
#include "nur/solver/rule/ShimaPotentialReachabilityRule.h"
#include "nur/solver/rule/ShimaShapePossibilityRule.h"
#include "nur/solver/topology/NoBlankShimaAllowedTopologicalRule.h"
#include "nur/solver/topology/KabeTopologicalRule.h"
#include "nur/solver/topology/ShimaTopologicalRule.h"
#include "nur/solver/AssumptionEnumerator.h"
#include "utl/Utils.h"

#include <functional>

namespace nur {

  struct Solver {
    struct Traits {
      typedef Board B;
      typedef Topology P;
      typedef Assumption A;
      typedef vector<bool> Fingerprint;
      typedef cmn::BoardState<B, P> BoardState;
      typedef cmn::BoardStateTrial<B, P, A> BoardStateTrial;
      typedef std::function<void(BoardState&, int)> ReportProgressFunc;

      ShimaCompletionRule shimaCompletionRule;
      ShimaBorderRule shimaBorderRule;
      SPOverwrapRule spOverwrapRule;
      ShimaPotentialReachabilityRule shimaPotentialReachabilityRule;
      ShimaShapePossibilityRule shimaShapePossibilityRule;
      NoBlankShimaAllowedTopologicalRule noBlankShimaAllowedTopologicalRule;
      KabeTopologicalRule kabeTopologicalRule;
      ShimaTopologicalRule shimaTopologicalRule;

      static void nullReportProgressFunc(BoardState&, int) {}
      ReportProgressFunc reportProgressFunc = nullReportProgressFunc;

      bool applyInitialRules(Board& b) {
        return false;
      }

      bool applyBoardRules(Board& b) {
        auto result = false;
        result |= shimaCompletionRule.apply(b);
        result |= shimaBorderRule.apply(b);
        result |= spOverwrapRule.apply(b);
        result |= shimaPotentialReachabilityRule.apply(b);
        // result |= shimaShapePossibilityRule.apply(b);
        return result;
      }

      bool applyTopologicalRules(Board& b, Topology& p) {
        auto result = false;
        result |= noBlankShimaAllowedTopologicalRule.apply(b, p);
        result |= kabeTopologicalRule.apply(b, p);
        result |= shimaTopologicalRule.apply(b, p);
        return result;
      }

      Topology analyzeTopology(Board& b) {
        return Topology::analyze(b);
      }

      cmn::CompletionStatus checkCompletion(Board& b, const Topology& topology) {
        bool complete = true;
        for (auto& shima : b.activeShimaList()) {
          if (shima.confirmedArea > shima.number) {
            throw cmn::HatanException("Shima is too large");
          } else if (shima.confirmedArea < shima.number) {
            complete = false;
          }
        }

        if (topology.kabeOpenPoints.size() != 1 || topology.openArea > 0) {
          complete = false;
        }

        return complete ? cmn::CompletionStatus::complete : cmn::CompletionStatus::incomplete;
      }

      Board duplicateBoard(Board& b) {
        return b.duplicate();
      }

      Fingerprint calcFingerprint(Board& b) {
        // TODO efficiency
        vector<bool> result;
        for (auto cell : b.cells.data) {
          result.push_back(cell.kabe());
          result.push_back(cell.anyShima());
        }
        return result;
      }

      vector<Assumption> enumerateAssumptions(Board& b, const Topology& topology) {
        return AssumptionEnumerator::enumerate(b, topology);
      }

      void applyAssumption(Board& b, const Assumption& assumption) {
        if (assumption.kabe) {
          b.confirmKabe(assumption.p); // カベを仮に設定する
        } else {
          b.reserveShima(assumption.p); // シマを仮に設定する
        }
        b.setAssumed(assumption.p); // 仮説であることを記録する
      }

      bool rejectAssumption(Board& b, const Assumption& assumption) {
        return assumption.kabe
          ? b.reserveShima(assumption.p) // カベが否定されたのでシマに設定する
          : b.confirmKabe(assumption.p); // シマが否定されたのでカベに設定する
      }

      bool invalidAssumption(
        vector<BoardStateTrial>& trialList,
        BoardState& state,
        const Assumption& assumption
      ) {
        // 既に確定しているか、結果的に同じ状態になる仮説が採択されていたら無効
        return !state.board.cellAt(assumption.p).open()
          || std::any_of(trialList.begin(), trialList.end(),
            [&](auto& trial) {
              auto cell = trial.board.cellAt(assumption.p);
              return (cell.anyShima() && !assumption.kabe) || (cell.kabe() && assumption.kabe);
            });
      }

      void snapshotDirtyFlags(Board& b) {
        for (auto& shima : b.activeShimaList()) {
          shima.snapshotDirtyFlags();
        }
      }

      void sortTrialList(vector<BoardStateTrial>& trialList) {
        assert(false);
        // TODO
      }

      void reportProgress(BoardState& state, int depth) {
        reportProgressFunc(state, depth);
      }
    } traits;

    using BoardState = Traits::BoardState;
    using ReportProgressFunc = Traits::ReportProgressFunc;

    cmn::Solver::Engine<Traits> engine = { traits };

    auto solve(
      Board& board,
      const ReportProgressFunc& reportProgressFunc = Traits::nullReportProgressFunc)
    {
      engine.traits.reportProgressFunc = reportProgressFunc;
      auto state = engine.solve(board);
      return state;
    }
  };

}
