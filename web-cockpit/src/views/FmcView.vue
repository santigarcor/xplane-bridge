<script setup lang="ts">
import { useBridgeStore } from '@/stores/bridge'
import { computed, defineAsyncComponent, onMounted, watch } from 'vue'

const bridgeStore = useBridgeStore()

const activePlane = computed(() => bridgeStore.activePlane)
const planeStore = computed(() => bridgeStore.activePlaneStore)

onMounted(() => init())
watch(activePlane, () => init())

function init() {
  if (activePlane.value === '' || planeStore.value === null) {
    console.log('No active plane selected')
    return
  }
}

const dynamicComponent = computed(() => {
  if (activePlane.value === '') {
    return null
  }
  return defineAsyncComponent(() => import(`@/planes/${activePlane.value}/FmcComponent.vue`))
})

function handleKeyPressed(key: string) {
  if (navigator.vibrate) navigator.vibrate(15)
  console.log('Key pressed in FMC View:', key)
  bridgeStore.sendCommand(key)
}
</script>

<template>
  <div
    class="image-container relative w-full lg:max-w-100 xl:max-w-120 mx-auto aspect-271/415 shadow-2xl bg-black"
  >
    <component
      v-if="dynamicComponent !== null"
      :is="dynamicComponent"
      @key-pressed="handleKeyPressed"
    />
  </div>
</template>

<style scoped></style>
