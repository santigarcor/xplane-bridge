<script setup lang="ts">
import { use757Store } from '@/planes/ff_757/stores/757'
import { FMC_MAP } from './keys'
import { isPair } from '@/helpers'

defineEmits<{
  (e: 'keyPressed', key: string): void
}>()

const store = use757Store()
</script>

<template>
  <div
    class="fmc-container relative w-full lg:max-w-125 mx-auto aspect-271/415 shadow-2xl bg-black"
  >
    <img src="./assets/757fmc.png" class="absolute inset-0 w-full h-full object-fill" />
    <div class="absolute inset-0 z-10">
      <div
        class="absolute top-[6%] left-[15%] w-[70%] h-[38.5%] font-fmc flex flex-col justify-start gap-0 -pt-1 pointer-events-none touch-none"
        :class="{ 'bg-blue-500/10 border border-blue-500': store.debug }"
      >
        <p
          v-for="(line, lineIndex) in store.lines"
          :key="lineIndex"
          class="whitespace-pre"
          :class="{
            'leading-[1.2]': lineIndex == 0,
            'leading-[0.2] lg:leading-[0.27]':
              isPair(lineIndex + 1) && ![0, store.lines.length - 1].includes(lineIndex),
            'leading-none':
              !isPair(lineIndex + 1) && ![0, store.lines.length - 1].includes(lineIndex),
            'leading-none lg:leading-none': lineIndex == store.lines.length - 1,
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
