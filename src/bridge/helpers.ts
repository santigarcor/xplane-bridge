import { ParserType } from './types.js'

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

export const round = (v: any) => Math.round(v)

export const toDegrees = (v: any) => Math.round(v * (180 / Math.PI))

export const valueMap = (v: any, map: any) => (v: any) =>
  map && map[v] !== undefined ? map[v] : v

export const none = (v: any) => v

export const decodeBase64 = (v: string) =>
  Buffer.from(v, 'base64').toString('utf-8')

export const atIndex = (index: number) => (v: any) =>
  Array.isArray(v) ? v[index] : v

export const pipe =
  (...fns: Array<(v: any) => any>) =>
  (v: any) =>
    fns.reduce((acc, fn) => fn(acc), v)

export const parserLibrary: Record<ParserType, (v: any, extra?: any) => any> = {
  [ParserType.BOOLEAN]: toBoolean,
  [ParserType.ROUND]: round,
  [ParserType.TO_DEGREES]: toDegrees,
  [ParserType.VALUE_MAP]: valueMap,
  [ParserType.NONE]: none,
  [ParserType.BASE64DECODE]: decodeBase64,
}
