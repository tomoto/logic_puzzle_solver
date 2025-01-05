import { RingQueue } from './RingQueue'

export class Deduplicator<T> {
  public set = new Set<T>()

  hasAndAdd(value: T): boolean {
    if (this.set.has(value)) {
      return true
    } else {
      this.set.add(value)
      return false
    }
  }
}

export class LimitedDeduplicator<T> {
  public set = new Set<T>()
  private queue: RingQueue<T>

  constructor(private capacity: number) {
    this.queue = new RingQueue(capacity)
  }

  hasAndAdd(value: T): boolean {
    if (this.set.has(value)) {
      return true
    }

    if (this.queue.full) {
      const removed = this.queue.shift() as T
      this.set.delete(removed)
    }

    this.set.add(value)
    this.queue.push(value)
    return false
  }
}
