export type Optional<T> = T | undefined

export function or(...values: boolean[]): boolean {
  return values.some(e => e)
}

/**
 * 配列の各要素をブール値を返すラムダ式で評価し、その結果を論理和する
 * 全ての要素を評価したい場合にsomeの代わりに使用する
 */
export function evalAllOr<T>(
  xs: T[],
  f: (x: T, i: number, a: T[]) => boolean,
  init: boolean = false,
): boolean {
  return xs.reduce((acc, x, i, a) => f(x, i, a) || acc, init)
}

/**
 * [begin, end)の範囲の値をstep刻みで生成するジェネレータ
 */
export function* range(
  begin: number,
  end: number,
  step: number = 1,
): Generator<number> {
  for (let i = begin; i < end; i += step) {
    yield i
  }
}

/**
 * mapのgenerator版
 */
export function* map<T, R>(
  generator: Generator<T>,
  func: (x: T) => R,
): Generator<R> {
  for (const value of generator) {
    yield func(value)
  }
}

export function throwError(e?: string | Error): never {
  if (e === undefined) {
    e = new Error('Should not be reached')
  }

  if (typeof e === 'string') {
    e = new Error(e)
  }
  throw e
}
