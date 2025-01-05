export type CompletionStatus = 'complete' | 'incomplete' | 'hatan'

// mutable
export interface BoardState<B, P> {
  board: B
  topology: P
  status: CompletionStatus
}

export interface BoardStateTrial<B, P, A> extends BoardState<B, P> {
  assumption: A
}
