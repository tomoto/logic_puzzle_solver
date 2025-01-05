import { Array2D } from '../../../src/util/Array2D'

describe('Array2D', () => {
  test('works', () => {
    const a = new Array2D<number>(10, 7)
    a.set(5, 5, 3.14)
    expect(a.get(5, 5)).toBe(3.14)
    expect(a.linearIndex(3, 3)).toBe(33)
  })
})
