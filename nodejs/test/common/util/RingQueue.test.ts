import { RingQueue } from '../../../src/util/RingQueue'

describe('RingQueue', () => {
  test('can push and shift', () => {
    const q = new RingQueue<number>(3)
    q.push(1)
    q.push(2)
    expect(q.full).toBe(false)
    q.push(3)
    expect(q.full).toBe(true)
    expect(q.shift()).toBe(1)
    expect(q.shift()).toBe(2)
    q.push(4)
    expect(q.shift()).toBe(3)
    expect(q.shift()).toBe(4)
    expect(q.shift()).toBe(undefined)
  })
})
