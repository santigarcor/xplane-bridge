<script setup lang="ts">
import { use757Store } from '@/stores/757'
import { ref } from 'vue'

const debug = ref(true)
const store = use757Store()

store.setDisplay(
  '          IDENT      1/1 MODEL           ENGINES757-200      RB211-535E4 NAV DATA         ACTIVEAIRAC2601 JAN22/FEB19/26 MODEL VER              2.6.23                   OP PROGRAM             1.10.11+B3265D2          DRAG/FF                +0.0/+0.0               ------------------------<INDEX         POS INIT>                        ',
  [
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
    4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 4, 4, 4, 4, 4, 4, 4, 4, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
  ],
  [
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  ],
)

const handleKey = (key: string) => {
  if (navigator.vibrate) navigator.vibrate(15)
  console.log('Acción:', key)
}

const isPairLine = (line: number) => {
  return line % 2 === 0
}
</script>

<template>
  <div class="fmc-container relative w-full max-w-125 mx-auto aspect-271/415 shadow-2xl bg-black">
    <img src="../assets/757fmc.png" class="absolute inset-0 w-full h-full object-fill" />

    <div class="absolute inset-0 z-10">
      <div
        class="absolute top-[6%] left-[15%] w-[70%] h-[38.5%] font-fmc flex flex-col justify-start gap-0 -pt-1"
        :class="{ 'bg-blue-500/10 border border-blue-500': debug }"
      >
        <p
          v-for="(line, lineIndex) in store.lines"
          :key="lineIndex"
          class="whitespace-pre"
          :class="{
            'leading-[0.5]': lineIndex == store.lines.length - 1,
            'leading-[0.308]':
              isPairLine(lineIndex + 1) && ![0, store.lines.length - 1].includes(lineIndex),
            'leading-none': !isPairLine(lineIndex + 1),
          }"
        >
          <span
            v-for="(char, charIndex) in line"
            :key="charIndex"
            :class="`${store.getSize(lineIndex, charIndex)} ${store.getColor(lineIndex, charIndex)}`"
          >
            {{ char }}
          </span>
        </p>
      </div>

      <button
        v-for="i in 6"
        :key="`L${i}`"
        @click.prevent="handleKey(`LSK_L${i}`)"
        class="absolute left-[1.5%] w-[7%] h-[3.3%] rounded-sm transition-all active:bg-black active:opacity-15"
        :style="{ top: 11.7 + (i - 1) * 4.77 + '%' }"
        :class="debug ? 'bg-red-500/30 border border-red-500' : 'bg-transparent'"
      ></button>

      <button
        v-for="i in 6"
        :key="`R${i}`"
        @click.prevent="handleKey(`LSK_R${i}`)"
        class="absolute right-[1.2%] w-[7%] h-[3.3%] rounded-sm transition-all active:bg-black active:opacity-15"
        :style="{ top: 11.7 + (i - 1) * 4.77 + '%' }"
        :class="debug ? 'bg-red-500/30 border border-red-500' : 'bg-transparent'"
      ></button>

      <button
        @click.prevent="handleKey('A')"
        class="absolute top-[65.4%] left-[34.1%] w-[8.2%] h-[5.2%] rounded-full active:bg-white/20"
        :class="debug ? 'bg-yellow-500/30 border border-yellow-500' : 'bg-transparent'"
      ></button>
    </div>
  </div>
</template>

<style scoped>
/* Establecemos el contenedor como referencia para las unidades cqi
*/
.fmc-container {
  container-type: inline-size;
}
</style>
