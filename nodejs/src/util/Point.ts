import { Directions, Vector } from './Vector'

export interface Point {
  x: number
  y: number
}

export const Point = {
  at,
  equals,
  toString,
  tuplize,
  inRange,
  adjacent4,
  manhattanDistance,
} as const

function at(x: number, y: number): Point {
  return { x, y }
}

function equals(p1: Point, p2: Point): boolean {
  return p1.x === p2.x && p1.y === p2.y
}

function toString(p: Point): string {
  return `${p.x},${p.y}`
}

function tuplize(p: Point): [number, number] {
  return [p.x, p.y]
}

function inRange(
  x: number,
  y: number,
  xmin: number,
  xmax: number,
  ymin: number,
  ymax: number,
): boolean {
  return x >= xmin && x < xmax && y >= ymin && y < ymax
}

function adjacent4(x: number, y: number): Vector[] {
  return Directions[4].map(v => v.add(x, y))
}

function manhattanDistance(p1: Point, p2: Point): number {
  return Math.abs(p1.x - p2.x) + Math.abs(p1.y - p2.y)
}
