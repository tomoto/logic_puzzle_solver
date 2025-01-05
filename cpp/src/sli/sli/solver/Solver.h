#pragma once

#include "cmn/solver/Solver.h"
#include "cmn/solver/topology/TrackTopology.h"
#include "cmn/solver/topology/TrackAnalyzer.h"

#include "sli/data/Board.h"
#include "sli/solver/rule/CellRuleRunner.h"
#include "sli/solver/rule/ThreePairInitialRule.h"
#include "sli/solver/rule/BasicNumberRule.h"
#include "sli/solver/rule/DotRule.h"
#include "sli/solver/rule/NumberCornerRule.h"
#include "sli/solver/rule/IncomingCornerRule.h"
#include "sli/solver/topology/AvoidLoopTopologicalRule.h"
#include "sli/solver/topology/SegmentTopologicalRule.h"
#include "sli/solver/topology/TrackAnalysisSubject.h"

#include "utl/Utils.h"

#include <set>
#include <algorithm>

namespace sli {

  struct Solver {
    struct Assumption {
      Point p;
    };

    struct Traits {
      typedef Board B;
      typedef cmn::TrackTopology P;
      typedef Assumption A;
      typedef vector<bool> Fingerprint;
      typedef cmn::BoardState<B, P> BoardState;
      typedef cmn::BoardStateTrial<B, P, A> BoardStateTrial;
      typedef std::function<void(BoardState&, int)> ReportProgressFunc;

      CellRuleRunner<ThreePairInitialRule<true>> threePairInitialRule;
      CellRuleRunner<ThreePairInitialRule<false>> threePairInitialRuleWithSmallLoop;

      CellRuleRunner<BasicNumberRule> basicNumberRule;
      CellRuleRunner<DotRule> dotRule;
      CellRuleRunner<NumberCornerRule> numberCornerRule;
      CellRuleRunner<IncomingCornerRule> incomingCornerRule;
      AvoidLoopTopologicalRule avoidLoopTopologicalRule;
      SegmentTopologicalRule segmentTopologicalRule;

      static void nullReportProgressFunc(BoardState&, int) {}
      ReportProgressFunc reportProgressFunc = nullReportProgressFunc;

      bool applyInitialRules(Board& b) {
        if (b.noSmallLoopsAllowed()) {
          return threePairInitialRule.apply(b);
        } else {
          // 33の周りの小ループが解になり得る特別な場合
          return threePairInitialRuleWithSmallLoop.apply(b);
        }
      }

      bool applyBoardRules(Board& b) {
        auto result = basicNumberRule.apply(b);
        result |= dotRule.apply(b);
        result |= numberCornerRule.apply(b);
        result |= incomingCornerRule.apply(b);
        return result;
      }

      bool applyTopologicalRules(Board& b, const P& topology) {
        auto result = avoidLoopTopologicalRule.apply(b, topology);
        result |= segmentTopologicalRule.apply(b, topology);
        return result;
      }

      P analyzeTopology(Board& b) {
        return cmn::TrackAnalyzer::analyze(TrackAnalysisSubject(b));
      }

      cmn::CompletionStatus checkCompletion(Board& b, const P& topology) {
        bool complete = true;

        // 数字の周りの線の数がぴったりかどうかと、破綻がないかどうかをチェック
        {
          auto [xrange, yrange] = Helper::cellRangeFor(b, CellType::number, true);
          for (auto y : yrange) {
            for (auto x : xrange) {
              auto n = b.numberAt(x, y);
              if (n != NumberCell::empty) {
                auto& s = b.surroundingStatsAt(x, y);
                if (s.exists > n || s.forbidden > 4 - n) {
                  throw HatanException("checkCompletion: inconsistent number and lines");
                } else if (s.exists < n) {
                  complete = false;
                }
              }
            }
          }
        }

        // 点の周りの線について、中途半端や破綻がないかどうかをチェック
        // (このチェックは冗長かもしれない)
        // {
        //   auto [xrange, yrange] = Helper::cellRangeFor(b, CellType::dot, false);
        //   for (auto y : yrange) {
        //     for (auto x : xrange) {
        //       auto& s = b.surroundingStatsAt(x, y);
        //       if (s.exists > 2 || (s.exists == 1 && s.forbidden == 3)) {
        //         throw HatanException("checkCompletion: inconsistent dot and lines");
        //       } else if (s.exists == 1) {
        //         complete = false;
        //       }
        //     }
        //   }
        // }

        auto& tracks = topology.tracks;
        if (tracks.size() >= 2) {
          // 複数の小さいループになっていないかをチェック
          if (std::any_of(tracks.begin(), tracks.end(), [](const auto& t) { return t.closed(); })) {
            throw HatanException("checkCompletion: multiple small loops");
          }
        }

        // 1つのループが完成しているかどうかをチェック
        return (complete && tracks.size() == 1 && tracks[0].closed())
          ? cmn::CompletionStatus::complete
          : cmn::CompletionStatus::incomplete;
      }

