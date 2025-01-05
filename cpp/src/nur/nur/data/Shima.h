#pragma once

#include "Base.h"
#include "ShimaWalker.h"
#include "utl/Point.h"
#include "utl/Array2D.h"
#include "cmn/data/HatanException.h"

#include <map>
#include <set>
#include <numeric>
#include <algorithm>

namespace nur {

  ///
  /// シマ(数字を含む上下左右に連続した白マス)のモデル
  /// 数字nを中心に、上下左右方向に最大(n-1)伸びる想定
  ///
  struct Shima {
    int id = 0; // シマのID(1-based), 0はダミー
    int number = 0; // 数字
    Point center; // 中心点(数字のマスの位置)
    Point leftTop; // 左上点
    Point rightBottom; // 右下点
    int width = 0; // 幅(盤面によるクリップを考慮)
    int height = 0; // 高さ(盤面によるクリップを考慮)

    /// シマポテンシャル
    /// 数字を中心としたシマの到達可能性を表す
    /// 
    /// 数字=4の場合のシマポテンシャルの初期状態:
    ///
    /// 0001000
    /// 0012100
    /// 0123210
    /// 1234321
    /// 0123210
    /// 0012100
    /// 0001000
    ///
    /// (3,2)が黒マスになった場合、シマポテンシャルはそこを「流れる」ことができなくなり、
    /// 次のように更新される:
    ///
    /// 0000000
    /// 0010100
    /// 012x210
    /// 1234321
    /// 0123210
    /// 0012100
    /// 0001000
    ///
    Array2D<int> potential{ 0,0 };

    int confirmedArea = 0; // 確定した白マスの数
    int possibleArea = 0; // 可能性のある(=シマポテンシャルが0でない)白マスの数

    // 変更があったか
    bool dirty : 1 = true;
    bool dirtyBuffer : 1 = true;

    std::vector<std::set<Point>> possiblePointsByPotential;
    std::vector<std::set<Point>> confirmedPointsByPotential;

    /// ダミー用コンストラクタ
    Shima() {}

    Shima(int id, int n, const Point& center, int boardWidth, int boardHeight) :
      id(id),
      number(n),
      center(center),
      leftTop{ std::max(center.x - n + 1, 0), std::max(center.y - n + 1, 0) },
      rightBottom{
        std::min(center.x + n - 1, boardWidth - 1),
        std::min(center.y + n - 1, boardHeight - 1)
      },
      width(rightBottom.x - leftTop.x + 1),
      height(rightBottom.y - leftTop.y + 1),
      potential{ width, height },
      possiblePointsByPotential(n + 1),
      confirmedPointsByPotential(n + 1)
    {
      initPotential();
    }

    /// id==0のダミーならtrue
    bool dummy() const {
      return id == 0;
    }

    bool inside(int x, int y) const {
      return x >= leftTop.x && x <= rightBottom.x && y >= leftTop.y && y <= rightBottom.y;
    }

    bool inside(const Point& p) const {
      return inside(p.x, p.y);
    }

    int size() const {
      return number * 2 - 1;
    }

    auto xrange() const {
      return range(leftTop.x, rightBottom.x + 1);
    }

    auto yrange() const {
      return range(leftTop.y, rightBottom.y + 1);
    }

    bool complete() const {
      return confirmedArea == number;
    }

    bool rejectAround(const Point& p) {
      return reject(p)
        | Directions::applyToAdjacents([&](const Point& d) { return reject(p.add(d)); });
    }

    bool reject(const Point& p) {
      if (!inside(p)) {
        return false;
      }

      return updatePotential(p, 0);
    }

    bool confirm(const Point& p) {
      if (!inside(p)) {
        return false; // シマの外
      }
      auto n = potentialAt(p);
      if (confirmedPointsByPotential[n].contains(p)) {
        return false; // 既に確定済み
      }
      if (n == 0) {
        throw cmn::HatanException("Cannot take the cell with potential 0.");
      }
      if (complete()) {
        throw cmn::HatanException("Shima is too large");
      }

      confirmedArea++;

      possiblePointsByPotential[n].erase(p);
      confirmedPointsByPotential[n].insert(p);

      dirtify();

      if (confirmedArea == number) {
        // 領地がすべて確定したので、確定していないポテンシャルをすべて消去して終了
        clearUnconfirmedPotential();
        return true;
      }

      //
      // 最遠到達可能ポテンシャルに関するルールの適用
      //

      int mostDistantReachablePotential = calcMostDistantReachablePotential();

      // 最遠到達可能ポテンシャル未満の選択肢は全滅
      for (int i = 1; i < mostDistantReachablePotential; ++i) {
        clearUnconfirmedPotentialOf(i);
      }

      // 最遠到達可能ポテンシャルのマスが確定した場合、
      // 最遠到達可能ポテンシャルは必ず1減少する
      // ⇒同ポテンシャルの他の選択肢は全滅
      if (n == mostDistantReachablePotential) {
        clearUnconfirmedPotentialOf(n);
      }

      //
      // このマスが確定したことによるポテンシャルの及ぶ範囲の減少を考慮
      //

      ShimaWalker walker(*this, p);
      for (auto& y : yrange()) {
        for (auto& x : xrange()) {
          Point q = { x, y };
          if (!walker.walkedAt(q) && potentialAt(q) > 0) {
            updatePotential(q, 0);
          }
        }
      }

      return true;
    }

