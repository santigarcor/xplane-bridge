export enum RawDisplayColor {
  BLACK = 0,
  CYAN = 1,
  RED = 2,
  YELLOW = 3,
  GREEN = 4,
  MAGENTA = 5,
  AMBER = 6,
  WHITE = 7,
}

export enum RawDisplayFontSize {
  LARGE = 0,
  SMALL = 1,
}

export const ColorMap: Record<RawDisplayColor, string> = {
  [RawDisplayColor.BLACK]: 'text-black',
  [RawDisplayColor.CYAN]: 'text-cyan',
  [RawDisplayColor.RED]: 'text-red',
  [RawDisplayColor.YELLOW]: 'text-yellow',
  [RawDisplayColor.GREEN]: 'text-fmc-green',
  [RawDisplayColor.MAGENTA]: 'text-magenta',
  [RawDisplayColor.AMBER]: 'text-amber',
  [RawDisplayColor.WHITE]: 'text-white',
}

export const SizeMap: Record<RawDisplayFontSize, string> = {
  [RawDisplayFontSize.LARGE]: 'text-[5.5cqi] tracking-[0.7cqi]',
  [RawDisplayFontSize.SMALL]: 'text-[4cqi] tracking-[1.35cqi]',
}
