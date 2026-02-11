import { computed, ref } from 'vue'
import { defineStore } from 'pinia'

enum LineSuffix {
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

const ColorMap: Record<LineSuffix, string> = {
  [LineSuffix.G]: 'text-fmc-green',
  [LineSuffix.GX]: 'text-fmc-green',
  [LineSuffix.I]: 'text-black bg-white',
  [LineSuffix.L]: 'text-white',
  [LineSuffix.LX]: 'text-white',
  [LineSuffix.M]: 'text-fuchsia',
  [LineSuffix.S]: 'text-white',
  [LineSuffix.X]: 'text-white',
  [LineSuffix.C]: 'text-cyan',
}

const largeSize = 'text-[5.5cqi] tracking-[0.54cqi]'
const smallSize = 'text-[4cqi] tracking-[1.16cqi]'

const FontSizeMap: Record<LineSuffix, string> = {
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

type LineCharData = {
  char: string
  color: string
  size: string
}

export const use737Store = defineStore('ZIBO_737', () => {
  const debug = ref(false)
  const rawLinesData = ref<Record<string, string>>({})
  const rawLineKeys = computed(() => Object.keys(rawLinesData.value))
  const lines = computed(() => {
    return Array.from({ length: 14 }, (_, i) => ' '.repeat(24)).map((emptyLine, index) => {
      const isMainLine = index % 2 === 0 || index === 0 || index === 13
      const rawLineNumber = (index % 2 === 0 ? index : index - 1) / 2
      const filteredRawLineKeys = rawLineKeys.value.filter(
        (lineKey) =>
          parseInt(lineKey.split('_')[0]!, 10) === rawLineNumber &&
          (isMainLine ? !lineKey.endsWith('X') : lineKey.endsWith('X')),
      )

      return emptyLine.split('').map((char, charIndex) => {
        const charData: LineCharData = {
          char, //default empty string ' '
          color: ColorMap[isMainLine ? LineSuffix.L : LineSuffix.X], // Default color
          size: FontSizeMap[isMainLine ? LineSuffix.L : LineSuffix.X],
        }
        return filteredRawLineKeys.reduce<LineCharData>((lastChartData, currentKey) => {
          const lineSuffix = currentKey.split('_')[1]! as LineSuffix
          const currentChar = rawLinesData.value[currentKey]![charIndex] || ' '

          if (currentChar === ' ') return lastChartData

          return {
            char: currentChar,
            color: ColorMap[lineSuffix],
            size: FontSizeMap[lineSuffix],
          }
        }, charData)
      })
    })
  })

  function setRawLine(lineKey: string, value: string) {
    rawLinesData.value[lineKey] = value
  }

  function handleBridgeCommand(command: string, value: string | number): void {
    if (command.startsWith('fmc_line-')) {
      const lineKey = command.split('-').pop() as string
      setRawLine(lineKey, value.toString())
    }
  }

  return { lines, setRawLine, debug, handleBridgeCommand }
})
