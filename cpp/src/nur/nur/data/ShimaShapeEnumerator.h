#pragma once

#include "Shima.h"
#include "utl/NumericCombination.h"
#include "utl/Utils.h"
#include "utl/Array2DUtils.h"

#include <ranges>
#include <iostream>

namespace nur {

  /// あり得るシマの形を列挙する処理
  struct ShimaShapeEnumerator {
    vector<Shima> result;

    void enumerate(Shima& shima) {
      result.clear();

      enumerateInternal(shima, shima.number, shima.number, {});
    }

    string toString() {
      using namespace Array2DUtils;

      string s;
      for (auto& shima : result) {
        s += join(alignRight(transform(
          shima.potential,
          [](int value) { return std::to_string(value); } \
        )), " ") + "\n\n";
      }

      return s;
    }

  private:
    /// ポテンシャルを下りながら、可能な白マス確保の組み合わせを列挙する
    /// @param parentShima 処理中のシマ状態
    /// @param potential 処理中のポテンシャル
    /// @param remaining 残り確保すべきマスの数
    /// @param pointsToConfirm 今回確保するマスのリスト
    void enumerateInternal(Shima& parentShima, int potential, int remaining, const vector<Point>& pointsToConfirm) {
      // 対象のポテンシャルにおいて、確保しようとしているマス数と確定済みのマス数の合計が残り確保数を越えてはならない
      assert(pointsToConfirm.size() + parentShima.confirmedPointsByPotential[potential].size() <= remaining);

      Shima shima = parentShima; // make a copy
      try {
        // まず指定されたマスを領地として確定
        for (auto& p : pointsToConfirm) {
          shima.confirm(p);
        }
        // 確定されなかったマスを除外
        auto pointsToReject = shima.possiblePointsByPotential[potential];
        for (auto& p : pointsToReject) {
          shima.reject(p);
        }
      } catch (cmn::HatanException& e) {
        // なぜか破綻したので選択肢から除外
        return;
      }

      // 残りの確保すべきマス数を減らす
      remaining -= shima.confirmedPointsByPotential[potential].size();

      if (remaining == 0) {
        // 確保完了したので最終チェック
        for (int i : range(1, potential)) {
          if (!shima.confirmedPointsByPotential[i].empty()) {
            // ここよりさらに遠くに確定マスがあった＝届いていない場合は選択肢からはずす
            return;
          }
        }
        // 本当に確定する
        result.push_back(shima);
        return;
      }

      // 最低ポテンシャルに達したのにマスが余っていたら何かおかしい
      assert(potential > 1);

      //
      // 一段低いポテンシャルから次に確定させるマスを選択し、再帰的に処理
      //

      // 確定マスに隣接する選択肢となる(ポテンシャルが一段低い)マスを列挙
      std::set<Point> nextPossiblePoints; // 未確定のマス
      std::set<Point> nextConfirmedPoints; // 確定済みのマス
      for (auto& p : shima.confirmedPointsByPotential[potential]) {
        for (auto& d : Directions::adjacents) {
          if (auto q = p.add(d); shima.potentialAt(q) == potential - 1) {
            if (shima.confirmedPointsByPotential[potential - 1].contains(q)) {
              nextConfirmedPoints.insert(q);
            } else {
              nextPossiblePoints.insert(q);
            }
          }
        }
      }

      if (nextConfirmedPoints.size() != shima.confirmedPointsByPotential[potential - 1].size()) {
        // 隣接マスに確定マスがすべて含まれていなければ選択肢から除外
        return;
      }

      // 一段低いポテンシャルに確定マスがある場合、オプションから何も選択しないのもOK
      int minPickUp = nextConfirmedPoints.empty() ? 1 : 0;
      // オプションから選択できる最大数は、選択肢の数と残りマス数とで決まる
      int maxPickUp = std::min(nextPossiblePoints.size(), remaining - nextConfirmedPoints.size());

      // 選択肢から組み合わせで選び出し、再帰的に処理
      vector nextPossiblePointVector(nextPossiblePoints.begin(), nextPossiblePoints.end());
      for (int i : range(minPickUp, maxPickUp + 1)) {
        NumericCombination combination(nextPossiblePoints.size(), i);
        do {
          auto nextPointsToConfirm = transform(combination.current, [&](int index) {
            return nextPossiblePointVector[index];
            });
          enumerateInternal(shima, potential - 1, remaining, nextPointsToConfirm);
        } while (combination.next());
      }
    }
  };

}
