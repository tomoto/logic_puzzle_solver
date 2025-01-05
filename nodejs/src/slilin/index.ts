import readline from 'readline'
import yargs from 'yargs'
import { hideBin } from 'yargs/helpers'
import { SlilinBoard } from './data/SlilinBoard'
import { SlilinFormatter } from './serializer/SlilinFormatter'
import { SlilinLoader } from './serializer/SlilinLoader'
import { SlilinBoardState, SlilinSolver } from './solver/SlilinSolver'

function solve(
  text: string[],
  options: {
    speed: number
  },
): void {
  const board = SlilinLoader.loadCompact(text)
  let count = 0

  const clearLine = '\x1b[2K\r'

  function printBoardState(
    b: SlilinBoard,
    depth: number,
    opt?: {
      moveToHomeFirst?: boolean
      showDirty?: boolean
    },
  ): void {
    if (opt?.moveToHomeFirst) {
      console.log(`\x1b[${b.physicalHeight}A`)
    }
    console.log(SlilinFormatter.formatPretty(b, { showDirty: opt?.showDirty }))
    console.log(
      clearLine +
        `count: ${count} depth: ${depth} fingerprints: ${solver.engine.fingerprintSet.set.size} `,
    )
  }

  function reportProgress(state: SlilinBoardState, depth: number): void {
    count++
    if (options.speed && count % options.speed === 0) {
      printBoardState(state.board, depth, {
        moveToHomeFirst: true,
        showDirty: true,
      })
    }
  }

  const solver = new SlilinSolver({ reportProgress })

  printBoardState(board, 0)

  // 実行時間計測開始
  console.time('Elapsed time')
  // 解く
  const result = solver.solve(board)
  // 最終状態を表示
  printBoardState(result.board, 0, { moveToHomeFirst: true })
  console.log(result.status)
  console.timeEnd('Elapsed time')
  console.log()
}

const argv = yargs(hideBin(process.argv))
  .version('1.0')
  .options('s', {
    alias: 'speed',
    type: 'number',
    description:
      'Speed of screen update (smaller is faster, zero is no update)',
    default: 100,
  })
  .help()
  .usage(
    'This script solves the slither link problem given to the stdin. Enjoy!',
  )
  .parseSync()

const lines: string[] = []

const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout,
  terminal: false,
})

rl.on('line', line => {
  line = line.trim()
  if (!line.startsWith('#')) {
    lines.push(line)
  }
})

rl.on('close', () => {
  solve(lines, {
    speed: argv.s,
  })
})