      vector<Assumption> enumerateAssumptions(Board& b, const P& topology) {
        vector<Assumption> result;
        if (topology.tracks.size() > 0) {
          // 存在する線の端点を1つ延長したものを仮説として採用する
          std::unordered_set<Point, PointHash> dedup;
          auto helper = [&](const Point& p) {
            for (const auto& v : Directions::adjacents) {
              auto q = p.add(v);
              if (!b.lineAt(q).fixed() && !hasAndAdd(dedup, q)) {
                result.push_back(Assumption{ q });
              }
            }};

          for (auto& t : topology.tracks) {
            helper(t.start);
            helper(t.end);
          }
        } else {
          // 線がまったくない特殊な状態; すべての選択肢を仮説として採用する
          for (auto y : range(1, b.physicalHeight - 1)) {
            for (auto x : range(1 + (y & 1), b.physicalWidth - 1, 2)) {
              if (!b.lineAt(x, y).fixed()) {
                result.push_back({ x, y });
              }
            }
          }
        }
        return std::move(result);
      }

      Board duplicateBoard(Board& b) {
        return b.duplicate();
      }

      Fingerprint calcFingerprint(Board& b) {
        Fingerprint result;
        result.reserve(b.lines.size());
        for (auto& line : b.lines) {
          result.push_back(line.exists);
        }
        return result;
      }

      void applyAssumption(Board& b, const Assumption& assumption) {
        b.setExistsAt(assumption.p);
        b.lineAt(assumption.p).assumed = true;
      }

      bool rejectAssumption(Board& b, const Assumption& assumption) {
        b.lineAt(assumption.p).assumed = true;
        return b.setForbiddenAt(assumption.p);
      }

      bool invalidAssumption(
        vector<BoardStateTrial>& trialList,
        BoardState& state,
        const Assumption& assumption)
      {
        auto& p = assumption.p;
        auto& b = state.board;

        if (state.topology.totalLeg > 0) {
          switch (Helper::cellTypeAt(p.x, p.y)) {
          case CellType::hline:
            if (!(b.surroundingStats.at(p.x - 1, p.y).exists == 1
              || b.surroundingStats.at(p.x + 1, p.y).exists == 1)) {
              return true;
            }
            break;
          case CellType::vline:
            if (!(b.surroundingStats.at(p.x, p.y - 1).exists == 1
              || b.surroundingStats.at(p.x, p.y + 1).exists == 1)) {
              return true;
            }
            break;
          }
        }

        return b.lineAt(p).fixed()
          || std::any_of(
            trialList.begin(),
            trialList.end(),
            [&](auto& t) { return t.board.lineAt(p).exists; });
      }

      void snapshotDirtyFlags(Board& b) {
        std::swap(b.dirtyFlags, b.dirtyFlagsBuffer);
        fill(b.dirtyFlagsBuffer.cell.data, false);
        fill(b.dirtyFlagsBuffer.line, false);
      }

      void sortTrialList(vector<BoardStateTrial>& trialList) {
        std::sort(trialList.begin(), trialList.end(), [](BoardStateTrial& a, BoardStateTrial& b) {
          return a.topology.totalLeg > b.topology.totalLeg
            || (a.topology.totalLeg == b.topology.totalLeg
              && a.topology.tracks.size() > b.topology.tracks.size());
          });
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
      return engine.solve(board);
    }
  };

}
