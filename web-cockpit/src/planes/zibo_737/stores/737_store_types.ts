export enum LineSuffix {
  G = 'G',
  GX = 'GX',
  I = 'I',
  L = 'L',
  LX = 'LX',
  M = 'M',
  S = 'S',
  X = 'X',
  C = 'C',
}

export const ColorMap: Record<LineSuffix, string> = {
  [LineSuffix.G]: 'text-fmc-green',
  [LineSuffix.GX]: 'text-fmc-green',
  [LineSuffix.I]: 'text-black bg-white',
  [LineSuffix.L]: 'text-white',
  [LineSuffix.LX]: 'text-white',
  [LineSuffix.M]: 'text-fuchsia-500',
  [LineSuffix.S]: 'text-white',
  [LineSuffix.X]: 'text-white',
  [LineSuffix.C]: 'text-cyan-500',
}

const largeSize = 'text-[5.5cqi] tracking-[0.54cqi]'
const smallSize = 'text-[4cqi] tracking-[1.16cqi]'

export const FontSizeMap: Record<LineSuffix, string> = {
  [LineSuffix.G]: largeSize,
  [LineSuffix.GX]: smallSize,
  [LineSuffix.I]: largeSize,
  [LineSuffix.L]: largeSize,
  [LineSuffix.LX]: smallSize,
  [LineSuffix.M]: largeSize,
  [LineSuffix.S]: smallSize,
  [LineSuffix.X]: smallSize,
  [LineSuffix.C]: largeSize,
}

export type LineCharData = {
  char: string
  color: string
  size: string
}

export type NavFrequency = {
  value: string
  mode: NavFrequencyMode
  cursor: number
}

export type NavFrequencies = {
  active: NavFrequency
  standby: NavFrequency
}

export enum NavFrequencyMode {
  VOR = 'VOR',
  ILS = 'ILS',
  GLS = 'GLS',
}

export const NavFrequencyModes = [NavFrequencyMode.VOR, NavFrequencyMode.ILS, NavFrequencyMode.GLS]
