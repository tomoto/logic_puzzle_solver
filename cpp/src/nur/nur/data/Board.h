#pragma once

#include "Base.h"
#include "Shima.h"
#include "CellState.h"
#include "cmn/data/HatanException.h"
#include "utl/Array2D.h"
#include "utl/Utils.h"

#include <numeric>

namespace nur {

  struct Board {
    /// 盤面の幅
    int width;
    /// 盤面の高さ
    int height;

    /// マスの状態
    Array2D<CellState> cells;

    /// 仮説かどうかのフラグ
    Array2D<bool> assumedFlags;

    /// カベIDの採番用カウンタ
    /// -1から始まり減少方向
    int nextKabeId = -1;

    /// 仮シマIDの採番用カウンタ
    int nextReservedShimaId = CellState::reservedShimaBase;

  private:
    // シマの一覧
    // 初期盤面読み込み時に数字のマスごとにひとつ作られる
    // 第0要素はダミー; 第一要素以降が実際のシマで、activeShimaList()で取得できる
    vector<Shima> allShimaList;

  public:
    Board(int w, int h) : width(w), height(h), cells(w, h), assumedFlags(w, h), allShimaList(1) {}

    DEFAULT_MOVE_CTORS(Board);

    string toString() const;

    auto activeShimaList() {
      return std::span(allShimaList.begin() + 1, allShimaList.end());
    }

    auto& shima(int id) {
      return allShimaList[id];
    }

    CellState cellAt(const Point& p) const {
      return inRange(p) ? cells.at(p.x, p.y) : CellStates::outOfBounds;
    }

    bool setCellAt(const Point& p, CellState value) {
      if (!inRange(p) || cellAt(p) == value) {
        return false;
      }
      cells.at(p.x, p.y) = value;
      return true;
    }

    /// 指定位置に数字を設定し、初期状態のシマを作成する
    void createShima(const Point& p, int n);

    ///
    /// 指定位置をカベ(黒マス)として確定する
    ///
    /// 事前条件:
    /// - 既にカベか盤面外であれば何もしない
    /// - 既にシマか仮シマであれば破綻
    /// 
    /// 手続き:
    /// - 盤面のマスにカベIDを設定し、連続するカベが同一IDになるように調整
    /// - 2x2のクラスタにリーチであれば、残り1マスを仮シマにする
    /// - 全シマに対してこのマスを取ることを禁止
    ///
    bool confirmKabe(const Point& p);

    ///
    /// 指定位置をIDで指定されたシマの領地として確定する
    ///
    /// 事前条件:
    /// - 既に別のシマで確定していたら破綻
    /// - 既にカベであれば破綻
    /// - 仮シマであれば上書きののち、連続する領地を奪う
    ///
    /// 手続き:
    /// - 盤面のマスにシマIDを設定
    /// - シマポテンシャルとその他の統計情報を更新
    /// - このマスと隣接するマスを他のシマが取ることを禁止
    ///
    bool confirmShima(const Point& p, int id);

    ///
    /// 指定位置をシマとして確保する(シマIDは後で確定する)
    ///
    /// 事前条件:
    /// - 既にシマまたは仮シマであれば何もしない
    /// - 既にカベであれば破綻
    /// - 異なる複数のシマに接していたら破綻
    ///
    /// 手続き:
    /// - 接しているシマがあればそれに加わる(奪われる)
    /// - そうでなければ盤面のマスに仮シマIDを設定する
    ///
    bool reserveShima(const Point& p);

    void setAssumed(const Point& p) {
      assumedFlags.at(p.x, p.y) = true;
    }

    Board duplicate() const {
      return { *this };
    }

  private:
    Board(const Board& other) = default;

    bool inRange(const Point& p) const {
      return 0 <= p.x && p.x < width && 0 <= p.y && p.y < height;
    }

    /// 2x2のクラスタにリーチであれば、残り1マスを仮シマにする
    bool avoidKabeCrusterAround(const Point& p);

    /// 指定したマスで既存のカベを接続する
    void mergeKabesAt(const Point& p);

    /// マス状態の値が from のものを to に置換する
    /// カベを統合する際のカベIDの置換に使用する
    bool replaceCellStates(int from, int to);

    /// 指定のマスをいずれのシマも取ることを禁止する
    bool rejectShimaAt(const Point& p);
  };

}