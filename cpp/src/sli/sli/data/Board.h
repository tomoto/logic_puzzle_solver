#pragma once

#include "Base.h"
#include "Cell.h"
#include "Helper.h"
#include "utl/Array2D.h"
#include "utl/Point.h"

#include <vector>
#include <string>
#include <memory>

namespace sli {

  struct Board {
    typedef vector<bool> Fingerprint;

    /// 数字のマスで数えた盤面の幅
    int logicalWidth;
    /// 数字のマスで数えた盤面の高さ
    int logicalHeight;

    /// 数字、ドット、線、番人も含めた内部表現で数えた盤面の幅
    /// (= 数字のマスで数えた幅 x 2 + 1)
    ///
    /// 内部表現はこんな感じ:
    ///  X X X X X
    /// X. . . . .X
    ///   3   1
    /// X. . . . .X
    ///     2
    /// X. . . . .X
    ///  X X X X X
    ///
    int physicalWidth;
    /// 内部表現で数えた盤面の高さ
    int physicalHeight;

    /// 数字のマスの情報
    /// -1 は空白
    /// 外側に上下左右ひとマスずつ余白が持たせてある
    ///
    /// @see Helper::numberIndex 内部座標から本配列のインデックスへの変換
    Array2D<int> numbers;

    /// 線の状態
    /// sentinelも含めて直列にパッキングされている
    ///
    /// @see Helper::lineIndex 内部座標から本配列のインデックスへの変換
    vector<LineCellInfo> lines;

    /// 数値・ドットのマスの周りの線の状態
    struct SurroundingStats {
      uint16_t exists = 0; /// 引かれた線の数
      uint16_t forbidden = 0; /// 線が禁止されている数
      bool allFixed() const { return exists + forbidden == 4; } /// 周囲がすべて確定しているか
    };
    Array2D<SurroundingStats> surroundingStats;

    /// 変更があったか
    struct DirtyFlags {
      Array2D<bool> cell; /// セル粒度の情報
      vector<bool> line; /// 行粒度の情報
      DirtyFlags(int w, int h) :cell(w, h, true), line(h, true) {}
    } dirtyFlags, dirtyFlagsBuffer; // ダブルバッファリング

    Board(int logicalWidth, int logicalHeight);
    DEFAULT_MOVE_CTORS(Board);

    void clearState();

    int& numberAt(int x, int y) {
      using namespace Helper;
      assert(cellTypeAt(x, y) == CellType::number);
      return numbers.data[numberIndex(x, y, physicalWidth)];
    }

    int& numberAt(const Point& p) {
      return numberAt(p.x, p.y);
    }

    LineCellInfo& lineAt(int x, int y) {
      using namespace Helper;
      assert(cellTypeAt(x, y) == CellType::hline || cellTypeAt(x, y) == CellType::vline);
      return lines[lineIndex(x, y, physicalWidth)];
    }

    LineCellInfo& lineAt(const Point& p) {
      return lineAt(p.x, p.y);
    }

    bool setExistsAt(int x, int y);

    bool setExistsAt(const Point& p) {
      return setExistsAt(p.x, p.y);
    }

    bool setForbiddenAt(int x, int y);

    bool setForbiddenAt(const Point& p) {
      return setForbiddenAt(p.x, p.y);
    }

    SurroundingStats& surroundingStatsAt(int x, int y) {
      return surroundingStats.at(x, y);
    }

    bool setExistsAround(int x, int y) {
      // performance critical
      return (!lineAt(x - 1, y).fixed() && setExistsAt(x - 1, y))
        | (!lineAt(x + 1, y).fixed() && setExistsAt(x + 1, y))
        | (!lineAt(x, y - 1).fixed() && setExistsAt(x, y - 1))
        | (!lineAt(x, y + 1).fixed() && setExistsAt(x, y + 1));
    }

    bool setForbiddenAround(int x, int y) {
      // performance critical
      return (!lineAt(x - 1, y).fixed() && setForbiddenAt(x - 1, y))
        | (!lineAt(x + 1, y).fixed() && setForbiddenAt(x + 1, y))
        | (!lineAt(x, y - 1).fixed() && setForbiddenAt(x, y - 1))
        | (!lineAt(x, y + 1).fixed() && setForbiddenAt(x, y + 1));
    }

    bool dirtyAt(int x, int y) {
      return dirtyFlags.cell.at(x, y);
    }

    bool dirtyAtLine(int y) {
      return dirtyFlags.line[y];
    }

    Board duplicate() const {
      return { *this };
    }

    bool noSmallLoopsAllowed() const;

  private:
    Board(const Board& other) = default;

  };

}
