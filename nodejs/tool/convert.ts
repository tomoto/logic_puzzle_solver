import * as readline from 'readline'
import { SlilinLoader } from '../src/slilin/serializer/SlilinLoader'
import { SlilinFormatter } from '../src/slilin/serializer/SlilinFormatter'

/**
 * 数字だけのコンパクト形式の盤面から格子状のフル形式の盤面を生成する。
 */
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
  terminal: false,
})

const lines: string[] = []

rl.on('line', (line: string) => {
  line = line.trim()
  if (!line.startsWith('#')) {
    lines.push(line)
  }
})

rl.on('close', () => {
  const board = SlilinLoader.loadCompact(lines)
  console.log('[')
  console.log(
    SlilinFormatter.formatAscii(board)
      .split('\n')
      .map(line => "'" + line + "',")
      .join('\n'),
  )
  console.log(']')
})
