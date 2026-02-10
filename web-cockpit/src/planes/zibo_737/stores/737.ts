import { ref } from 'vue'
import { defineStore } from 'pinia'

enum RawDisplayColor {
  BLACK = 0,
  CYAN = 1,
  RED = 2,
  YELLOW = 3,
  GREEN = 4,
  MAGENTA = 5,
  AMBER = 6,
  WHITE = 7,
}

enum RawDisplayFontSize {
  LARGE = 0,
  SMALL = 1,
}

const colorMap: Record<RawDisplayColor, string> = {
  [RawDisplayColor.BLACK]: 'text-black',
  [RawDisplayColor.CYAN]: 'text-cyan',
  [RawDisplayColor.RED]: 'text-red',
  [RawDisplayColor.YELLOW]: 'text-yellow',
  [RawDisplayColor.GREEN]: 'text-fmc-green',
  [RawDisplayColor.MAGENTA]: 'text-magenta',
  [RawDisplayColor.AMBER]: 'text-amber',
  [RawDisplayColor.WHITE]: 'text-white',
}

const sizeMap: Record<RawDisplayFontSize, string> = {
  [RawDisplayFontSize.LARGE]: 'text-[5.5cqi] tracking-[0.7cqi]',
  [RawDisplayFontSize.SMALL]: 'text-[4cqi] tracking-[1.35cqi]',
}

export const use737Store = defineStore('ZIBO_737', () => {
  const debug = ref(false)
  const lines = ref<string[]>([])
  const lineColors = ref<string[]>([])
  const charSizes = ref<string[]>([])

  function setDisplay(data: string, colors: RawDisplayColor[], sizes: RawDisplayFontSize[]) {
    lines.value = data.match(/.{1,24}/g) || []
    lineColors.value = colors.join('').match(/.{1,24}/g) || []
    charSizes.value = sizes.join('').match(/.{1,24}/g) || []
  }

  function getColor(line: number, char: number): string {
    const colorCode = lineColors.value[line]?.[char] || RawDisplayColor.GREEN
    const colorNum = parseInt(`${colorCode}`, 10) as RawDisplayColor
    return colorMap[colorNum]
  }

  function getSize(line: number, char: number): string {
    const sizeCode = charSizes.value[line]?.[char] || RawDisplayFontSize.LARGE
    const sizeNum = parseInt(`${sizeCode}`, 10) as RawDisplayFontSize
    return sizeMap[sizeNum]
  }

  function handleBridgeCommand(command: string, value: string | number): void {}

  return { lines, lineColors, charSizes, setDisplay, getSize, getColor, debug, handleBridgeCommand }
})
