export function ensureArray<Type>(value: Type | Type[]) {
  return Array.isArray(value) ? value : [value]
}
