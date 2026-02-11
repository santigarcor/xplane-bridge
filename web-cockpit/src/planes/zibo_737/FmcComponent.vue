<script setup lang="ts">
import { isPair } from '@/helpers'
import { FMC_MAP } from './types/737_keys'
import { use737Store } from './stores/737'

defineEmits<{
  (e: 'keyPressed', key: string): void
}>()

const store = use737Store()
</script>

<template>
  <div
    class="fmc-container relative w-full lg:max-w-125 mx-auto aspect-271/415 shadow-2xl bg-black"
  >
    <img src="./assets/737fmc.png" class="absolute inset-0 w-full h-full object-fill" />
    <div class="absolute inset-0 z-10">
      <div
        class="absolute top-[5.5%] left-[16.9%] w-[66%] h-[36.9%] font-fmc flex flex-col justify-start gap-0 -pt-1 pointer-events-none touch-none"
        :class="{ 'bg-blue-500/10 border border-blue-500': store.debug }"
      >
        <p
          v-for="(lineData, lineIndex) in store.lines"
          :key="lineIndex"
          class="whitespace-pre"
          :class="{
            'leading-[1.2]': lineIndex == 0,
            'leading-[0.2] lg:leading-[0.25]':
              isPair(lineIndex + 1) && ![0, store.lines.length - 1].includes(lineIndex),
            'leading-[1.15] a':
              !isPair(lineIndex + 1) && ![0, store.lines.length - 1].includes(lineIndex),
            'leading-none lg:leading-none': lineIndex == store.lines.length - 1,
          }"
        >
          <template v-for="(charData, charIndex) in lineData" :key="charIndex">
            <span
              v-if="!['□'].includes(charData.char)"
              :class="`${charData.size} ${charData.color}`"
            >
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

      <button
        v-for="key in FMC_MAP"
        :key="key.id"
        @touchstart.prevent="$emit('keyPressed', key.id)"
        class="absolute transition-all active:bg-white/20 active:brightness-50 cursor-pointer"
        :class="store.debug ? 'border border-red-500 bg-red-500/10' : 'bg-transparent'"
        :style="{
          top: key.top + '%',
          left: key.left + '%',
          width: key.width + '%',
          height: key.height + '%',
        }"
      >
        <span v-if="store.debug" class="text-[2cqi] text-red-500 absolute -top-4 left-0">
          {{ key.id }}
        </span>
      </button>
      <div
        v-if="store.fmcLights['exec']"
        class="bg-yellow-400/58 rounded-xl absolute top-[54.2%] left-[80.6%] w-[8%] h-[1.5%] touch-none"
        :class="{ 'bg-yellow-500/10 border border-yellow-500': store.debug }"
      ></div>
    </div>
  </div>
</template>

<style scoped>
/* Establecemos el contenedor como referencia para las unidades cqi
*/
.fmc-container {
  container-type: inline-size;
}

button {
  touch-action: manipulation;
}
</style>
