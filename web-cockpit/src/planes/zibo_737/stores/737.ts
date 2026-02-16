import { computed, ref } from 'vue'
import { defineStore } from 'pinia'
import { parseFmcString } from '@/helpers'
import {
  ColorMap,
  FontSizeMap,
  LineSuffix,
  NavFrequencyModes,
  type LineCharData,
  type NavFrequencies,
  type NavFrequency,
} from './737_store_types'

export const use737Store = defineStore('ZIBO_737', () => {
  const debug = ref(false)
  const rawLinesData = ref<Record<string, string>>({})
  const fmcLights = ref<Record<string, boolean>>({})
  const fmcLines = computed(() => {
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
  const navValues = ref<NavFrequencies>({
    active: {
      value: '',
      mode: 'VOR',
      cursor: 0,
    },
    standby: {
      value: '',
      mode: 'VOR',
      cursor: 0,
    },
  })
  const navError = ref(false)

  function setNavValues(
    navType: keyof NavFrequencies,
    field: keyof NavFrequency,
    value: string | number | number[],
  ) {
    switch (field) {
      case 'mode':
        navValues.value[navType][field] = NavFrequencyModes[Number(value)] || 'VOR'
        break
      case 'cursor':
        navValues.value[navType][field] = 5 - Number(value)
        break
      case 'value':
        navValues.value[navType][field] = Array.isArray(value)
          ? value.reverse().join('')
          : value.toString()
        break
    }
  }

  function handleBridgeCommand(command: string, value: string | number | number[]): void {
    if (command.startsWith('fmc_line-')) {
      const lineKey = command.split('-').pop() as string
      rawLinesData.value[lineKey] = parseFmcString(value.toString())
    }

    if (command === 'fmc_exec_light') {
      fmcLights.value['exec'] = value === 1
    }

    if (command === 'fmc_msg_light') {
      fmcLights.value['msg'] = value === 1
    }

    if (command === 'nav_error') {
      navError.value = Boolean(value)
      return
    }

    if (command.startsWith('nav_')) {
      const [_, navType, navField] = command.split('_')
      setNavValues(navType as keyof NavFrequencies, navField as keyof NavFrequency, value)
    }
  }

  return {
    fmcLines,
    navValues,
    debug,
    handleBridgeCommand,
    fmcLights: computed(() => fmcLights.value),
    navError: computed(() => navError.value),
  }
})
