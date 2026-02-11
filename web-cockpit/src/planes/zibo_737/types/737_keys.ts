import type { FmcKey } from '@/types'

const LSK_LEFT: FmcKey[] = Array.from({ length: 6 }, (_, i) => ({
  id: `LSK_L${i + 1}`,
  top: 10.5 + i * 5.05,
  left: 0.1,
  width: 7,
  height: 3.3,
}))

const LSK_RIGHT: FmcKey[] = Array.from({ length: 6 }, (_, i) => ({
  id: `LSK_R${i + 1}`,
  top: 10.5 + i * 5.1,
  left: 92.7,
  width: 7,
  height: 3.3,
}))

const alphaKeysData = [
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  'G',
  'H',
  'I',
  'J',
  'K',
  'L',
  'M',
  'N',
  'O',
  'P',
  'Q',
  'R',
  'S',
  'T',
  'U',
  'V',
  'W',
  'X',
  'Y',
  'Z',
  'SP',
  'DEL',
  '/',
  'CLR',
]

const ALPHA_KEYS: FmcKey[] = alphaKeysData.map((key, index) => ({
  id: key,
  top: 61.6 + Math.floor(index / 5) * 6.5,
  left: 41.7 + (index % 5) * 10.3,
  width: 8.1,
  height: 5.2,
}))

const NUM_KEYS: FmcKey[] = ['1', '2', '3', '4', '5', '6', '7', '8', '9', '.', '0', '+/-'].map(
  (key, index) => ({
    id: key,
    top: 74.5 + Math.floor(index / 3) * 6.6,
    left: 9.4 + (index % 3) * 10.3,
    width: 8.1,
    height: 5.2,
  }),
)

const FUNC_KEYS1: FmcKey[] = [
  'INIT_REF',
  'RTE',
  'DEP_ARR',
  'ATC',
  'VNAV',
  'FIX',
  'LEGS',
  'HOLD',
  'FMC_COMM',
  'PROG',
].map((key, index) => ({
  id: key,
  top: 48.4 + Math.floor(index / 5) * 6.4,
  left: 10.5 + (index % 5) * 12.9,
  width: 11.5,
  height: 5.5,
}))

const FUNC_KEYS2: FmcKey[] = ['MENU', 'N1_LIMIT', 'PREV_PAGE', 'NEXT_PAGE'].map((key, index) => ({
  id: key,
  top: 61.4 + Math.floor(index / 2) * 6.4,
  left: 10.5 + (index % 2) * 12.9,
  width: 11.5,
  height: 5.5,
}))

export const FMC_MAP: FmcKey[] = [
  ...LSK_LEFT,
  ...LSK_RIGHT,
  ...ALPHA_KEYS,
  ...NUM_KEYS,
  ...FUNC_KEYS1,
  ...FUNC_KEYS2,
  {
    id: 'EXEC',
    top: 56.1,
    left: 78.8,
    width: 11.5,
    height: 4.3,
  },
]
