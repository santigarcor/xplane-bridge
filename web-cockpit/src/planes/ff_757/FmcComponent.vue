<script setup lang="ts">
import { use757Store } from '@/planes/ff_757/stores/757'
import { FMC_MAP } from './types/757_keys'
import { isPair } from '@/helpers'
import CockpitImageKey from '@/components/CockpitImageKey.vue'

const store = use757Store()
</script>

<template>
  <img src="./assets/757fmc.png" class="absolute inset-0 w-full h-full object-fill" />
  <div class="absolute inset-0 z-10">
    <div
      class="absolute top-[6%] left-[15%] w-[70%] h-[38.5%] font-fmc flex flex-col justify-start gap-0 -pt-1 pointer-events-none touch-none"
      :class="{ 'bg-blue-500/10 border border-blue-500': store.debug }"
    >
      <p
        v-for="(line, lineIndex) in store.fmcLines"
        :key="lineIndex"
        class="whitespace-pre"
        :class="{
          'leading-[1.2]': lineIndex == 0,
          'leading-[0.27] lg:leading-[0.27]':
            isPair(lineIndex + 1) && ![0, store.fmcLines.length - 1].includes(lineIndex),
          'leading-none':
            !isPair(lineIndex + 1) && ![0, store.fmcLines.length - 1].includes(lineIndex),
          'leading-none lg:leading-none': lineIndex == store.fmcLines.length - 1,
        }"
      >
        <template v-for="(char, charIndex) in line" :key="charIndex">
          <span
            v-if="!['□'].includes(char)"
            :class="`${store.getSize(lineIndex, charIndex)} ${store.getColor(lineIndex, charIndex)}`"
          >
            {{ char }}
          </span>
          <span
            v-else
            class="inline-block size-[2.9cqi] border-2 border-solid border-current m-0"
            :class="`${store.getSize(lineIndex, charIndex)} ${store.getColor(lineIndex, charIndex)}`"
          ></span>
        </template>
      </p>
    </div>

    <CockpitImageKey v-for="key in FMC_MAP" :key="key.id" :data="key" />
  </div>
</template>

<style scoped>
button {
  touch-action: manipulation;
}
</style>
