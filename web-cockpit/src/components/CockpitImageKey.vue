<script setup lang="ts">
import { useWebCockpitStore } from '@/stores/web_cockpit_store'
import type { CockpitImageKey } from '@/types'

const props = defineProps<{
  data: CockpitImageKey
}>()
const webCockpitStore = useWebCockpitStore()
</script>
<template>
  <button
    @touchstart.prevent="webCockpitStore.pressKey(props.data.id)"
    class="absolute transition-all active:bg-white/20 active:brightness-50 cursor-pointer"
    :class="webCockpitStore.debug ? 'border border-red-500 bg-red-500/10' : 'bg-transparent'"
    :style="{
      top: props.data.top + '%',
      left: props.data.left + '%',
      width: props.data.width + '%',
      height: props.data.height + '%',
    }"
  >
    <span v-if="webCockpitStore.debug" class="text-[2cqi] text-red-500 absolute -top-4 left-0">
      {{ props.data.id }}
    </span>
  </button>
</template>
