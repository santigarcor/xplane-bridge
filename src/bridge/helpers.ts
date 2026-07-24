export function ensureArray<Type>(value: Type | Type[]) {
  return Array.isArray(value) ? value : [value]
}

/**
 * Composable value parsers, meant to be combined with `pipe` when a
 * dataref's parser needs more than one step (e.g. decoding an array
 * dataref before picking an element out of it). Ordering is left to the
 * caller since it depends on the shape of the raw value: an encoded
 * value must be decoded before indexing, while a plain numeric array can
 * be indexed directly.
 */
export const toBoolean = (v: any) => (v > 0 ? 1 : 0)

export const decodeBase64 = (v: string) =>
  Buffer.from(v, 'base64').toString('utf-8')

export const atIndex = (index: number) => (v: any) =>
  Array.isArray(v) ? v[index] : v

export const pipe =
  (...fns: Array<(v: any) => any>) =>
  (v: any) =>
    fns.reduce((acc, fn) => fn(acc), v)
