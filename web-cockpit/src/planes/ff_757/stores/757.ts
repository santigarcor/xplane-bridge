import { ref } from 'vue'
import { defineStore } from 'pinia'
import { parseFmcString } from '@/helpers'
import { ColorMap, RawDisplayColor, RawDisplayFontSize, SizeMap } from './757_store_types'

export const use757Store = defineStore('FF_757', () => {
  const debug = ref(false)
  const fmcLines = ref<string[]>([])
  const lineColors = ref<string[]>([])
  const charSizes = ref<string[]>([])

  function setFmcText(data: string) {
    fmcLines.value = data.match(/.{1,24}/g) || []
  }
  function setFmcColors(colors: RawDisplayColor[]) {
    lineColors.value = colors.join('').match(/.{1,24}/g) || []
  }
  function setFmcSizes(sizes: RawDisplayFontSize[]) {
    charSizes.value = sizes.join('').match(/.{1,24}/g) || []
  }

  function getColor(line: number, char: number): string {
    const colorCode = lineColors.value[line]?.[char] || RawDisplayColor.GREEN
    const colorNum = parseInt(`${colorCode}`, 10) as RawDisplayColor
    return ColorMap[colorNum]
  }

  function getSize(line: number, char: number): string {
    const sizeCode = charSizes.value[line]?.[char] || RawDisplayFontSize.LARGE
    const sizeNum = parseInt(`${sizeCode}`, 10) as RawDisplayFontSize
    return SizeMap[sizeNum]
  }

  function handleBridgeCommand(command: string, value: number | string | Array<unknown>): void {
    if (command === 'set_fmc_text') {
      setFmcText(parseFmcString(value as string))
    }
    if (command === 'set_fmc_colors') {
      setFmcColors(value as RawDisplayColor[])
    }
    if (command === 'set_fmc_sizes') {
      setFmcSizes(value as RawDisplayFontSize[])
    }
  }

  return { fmcLines, lineColors, charSizes, getSize, getColor, debug, handleBridgeCommand }
})
