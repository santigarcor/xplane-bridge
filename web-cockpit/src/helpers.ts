export const isPair = (number: number) => {
  return number % 2 === 0
}

export const parseFmcString = (raw: string): string => {
  if (!raw) return ''

  return raw
    .replace(/\x1C/g, '°') //  -> Grado
    .replace(/\x1D/g, '□') //  -> Cuadrado (Placeholder)
    .replace(/\x1F/g, '↕') //  -> Flecha (si aplica)
    .replace(/</g, '‹') // Opcional: < más elegante para LSK
    .replace(/>/g, '›') // Opcional: > más elegante para LSK
}
