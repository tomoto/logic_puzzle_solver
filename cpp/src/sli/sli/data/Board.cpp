#include "Board.h"
#include "utl/Range.h"

namespace sli {
  /// 盤面の外側に番人(禁止された線)を配置する
  static void initSentinel(Board& b) {
    for (int y : range(1, b.physicalHeight, 2)) {
      b.setForbiddenAt(0, y);
      b.setForbiddenAt(b.physicalWidth - 1, y);
    }
    for (int x : range(1, b.physicalWidth, 2)) {
      b.setForbiddenAt(x, 0);
      b.setForbiddenAt(x, b.physicalHeight - 1);
    }
  }

  Board::Board(int logicalWidth, int logicalHeight) :
    logicalWidth(logicalWidth),
    logicalHeight(logicalHeight),
    physicalWidth(logicalWidth * 2 + 3),
    physicalHeight(logicalHeight * 2 + 3),
    numbers(logicalWidth + 2, logicalHeight + 2, NumberCell::empty),
    lines(physicalWidth* physicalHeight / 2),
    surroundingStats(physicalWidth, physicalHeight),
    dirtyFlags(physicalWidth, physicalHeight),
    dirtyFlagsBuffer(physicalWidth, physicalHeight)
  {
    initSentinel(*this);
  }

  void Board::clearState() {
    fill(lines, LineCellInfo());
    fill(surroundingStats.data, SurroundingStats());
    fill(dirtyFlags.cell.data, true);
    fill(dirtyFlags.line, true);
    fill(dirtyFlagsBuffer.cell.data, true);
    fill(dirtyFlagsBuffer.line, true);
    initSentinel(*this);
  }

  /// 変更箇所の周囲(全体で5x5)を変更影響ありとマークする
  static void dirtify(Board& b, int x, int y) {
    int top = std::max(y - 2, 0);
    int bottom = std::min(y + 2, b.physicalHeight - 1);
    int left = std::max(x - 2, 0);
    int right = std::min(x + 2, b.physicalWidth - 1);
    for (int y : range(top, bottom + 1)) {
      b.dirtyFlags.line[y] = true;
      b.dirtyFlagsBuffer.line[y] = true;
      for (int x : range(left, right + 1)) {
        b.dirtyFlags.cell.at(x, y) = true;
        b.dirtyFlagsBuffer.cell.at(x, y) = true;
      }
    }
  }

  /// 線を引いた際に周辺の統計情報を更新する
  static void addExistsOnNeighbors(Board& b, int x, int y) {
    // performance sensitive
    if (y > 0) {
      b.surroundingStats.at(x, y - 1).exists++;
    }
    if (y < b.physicalHeight - 1) {
      b.surroundingStats.at(x, y + 1).exists++;
    }
    if (x > 0) {
      b.surroundingStats.at(x - 1, y).exists++;
    }
    if (x < b.physicalWidth - 1) {
      b.surroundingStats.at(x + 1, y).exists++;
    }
  }

  /// 指定座標に線を引く
  bool Board::setExistsAt(int x, int y) {
    auto& line = lineAt(x, y);
    if (line.exists) {
      return false; // 既に線があるので変更なし
    }
    if (line.forbidden) { // 禁止されていたら破綻
      throw HatanException("Cannot set exists to true when forbidden is true");
    }
    line.exists = true; // 状態更新
    dirtify(*this, x, y); // 変更影響ありフラグを立てる
    addExistsOnNeighbors(*this, x, y); // 周囲の統計情報の更新
    return true; // 変更ありを返す
  }

  /// 線を更新した際に周辺の統計情報を更新する
  static void addForbiddenOnNeighbors(Board& b, int x, int y) {
    // performance sensitive
    if (y > 0) {
      b.surroundingStats.at(x, y - 1).forbidden++;
    }
    if (y < b.physicalHeight - 1) {
      b.surroundingStats.at(x, y + 1).forbidden++;
    }
    if (x > 0) {
      b.surroundingStats.at(x - 1, y).forbidden++;
    }
    if (x < b.physicalWidth - 1) {
      b.surroundingStats.at(x + 1, y).forbidden++;
    }
  }

  bool Board::setForbiddenAt(int x, int y) {
    auto& line = lineAt(x, y);
    if (line.forbidden) {
      return false; // 既に禁止されているので変更なし
    }
    if (line.exists) { // 線が書かれていたら破綻
      throw HatanException("Cannot set forbidden to true when exists is true");
    }
    line.forbidden = true; // 状態更新
    dirtify(*this, x, y); // 変更影響ありフラグを立てる
    addForbiddenOnNeighbors(*this, x, y); // 周囲の統計情報の更新
    return true; // 変更ありを返す
  }

  /// 小ループ禁止の盤面かどうかを判断する
  bool Board::noSmallLoopsAllowed() const {
    // 小ループが作れる最大の数の合計は12なので、数の合計が12を越えたら禁止と判断
    //
    //   1 1
    //  +-+-+
    // 1|3 3|1
    //  +-+-+
    //   1 1
    //
    const int threshold = 12;
    int total = 0;
    for (int n : numbers.data) {
      total += std::max(0, n);
      if (total > threshold) {
        return true; // 禁止
      }
    }
    return false; // 小ループの可能性が排除できない
  }

} // namespace sli
