<script setup lang="ts">
import { isPair } from '@/helpers'
import { FMC_MAP } from './types/737_keys'
import { use737Store } from './stores/737'
import CockpitImageKey from '@/components/CockpitImageKey.vue'

const store = use737Store()
</script>

<template>
  <img src="./assets/737fmc.png" class="absolute inset-0 w-full h-full object-fill" />
  <div class="absolute inset-0 z-10">
    <div
      class="absolute top-[5.5%] left-[16.9%] w-[66%] h-[36.9%] font-fmc flex flex-col justify-start gap-0 -pt-1 pointer-events-none touch-none"
      :class="{ 'bg-blue-500/10 border border-blue-500': store.debug }"
    >
      <p
        v-for="(lineData, lineIndex) in store.fmcLines"
        :key="lineIndex"
        class="whitespace-pre"
        :class="{
          'leading-[1.2]': lineIndex == 0,
          'leading-[0.24] lg:leading-[0.35]':
            isPair(lineIndex + 1) && ![0, store.fmcLines.length - 1].includes(lineIndex),
          'leading-[1.15]':
            !isPair(lineIndex + 1) && ![0, store.fmcLines.length - 1].includes(lineIndex),
          'leading-[0.7] lg:leading-[0.7]': lineIndex == store.fmcLines.length - 1,
        }"
      >
        <template v-for="(charData, charIndex) in lineData" :key="charIndex">
          <span v-if="!['□'].includes(charData.char)" :class="`${charData.size} ${charData.color}`">
            {{ charData.char }}
          </span>
          <span
            v-else
            class="inline-block size-[2.7cqi] border-2 border-solid border-current m-0"
            :class="`${charData.size} ${charData.color}`"
          ></span>
        </template>
      </p>
    </div>

    <CockpitImageKey
      v-for="cockpitImageKey in FMC_MAP"
      :key="cockpitImageKey.id"
      :data="cockpitImageKey"
    />

    <div
      v-if="store.fmcLights['exec']"
      class="bg-yellow-400/58 rounded-xl absolute top-[54.2%] left-[80.6%] w-[8%] h-[1.5%] touch-none"
      :class="{ 'bg-yellow-500/10 border border-yellow-500': store.debug }"
    ></div>
    <div
      v-if="store.fmcLights['msg']"
      class="flex flex-col text-[3cqi] font-bold leading-none text-yellow-400/58 absolute top-[71.2%] left-[93.3%] touch-none gap-0 items-center justify-center"
      :class="{ 'bg-yellow-500/10 border border-yellow-500': store.debug }"
    >
      <span>M</span>
      <span>S</span>
      <span>G</span>
    </div>
  </div>
</template>

<style scoped>
button {
  touch-action: manipulation;
}
</style>
