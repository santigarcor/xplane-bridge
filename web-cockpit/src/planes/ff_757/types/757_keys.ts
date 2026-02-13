import type { CockpitImageKey } from '@/types'

const LSK_LEFT: CockpitImageKey[] = Array.from({ length: 6 }, (_, i) => ({
  id: `LSK_L${i + 1}`,
  top: 11.7 + i * 4.77,
  left: 1.5,
  width: 7,
  height: 3.3,
}))

const LSK_RIGHT: CockpitImageKey[] = Array.from({ length: 6 }, (_, i) => ({
  id: `LSK_R${i + 1}`,
  top: 11.7 + i * 4.77,
  left: 91.5,
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

const ALPHA_KEYS: CockpitImageKey[] = alphaKeysData.map((key, index) => ({
  id: key,
  top: 61.4 + Math.floor(index / 5) * 6.3,
  left: 40 + (index % 5) * 9.7,
  width: 8.1,
  height: 5.2,
}))

const NUM_KEYS: CockpitImageKey[] = [
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8',
  '9',
  '.',
  '0',
  '+/-',
].map((key, index) => ({
  id: key,
  top: 74 + Math.floor(index / 3) * 6.3,
  left: 10.5 + (index % 3) * 9.2,
  width: 8.1,
  height: 5.2,
}))

const FUNC_KEYS1: CockpitImageKey[] = [
  'INIT_REF',
  'RTE',
  'CLB',
  'CRZ',
  'DES',
  'DIR_INTC',
  'LEGS',
  'DEP_ARR',
  'HOLD',
  'PROG',
].map((key, index) => ({
  id: key,
  top: 48.4 + Math.floor(index / 5) * 6,
  left: 10.7 + (index % 5) * 13.5,
  width: 11.5,
  height: 5.3,
}))

const FUNC_KEYS2: CockpitImageKey[] = ['MENU', 'FIX', 'PREV_PAGE', 'NEXT_PAGE'].map(
  (key, index) => ({
    id: key,
    top: 60.4 + Math.floor(index / 2) * 6,
    left: 10.7 + (index % 2) * 13.5,
    width: 11.5,
    height: 5.3,
  }),
)

export const FMC_MAP: CockpitImageKey[] = [
  ...LSK_LEFT,
  ...LSK_RIGHT,
  ...ALPHA_KEYS,
  ...NUM_KEYS,
  ...FUNC_KEYS1,
  ...FUNC_KEYS2,
  {
    id: 'EXEC',
    top: 54.4,
    left: 77.7,
    width: 11.5,
    height: 5.3,
  },
]