    //
    // シマポテンシャルの操作
    //

    /// シマポテンシャルの初期化
    /// 中心を数字と等しいものとし、周囲をマンハッタン距離に従って減少させた値とする
    void initPotential() {
      for (int y : yrange()) {
        for (int x : xrange()) {
          Point p = { x, y };
          int value = std::max(number - center.manhattanDistance(p), 0);
          if (value > 0) {
            setPotentialAt(p, value);
            possiblePointsByPotential[value].insert(p);
            possibleArea++;
          }
        }
      }
    }

    /// シマポテンシャルの取得
    /// シマの範囲外の場合は-1
    int potentialAt(const Point& p) const {
      return inside(p) ? potential.at(p.x - leftTop.x, p.y - leftTop.y) : -1;
    }

    // 指定位置へのシマポテンシャルの設定
    bool setPotentialAt(const Point& p, int value) {
      if (potentialAt(p) == value) {
        return false;
      }
      dirtify();
      return potential.at(p.x - leftTop.x, p.y - leftTop.y) = value;
    }

    /// 指定位置のシマポテンシャルを更新し、周囲にも影響を与える
    /// 減少のみ可能、増加はできない
    bool updatePotential(const Point& p, int newPotential) {
      // 同じならば何もしない
      int oldPotential = potentialAt(p);
      if (oldPotential == newPotential) {
        return false;
      }

      // 減る方向にしか更新しないはず
      assert(newPotential < oldPotential);

      // ポテンシャル書き換え
      setPotentialAt(p, newPotential);

      // pointsByPotentialの上で移動
      bool confirmed = confirmedPointsByPotential[oldPotential].contains(p);
      auto& pointsByPotential = confirmed ? confirmedPointsByPotential : possiblePointsByPotential;
      pointsByPotential[oldPotential].erase(p); // 古いバケツから抜く

      if (newPotential > 0) {
        pointsByPotential[newPotential].insert(p); // 新しいバケツに入れる
      } else {
        // 新たなポテンシャルがゼロの場合の特殊処理
        if (confirmed) {
          // 既に確定した領地のポテンシャルがゼロになった→届かないので破綻
          // throw cmn::HatanException("Cannot reject the cell once confirmed.");
        } else {
          // 可能性のある白マスを減らす
          possibleArea--;
        }
      }

      // 周囲のポテンシャルを再計算
      // このセルから流れ落ちる方向のみ更新すればよい
      if (oldPotential >= 2) {
        for (auto& d : Directions::adjacents) {
          auto q = p.add(d);
          if (potentialAt(q) == oldPotential - 1) {
            validatePotential(q);
          }
        }
      }

      return true;
    }

    /// シマポテンシャルがルールを満たすよう更新する
    bool validatePotential(const Point& p) {
      // 周囲のポテンシャルのうち最大のものを求める
      int maxAdjacentPotential = std::max({
        potentialAt(p.add(Directions::N)),
        potentialAt(p.add(Directions::E)),
        potentialAt(p.add(Directions::S)),
        potentialAt(p.add(Directions::W))
        });

      // 周囲のポテンシャルがここより高ければ更新不要
      if (maxAdjacentPotential > potentialAt(p)) {
        return false;
      }

      // 周囲のポテンシャルより一段階低く(流れ落ちる方向に)なるよう更新
      return updatePotential(p, std::max(maxAdjacentPotential - 1, 0));
    }

    /// 確定していないポテンシャルをすべて消去
    void clearUnconfirmedPotential() {
      for (auto& possiblePoints : possiblePointsByPotential) {
        for (auto& p : possiblePoints) {
          setPotentialAt(p, 0);
        }
        possiblePoints.clear();
      }
    }

    /// ポテンシャル=nの可能性を全滅させる
    /// (周辺のポテンシャル更新はしない)
    void clearUnconfirmedPotentialOf(int n) {
      for (auto& p : possiblePointsByPotential[n]) {
        setPotentialAt(p, 0);
      }
      possiblePointsByPotential[n].clear();
    }

    int calcMostDistantReachablePotential() const {
      // 最遠到達可能ポテンシャルを求める
      return 1 + std::accumulate(
        confirmedPointsByPotential.begin() + 1, // ポテンシャル0は無視
        confirmedPointsByPotential.end() - 1, // ポテンシャル最大は常に1マスなので無視
        0,
        [&](int result, auto& points) {
          // ポテンシャルごとに2つ目以降の確定マスがあれば、その分到達距離は短くなる
          return result + std::max(int(points.size()) - 1, 0);
        }
      );
    }

    void dirtify() {
      dirty = dirtyBuffer = true;
    }

    void snapshotDirtyFlags() {
      dirty = dirtyBuffer;
      dirtyBuffer = false;
    }

  };

} // namespace nur