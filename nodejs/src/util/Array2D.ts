import assert from 'assert'
import { map, range } from './Utils'

export class Array2D<T> {
  readonly width: number
  readonly height: number
  readonly elements: T[]

  constructor(w: number, h: number, initialValue?: T) {
    this.width = w
    this.height = h
    this.elements = Array(w * h).fill(initialValue)
  }

  get(x: number, y: number): T {
    this.validatePosition(x, y)
    return this.elements[x + y * this.width]
  }

  set(x: number, y: number, value: T): void {
    this.validatePosition(x, y)
    this.elements[x + y * this.width] = value
  }

  linearIndex(x: number, y: number): number {
    this.validatePosition(x, y)
    return x + y * this.width
  }

  slices(): T[][] {
    return Array.from(
      map(range(0, this.height), y =>
        this.elements.slice(y * this.width, (y + 1) * this.width),
      ),
    )
  }

  copyFrom(other: Array2D<T>): void {
    assert(this.elements.length === other.elements.length)
    other.elements.forEach((e, i) => (this.elements[i] = e))
  }

  private validatePosition(x: number, y: number): void {
    if (x < 0 || x >= this.width || y < 0 || y >= this.height) {
      throw Error(
        `The potision (${x},${y}) is out of range (${this.width},${this.height})`,
      )
    }
  }
}
