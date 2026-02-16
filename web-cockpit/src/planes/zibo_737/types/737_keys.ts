import type { CockpitImageKey } from '@/types'

const LSK_LEFT: CockpitImageKey[] = Array.from({ length: 6 }, (_, i) => ({
  id: `LSK_L${i + 1}`,
  top: 10.5 + i * 5.05,
  left: 0.1,
  width: 7,
  height: 3.3,
}))

const LSK_RIGHT: CockpitImageKey[] = Array.from({ length: 6 }, (_, i) => ({
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

const ALPHA_KEYS: CockpitImageKey[] = alphaKeysData.map((key, index) => ({
  id: key,
  top: 61.6 + Math.floor(index / 5) * 6.5,
  left: 41.7 + (index % 5) * 10.3,
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
  top: 74.5 + Math.floor(index / 3) * 6.6,
  left: 9.4 + (index % 3) * 10.3,
  width: 8.1,
  height: 5.2,
}))

const FUNC_KEYS1: CockpitImageKey[] = [
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

const FUNC_KEYS2: CockpitImageKey[] = ['MENU', 'N1_LIMIT', 'PREV_PAGE', 'NEXT_PAGE'].map(
  (key, index) => ({
    id: key,
    top: 61.4 + Math.floor(index / 2) * 6.4,
    left: 10.5 + (index % 2) * 12.9,
    width: 11.5,
    height: 5.5,
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
    top: 56.1,
    left: 78.8,
    width: 11.5,
    height: 4.3,
  },
]

const NAV_NUM_KEYS: CockpitImageKey[] = [
  'NAV_1',
  'NAV_2',
  'NAV_3',
  'NAV_4',
  'NAV_5',
  'NAV_6',
  'NAV_7',
  'NAV_8',
  'NAV_9',
  'NAV_EMPTY',
  'NAV_0',
  'NAV_CLR',
].map((key, index) => ({
  id: key,
  top: 3.2 + Math.floor(index / 3) * 25,
  left: 66 + (index % 3) * 9.8,
  width: 8.1,
  height: 17.2,
}))
export const NAV_MAP: CockpitImageKey[] = [
  ...NAV_NUM_KEYS,
  {
    id: 'NAV_DN',
    top: 79.4,
    left: 18.5,
    width: 7,
    height: 16,
  },
  {
    id: 'NAV_UP',
    top: 79.4,
    left: 28.5,
    width: 7,
    height: 16,
  },
  {
    id: 'NAV_ACT_STBY',
    top: 29.4,
    left: 6.7,
    width: 7,
    height: 16,
  },
]

// laminar/B738/push_button/mmr1_1
// laminar/B738/push_button/mmr1_2
// laminar/B738/push_button/mmr1_clr
// laminar/B738/push_button/mmr1_mode_up
// laminar/B738/push_button/mmr1_mode_dn
// laminar/B738/push_button/mmr1_act_stby

// laminar/B738/mmr/cpt/stby_cursor => 0 - 5 show the cursor position on the display if 4 it shows 1__.__
// laminar/B738/mmr/cpt/stby_mode => 0 -> VOR, 1 -> ILS, 2 -> GLS
// laminar/B738/mmr/cpt/stby_value array of values inverse
// laminar/B738/mmr/cpt/err shows ERR on the stdnby

// laminar/B738/mmr/cpt/act_value
// laminar/B738/mmr/cpt/act_mode
