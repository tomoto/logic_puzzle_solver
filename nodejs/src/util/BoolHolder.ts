export class BoolHolder {
  public value = false

  accumulate(x: boolean | (() => boolean)): void {
    if (typeof x !== 'boolean') {
      x = x()
    }
    this.value ||= x
  }
}
