#include "Board.h"

namespace nur {


  void Board::createShima(const Point& p, int n) {
    assert(cellAt(p).open());
    int id = allShimaList.size();
    allShimaList.push_back({ id, n, p, width, height });

    // 自分の領地として確定
    confirmShima(p, id);

    // 既存のシマの領地をrespectする
    for (auto& otherShima : activeShimaList()) {
      if (id != otherShima.id) {
        shima(id).rejectAround(otherShima.center);
      }
    }
  }

  /// 指定位置をカベ(黒マス)として確定する
  bool Board::confirmKabe(const Point& p) {
    auto cell = cellAt(p);

    if (cell.kabeOrOOB()) {
      // 既にカベなら何もしない
      return false;
    } else if (!cell.open()) {
      // 既にシマに決まっていたので破綻
      throw cmn::HatanException("Cannot change a shima to kabe.");
    }

    // 既存のカベとの接続を確認し、新たなカベIDを決定
    mergeKabesAt(p);

    // 2x2のクラスタにリーチであれば、残り1マスを仮シマにする
    avoidKabeCrusterAround(p);

    // シマにカベを避けるよう指示する
    rejectShimaAt(p);

    return true; // 変更ありを返す
  }

  /// 指定のマスをいずれのシマも取ることを禁止する
  bool Board::rejectShimaAt(const Point& p) {
    return evalAllOr(activeShimaList(), [&](Shima& shima) {
      return shima.reject(p);
      });
  }

  /// 指定したマスの周囲のカベをチェックし、2x2のクラスタができないかをチェックする
  bool Board::avoidKabeCrusterAround(const Point& p) {
    auto flags = std::accumulate(
      Directions::all.begin(),
      Directions::all.end(),
      0,
      [&](int flags, const Point& d) {
        return (flags << 1) | cellAt(p.add(d)).kabe();
      }
    );

    bool changed = false;

    for (int i : range(0, 8, 2)) {
      /// N/NE/Eの三方向をチェック
      if (testAllBits(flags, 0xe0)) { /// 3方向すべてがカベなら破綻
        throw cmn::HatanException("Kabes are creating 2x2 cluster.");
      } else if (testAllBits(flags, 0x60)) { // NE/EがカベならNをおさえる
        changed |= reserveShima(p.add(Directions::all[i]));
      } else if (testAllBits(flags, 0xa0)) { // N/EがカベならNEをおさえる
        changed |= reserveShima(p.add(Directions::all[i + 1]));
      } else if (testAllBits(flags, 0xc0)) { // N/NEがカベならEをおさえる
        changed |= reserveShima(p.add(Directions::all[(i + 2) % 8]));
      }
      flags = (flags << 2) | (flags >> 6); // 90度回転
    }

    return changed;
  }

  /// 指定したマスで既存のカベを接続する
  void Board::mergeKabesAt(const Point& p) {
    assert(cellAt(p).open()); // 指定場所がopenでないケースは呼び出し側でガード済み

    int newKabeId = 0;
    // 隣接マスの状態を調べる
    for (auto& d : Directions::adjacents) {
      auto cell = cellAt(p.add(d));
      if (cell.kabe()) {
        if (newKabeId == 0) {
          // 最初のカベが見つかったらそのIDを採用
          newKabeId = cell.kabeId();
        } else {
          // 2つ目以降のカベが見つかったらIDを揃える
          replaceCellStates(cell.kabeId(), newKabeId);
        }
      }
    }

    if (newKabeId == 0) {
      // 孤立したカベなら新たなIDを割り当てる
      newKabeId = nextKabeId--;
    }

    // カベIDを設定し、シマ状態を更新
    setCellAt(p, CellStates::kabe(newKabeId));
  }

  /// マス状態の値が from のものを to に置換する
  bool Board::replaceCellStates(int from, int to) {
    if (from == to) {
      return false;
    }

    bool changed = false;
    for (auto& cell : cells.data) {
      if (cell.value == from) {
        cell.value = to;
        changed = true;
      }
    }
    return changed;
  }

  bool Board::reserveShima(const Point& p) {
    if (cellAt(p).kabe()) {
      // 既にカベであれば破綻
      throw cmn::HatanException("Cannot change a kabe to shima.");
    } else if (!cellAt(p).open()) {
      // 既にシマであれば何もしない
      return false;
    }

    // 別のシマに接していたらjoinする
    int newReservedShimaId = 0;
    for (auto& d : Directions::adjacents) {
      auto cell = cellAt(p.add(d));

      // 確定したシマに接していたらjoinし、即リターンする
      if (cell.confirmedShima()) {
        return confirmShima(p, cell.shimaId());
      }

      // 未確定の島に接していたらIDを統一する
      if (cell.reservedShima()) {
        if (newReservedShimaId == 0) {
          newReservedShimaId = cell.shimaId();
        } else {
          replaceCellStates(cell.shimaId(), newReservedShimaId);
        }
      }
    }

    // 孤立していたので新たなIDを割り当てる
    if (newReservedShimaId == 0) {
      newReservedShimaId = nextReservedShimaId;
    }

    // 未確定の島としてマーク
    return setCellAt(p, CellStates::shima(newReservedShimaId));
  }

  bool Board::confirmShima(const Point& p, int id) {
    CellState s = cellAt(p);

    // 事前チェック
    if (s.kabe()) {
      // 既にカベであれば破綻
      throw cmn::HatanException("Cannot change a kabe to shima.");
    } else if (s.confirmedShima()) {
      if (s.shimaId() != id) {
        // 既に別のシマの領地であれば破綻
        throw cmn::HatanException("Already taken by another shima.");
      } else {
        // 自分自身の領地であれば何もしない
        return false;
      }
    }

    // このマスにシマIDを設定
    setCellAt(p, CellStates::shima(id));

    // シマに対して領地確定処理
    auto& targetShima = shima(id);
    targetShima.confirm(p);

    // このマスおよび隣接するマスを他のシマが取ることを禁止
    for (auto& otherShima : activeShimaList()) {
      if (id != otherShima.id) {
        otherShima.rejectAround(p);
      }
    }

    // reservedなシマに接していたらそれも奪う
    // (これはやらなくてもSPOverwrapRuleでやってくれるはず)
    //
    // for (auto& d : Directions::adjacents) {
    //   auto q = p.add(d);
    //   if (cellAt(q).reservedShima()) {
    //     confirmShima(q, id);
    //   }
    // }

    return true;
  }
}
