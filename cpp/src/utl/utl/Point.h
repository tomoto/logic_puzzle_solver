#pragma once

#include "Base.h"

#include <array>

namespace utl {

  /**
   * A 2D point with integral coordinates, can be a point or a vector.
   */
  template<std::integral T>
  struct PointBase {
    T x = 0;
    T y = 0;

    template<T ScaleX = 1, T ScaleY = ScaleX>
    constexpr PointBase<T> add(const PointBase<T>& p) const {
      return { x + p.x * ScaleX, y + p.y * ScaleY };
    }
    constexpr PointBase<T> addX(const PointBase<T>& p) const { return add<1, 0>(p); }
    constexpr PointBase<T> addY(const PointBase<T>& p) const { return add<0, 1>(p); }

    template<T ScaleX = 1, T ScaleY = ScaleX>
    constexpr PointBase<T> sub(const PointBase<T>& p) const {
      return { x - p.x * ScaleX, y - p.y * ScaleY };
    }
    constexpr PointBase<T> subX(const PointBase<T>& p) const { return sub<1, 0>(p); }
    constexpr PointBase<T> subY(const PointBase<T>& p) const { return sub<0, 1>(p); }

    constexpr PointBase<T> rrot() const {
      return { -y, x };
    }

    constexpr PointBase<T> lrot() const {
      return { y, -x };
    }

    constexpr PointBase<T> mul(T scale) const {
      return { x * scale, y * scale };
    }

    constexpr PointBase<T> neg() const {
      return { -x, -y };
    }

    constexpr auto manhattanDistance(const PointBase<T>& p) const {
      return std::abs(x - p.x) + std::abs(y - p.y);
    }

    constexpr bool valid() const {
      return x != invalid;
    }

    constexpr string toString() const {
      return "(" + std::to_string(x) + "," + std::to_string(y) + ")";
    }

    constexpr int hash() const {
      return x * 10007 + y;
    }

    auto operator<=>(const PointBase<T>&) const = default;

    static constexpr T invalid = std::numeric_limits<T>::min();
  };

  // template<std::integral T>
  // constexpr bool operator==(const PointBase<T>& p1, const PointBase<T>& p2)
  // {
  //   return p1.x == p2.x && p1.y == p2.y;
  // }

  // template<std::integral T>
  // constexpr bool operator<(const PointBase<T>& p1, const PointBase<T>& p2)
  // {
  //   return p1.y < p2.y || (p1.y == p2.y && p1.x < p2.x);
  // }

  template<std::integral T>
  struct PointHashBase {
    constexpr size_t operator()(const PointBase<T>& p) const {
      return p.hash();
    }
  };

  typedef PointBase<int> Point;
  typedef PointHashBase<int> PointHash;

  namespace Directions {

    const Point N = { 0, -1 };
    const Point NE = { 1, -1 };
    const Point E = { 1, 0 };
    const Point SE = { 1, 1 };
    const Point S = { 0, 1 };
    const Point SW = { -1, 1 };
    const Point W = { -1, 0 };
    const Point NW = { -1, -1 };

    const std::array<Point, 4> adjacents = { N, E, S, W };
    const std::array<Point, 4> corners = { NE, SE, SW, NW };
    const std::array<Point, 8> all = { N, NE, E, SE, S, SW, W, NW };

    template<typename F>
      requires std::convertible_to<bool, std::invoke_result_t<F, const Point&>>
    bool applyToAdjacents(const F& f) { return f(N) | f(E) | f(S) | f(W); }

    template<typename F>
      requires std::convertible_to<bool, std::invoke_result_t<F, const Point&>>
    bool applyToCorners(const F& f) { return f(NE) | f(SE) | f(SW) | f(NW); }
  } // namespace Directions

} // namespace utl
