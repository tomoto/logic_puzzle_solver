import { Point } from './Point'

export class Vector implements Point {
  constructor(
    readonly x = 0,
    readonly y = 0,
  ) {}

  add(x: number, y: number, a: number = 1): Vector {
    return new Vector(this.x + x * a, this.y + y * a)
  }

  addP(p: Point, a: number = 1): Vector {
    return this.add(p.x, p.y, a)
  }

  mul(a: number): Vector {
    return new Vector(this.x * a, this.y * a)
  }

  /**
   * Rotate 90 degrees to the right
   * @returns new Vector
   */
  rrot(): Vector {
    return new Vector(-this.y, this.x)
  }

  /**
   * Rotate 90 degrees to the left
   * @returns new Vector
   */
  lrot(): Vector {
    return new Vector(this.y, -this.x)
  }

  toString(): string {
    return `[${this.x},${this.y}]`
  }

  static atP(p: Point): Vector {
    return new Vector(p.x, p.y)
  }
}

export const Direction = {
  N: new Vector(0, -1),
  NE: new Vector(1, -1),
  E: new Vector(1, 0),
  SE: new Vector(1, 1),
  S: new Vector(0, 1),
  SW: new Vector(-1, 1),
  W: new Vector(-1, 0),
  NW: new Vector(-1, -1),
} as const

export const Directions = {
  /**
   * 4-way directions, starting from north and clockwise
   */
  [4]: [Direction.N, Direction.E, Direction.S, Direction.W],

  /**
   * 8-way directions, starting from north and clockwise
   */
  [8]: [
    Direction.N,
    Direction.NE,
    Direction.E,
    Direction.SE,
    Direction.S,
    Direction.SW,
    Direction.W,
    Direction.NW,
  ],
} as const
