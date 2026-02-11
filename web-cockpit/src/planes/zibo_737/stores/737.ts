import { computed, ref } from 'vue'
import { defineStore } from 'pinia'
import { parseFmcString } from '@/helpers'
import { ColorMap, FontSizeMap, LineSuffix, type LineCharData } from './737_store_types'

export const use737Store = defineStore('ZIBO_737', () => {
  const debug = ref(false)
  const rawLinesData = ref<Record<string, string>>({})
  const fmcLights = ref<Record<string, boolean>>({})
  const lines = computed(() => {
    let secondaryLineSubstractor = 0
    return Array.from({ length: 14 }, () => ' '.repeat(24)).map((emptyLine, index) => {
      const isMainLine = index % 2 === 0 || index === 0 || index === 13
      const rawLineNumber = isMainLine ? Math.ceil(index / 2) : index - secondaryLineSubstractor++
      // const rawLineNumber = (index % 2 === 0 ? index : index - 1) / 2
      const filteredRawLineKeys = Object.keys(rawLinesData.value).filter(
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

  function handleBridgeCommand(command: string, value: string | number): void {
    if (command.startsWith('fmc_line-')) {
      const lineKey = command.split('-').pop() as string
      rawLinesData.value[lineKey] = parseFmcString(value.toString())
    }

    if (command === 'fmc_exec_light') {
      fmcLights.value['exec'] = value === 1
    }
  }

  return { lines, debug, handleBridgeCommand, fmcLights: computed(() => fmcLights.value) }
})
