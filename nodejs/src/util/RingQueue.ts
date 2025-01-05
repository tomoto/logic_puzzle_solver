import { Optional } from './Utils'

export class RingQueue<T> {
  private queue: Optional<T>[] = []
  private head: number = 0 // 0 <= head < capacity
  private tail: number = 0 // 0 <= tail < capacity
  private _length: number = 0

  constructor(private capacity: number) {}

  /**
   * キューに値を追加する
   * 容量を越えた場合は例外を投げる
   */
  push(value: T): void {
    if (this.length === this.capacity) {
      throw new Error('RingQueue is full')
    }

    this.queue[this.tail] = value
    this.increment('tail')
    this._length++
  }

  get length(): number {
    return this._length
  }

  get full(): boolean {
    return this._length === this.capacity
  }

  shift(): Optional<T> {
    if (this.length === 0) {
      return undefined
    }

    const value = this.queue[this.head]
    this.queue[this.head] = undefined

    this.increment('head')
    this._length--
    return value
  }

  private increment(property: 'head' | 'tail'): void {
    if (this[property] === this.capacity - 1) {
      this[property] = 0
    } else {
      this[property]++
    }
  }
}
