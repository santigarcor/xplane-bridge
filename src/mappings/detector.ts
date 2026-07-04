import type { SupportedAircraft } from './types.js'

const PLANE_PATTERNS: { pattern: RegExp; plane: SupportedAircraft }[] = [
  { pattern: /737/i, plane: 'zibo_737' },
  { pattern: /757/i, plane: 'ff_757' },
  { pattern: /777/i, plane: 'ff_777' },
  { pattern: /Cessna_172SP_G1000/i, plane: 'c172_g1000' },
  { pattern: /Cessna_172SP.acf/i, plane: 'c172' },
]

export function detectPlane(liveryPath: string): SupportedAircraft | null {
  for (const { pattern, plane } of PLANE_PATTERNS) {
    if (pattern.test(liveryPath)) return plane
  }
  return null
}
