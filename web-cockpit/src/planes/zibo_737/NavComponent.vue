<script setup lang="ts">
import { isPair } from '@/helpers'
import { NAV_MAP } from './types/737_keys'
import { use737Store } from './stores/737'

defineEmits<{
  (e: 'keyPressed', key: string): void
}>()

const store = use737Store()
</script>

<template>
  <div class="flex gap-4">
    <div class="image-container relative w-full lg:max-w-125 aspect-2995/1351 shadow-2xl bg-black">
      <img src="./assets/737nav.png" class="absolute inset-0 w-full h-full object-fill" />
      <div class="absolute inset-0 z-10">
        <!-- 43 18.9 18.9 17.5 -->
        <div
          class="absolute top-[18.9%] left-[18.9%] w-[43%] h-[17.5%] font-fmc flex flex-col justify-start gap-0 -pt-1 pointer-events-none touch-none"
          :class="{ 'bg-blue-500/10 border border-blue-500': store.debug }"
        >
          <p
            v-for="(lineData, lineIndex) in store.lines"
            :key="lineIndex"
            class="whitespace-pre"
            :class="{
              'leading-[1.2]': lineIndex == 0,
              'leading-[0.15] lg:leading-[0.25]':
                isPair(lineIndex + 1) && ![0, store.lines.length - 1].includes(lineIndex),
              'leading-[1.15]':
                !isPair(lineIndex + 1) && ![0, store.lines.length - 1].includes(lineIndex),
              'leading-[0.8] lg:leading-none': lineIndex == store.lines.length - 1,
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
        <div
          class="absolute top-[41%] left-[18.9%] w-[43%] h-[17.5%] font-fmc flex flex-col justify-start gap-0 -pt-1 pointer-events-none touch-none"
          :class="{ 'bg-blue-500/10 border border-blue-500': store.debug }"
        >
          <p
            v-for="(lineData, lineIndex) in store.lines"
            :key="lineIndex"
            class="whitespace-pre"
            :class="{
              'leading-[1.2]': lineIndex == 0,
              'leading-[0.15] lg:leading-[0.25]':
                isPair(lineIndex + 1) && ![0, store.lines.length - 1].includes(lineIndex),
              'leading-[1.15]':
                !isPair(lineIndex + 1) && ![0, store.lines.length - 1].includes(lineIndex),
              'leading-[0.8] lg:leading-none': lineIndex == store.lines.length - 1,
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
          v-for="key in NAV_MAP"
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
    </div>
  </div>
</template>

<style scoped>
button {
  touch-action: manipulation;
}
</style>
