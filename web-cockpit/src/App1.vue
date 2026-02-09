<script setup lang="ts">
import { ref } from 'vue'

// --- Estado Simulado (Luego vendrá de Pinia/WebSocket) ---
// El FMC del 737 suele tener 14 líneas.
// Simulamos objetos con texto y color.
const screenLines = ref(
  Array.from({ length: 14 }, (_, i) => {
    if (i === 0) return { text: '      MENU', color: 'white' }
    if (i === 3) return { text: '<FMC', color: 'green' }
    if (i === 5) return { text: '<ACARS', color: 'green' }
    return { text: '', color: 'green' }
  }),
)

// Función para manejar clicks (la conectaremos al socket después)
const handleKeyPress = (key: string) => {
  console.log(`[FMC Key] Presionada: ${key}`)
  // Aquí llamaremos a fmcStore.sendKey(key)
  if (navigator.vibrate) navigator.vibrate(20) // Feedback háptico si el móvil lo soporta
}

// --- Definición de Teclado ---
// Agrupamos las teclas para iterarlas fácilmente
const functionKeys = [
  'INIT REF',
  'RTE',
  'CLB',
  'CRZ',
  'DES',
  'MENU',
  'LEGS',
  'DEP ARR',
  'HOLD',
  'PROG',
  'EXEC',
]
const keypad = [
  '1',
  '2',
  '3',
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  'G',
  '4',
  '5',
  '6',
  'H',
  'I',
  'J',
  'K',
  'L',
  'M',
  'N',
  '7',
  '8',
  '9',
  'O',
  'P',
  'Q',
  'R',
  'S',
  'T',
  'U',
  '.',
  '0',
  '+/-',
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
</script>

<template>
  <main
    class="bg-gray-900 h-screen w-screen flex flex-col p-2 select-none overflow-hidden safe-area-inset"
  >
    <div class="grid grid-cols-[auto_1fr_auto] gap-3 grow max-h-[55vh]">
      <div class="flex flex-col justify-between py-4">
        <button
          v-for="i in 6"
          :key="`L${i}`"
          @touchstart.prevent="handleKeyPress(`LSK_L${i}`)"
          class="fmc-btn lsk-btn"
        ></button>
      </div>

      <div
        class="bg-fmc-dark border-8 border-gray-800 rounded-xl p-3 font-fmc text-2xl leading-none shadow-[inset_0_0_15px_rgba(0,255,0,0.3)] flex flex-col justify-between overflow-hidden relative"
      >
        <div
          class="absolute inset-0 bg-[linear-gradient(transparent_50%,rgba(0,0,0,0.4)_50%)] bg-size[100%_4px] pointer-events-none opacity-30"
        ></div>

        <div
          v-for="(line, index) in screenLines"
          :key="index"
          class="whitespace-pre h-[1.3em] flex items-center"
          :class="{
            'text-fmc-green': line.color === 'green',
            'text-white': line.color === 'white',
            'text-cyan-400': line.color === 'cyan', // Para mensajes de sistema
          }"
        >
          {{ line.text }}
        </div>
      </div>

      <div class="flex flex-col justify-between py-4">
        <button
          v-for="i in 6"
          :key="`R${i}`"
          @touchstart.prevent="handleKeyPress(`LSK_R${i}`)"
          class="fmc-btn lsk-btn"
        ></button>
      </div>
    </div>

    <div
      class="bg-gray-800 rounded-t-2xl mt-4 p-3 grid grid-cols-6 gap-2 shrink-0 shadow-inner border-t border-gray-700"
    >
      <template v-for="key in functionKeys" :key="key">
        <button
          @touchstart.prevent="handleKeyPress(key)"
          class="fmc-btn func-btn text-sm sm:text-base"
          :class="{ 'col-span-2 bg-gray-700': key === 'EXEC' }"
        >
          {{ key }}
          <div
            v-if="key === 'EXEC'"
            class="w-3 h-1 bg-yellow-500 mx-auto mt-1 rounded-full opacity-50"
          ></div>
        </button>
      </template>

      <button
        v-for="key in keypad"
        :key="key"
        @touchstart.prevent="handleKeyPress(key)"
        class="fmc-btn aspect-square text-lg sm:text-xl grow-0"
        :class="{ 'text-red-400': key === 'DEL' || key === 'CLR' }"
      >
        {{ key === 'SP' ? '␣' : key }}
      </button>
    </div>
  </main>
</template>

<style scoped>
@reference "./assets/main.css";
/* Clases de utilidad reutilizables con @apply de Tailwind */

/* Estilo base para cualquier botón físico */
.fmc-btn {
  @apply bg-gray-700 text-gray-200 font-bold rounded-md shadow-md border-b-4 border-gray-950
         active:bg-gray-600 active:border-b-2 active:translate-y-0.5 active:shadow-sm
         transition-all flex items-center justify-center select-none touch-manipulation;
}

/* Estilo específico para los LSK laterales (más rectangulares) */
.lsk-btn {
  @apply w-10 h-14 sm:w-12 sm:h-16;
}

/* Estilo para las teclas de función superiores */
.func-btn {
  @apply h-12 sm:h-14 flex-col leading-tight;
}

/* Utilidad para el notch del iPhone */
.safe-area-inset {
  padding-top: env(safe-area-inset-top);
  padding-bottom: env(safe-area-inset-bottom);
}
</style>
