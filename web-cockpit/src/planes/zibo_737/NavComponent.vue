<script setup lang="ts">
import { NAV_MAP, TRANSPONDER_MAP } from './types/737_keys'
import { use737Store } from './stores/737'
import { NavFrequencyMode } from './stores/737_store_types'
import CockpitImageKey from '@/components/CockpitImageKey.vue'

defineEmits<{
  (e: 'keyPressed', key: string): void
}>()

const store = use737Store()
const addDecimalPoint = (value: string, mode: NavFrequencyMode) => {
  if (mode === NavFrequencyMode.GLS) {
    return value
  }

  const values = value.split('')
  values.splice(3, 0, '.')
  return values.join('')
}

const parseStandbyValue = () =>
  store.navValues.standby.value
    .split('')
    .map((char, index) => (index < store.navValues.standby.cursor ? char : '_'))
    .join('')
</script>

<template>
  <div class="flex flex-wrap gap-2">
    <!-- NAV START -->
    <div class="image-container relative w-full lg:max-w-125 aspect-2995/1351 shadow-2xl bg-white">
      <img src="./assets/737nav.png" class="absolute inset-0 w-full h-full object-fill" />
      <div class="absolute inset-0 z-10">
        <!-- 43 18.9 18.9 17.5 -->
        <div
          class="font-lcd text-[6cqi] text-white/65 absolute top-[18%] left-[20%] w-[43%] h-[17.5%] flex items-center justify-start gap-2 -pt-1 pointer-events-none touch-none"
          :class="{ 'bg-blue-500/10 border border-blue-500': store.debug }"
        >
          <p class="whitespace-pre">
            {{ store.navValues.active.mode }}
          </p>
          <p class="whitespace-pre">
            {{ addDecimalPoint(store.navValues.active.value, store.navValues.active.mode) }}
          </p>
        </div>
        <div
          class="font-lcd text-[6cqi] text-white/65 absolute top-[42%] left-[20%] w-[43%] h-[17.5%] flex items-center justify-start gap-2 -pt-1 pointer-events-none touch-none"
          :class="{ 'bg-blue-500/10 border border-blue-500': store.debug }"
        >
          <p class="whitespace-pre">
            {{ store.navError ? 'ERR' : store.navValues.standby.mode }}
          </p>
          <p class="whitespace-pre">
            {{ addDecimalPoint(parseStandbyValue(), store.navValues.active.mode) }}
          </p>
        </div>

        <CockpitImageKey
          v-for="cockpitImageKey in NAV_MAP"
          :key="cockpitImageKey.id"
          :data="cockpitImageKey"
        />
      </div>
    </div>
    <!-- NAV END -->
    <!-- TRANSPONDER START -->
    <div
      class="image-container relative w-full lg:max-w-125 xl:max-w-148 aspect-2995/1146 shadow-2xl bg-white"
    >
      <img src="./assets/737transponder.png" class="absolute inset-0 w-full h-auto object-fill" />
      <div class="absolute inset-0 z-10">
        <div
          class="text-white/65 absolute top-[9%] left-[40%] w-[20%] h-[33%] flex flex-col items-start justify-start gap-3 -pt-1 pointer-events-none touch-none leading-none"
          :class="{ 'bg-blue-500/10 border border-blue-500': store.debug }"
        >
          <p class="text-[3.5cqi]">ATC 1</p>
          <p class="font-lcd text-[6cqi]">{{ store.transpoderCode }}</p>
        </div>
        <img
          src="./assets/737transponder-arrow.png"
          class="absolute top-[13%] left-[14.2%] w-[7.5%] h-auto"
          :style="`transform: rotate(${store.transpoderMode * 37}deg)`"
        />

        <CockpitImageKey
          v-for="cockpitImageKey in TRANSPONDER_MAP"
          :key="cockpitImageKey.id"
          :data="cockpitImageKey"
        />
      </div>
    </div>
    <!-- TRANSPONDER END -->
  </div>
</template>

<style scoped>
button {
  touch-action: manipulation;
}
</style>
