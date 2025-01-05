#pragma once

#include "nur/data/Board.h"

#include <map>
#include <set>

namespace nur {

  struct Topology {
    // カベやシマに接する開放点(カベでもシマでもないマス)の集合
    // カベやシマのIDごとにグループ化されている
    std::map<int, std::set<Point>> shimaOpenPoints;
    std::map<int, std::set<Point>> kabeOpenPoints;

    int openArea = 0; // 開放点の数

    Topology() = default;
    DEFAULT_MOVE_CTORS(Topology);

    static Topology analyze(Board& b) {
      Topology result;

      // 盤面全体のトポロジー分析
      for (auto y : range(0, b.height)) {
        for (auto x : range(0, b.width)) {
          Point p = { x, y };
          auto cell = b.cellAt(p);

          // 開放点は数だけ数えて次に進む
          if (cell.open()) {
            result.openArea++;
            continue;
          }

          // カベやシマが存在
          // 開放点セットを作成または取得
          auto& points =
            (cell.kabe()
              ? result.kabeOpenPoints
              : result.shimaOpenPoints
              )[cell.value];

          // その周囲の開放点を探し、登録する
          for (auto& d : Directions::adjacents) {
            auto q = p.add(d);
            auto adjacentCell = b.cellAt(q);
            if (adjacentCell.open()) {
              points.insert(q);
            } else {
              // TODO 念の為の破綻チェックを入れてもいい
            }
          }
        }
      }

      return std::move(result);
    }
  };

}