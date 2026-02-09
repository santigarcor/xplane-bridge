<script setup lang="ts">
import { ref } from 'vue'

const debug = ref(true)

// Simulación de datos
const lines = ref([
  { text: '          IDENT      1/1', color: 'white' },
  { text: ' MODEL           ENGINES', color: 'green' },
  { text: '757-200      RB211-535E4', color: 'green' },
  { text: ' NAV DATA         ACTIVE', color: 'green' },
  { text: 'AIRAC2601 JAN22/FEB19/26', color: 'green' },
  { text: ' MODEL VER              ', color: 'green' },
  { text: '2.6.23                  ', color: 'green' },
  { text: ' OP PROGRAM             ', color: 'green' },
  { text: '1.10.11+B3265D2         ', color: 'green' },
  { text: ' DRAG/FF                ', color: 'green' },
  { text: '+0.0/+0.0               ', color: 'green' },
  { text: '------------------------', color: 'green' },
  { text: '<INDEX         POS INIT>', color: 'green' },
  { text: '                        ', color: 'green' },
])

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
          v-for="(line, i) in lines"
          :key="i"
          :class="{
            'text-[#00FF00]': line.color === 'green',
            'text-white': line.color !== 'green',
            'text-[5.5cqi] leading-[0.5] tracking-[0.7cqi]': i == lines.length - 1,
            'text-[5.5cqi] leading-[1.1] tracking-[0.7cqi]': i == 0,
            'text-[4cqi] leading-[0.47]': isPairLine(i + 1) && ![0, lines.length - 1].includes(i),
            'text-[5.5cqi] leading-none': !isPairLine(i + 1) && i !== 0,
          }"
          class="whitespace-pre"
        >
          {{ line.text }}
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
