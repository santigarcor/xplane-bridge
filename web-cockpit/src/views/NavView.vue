<script setup lang="ts">
import { useBridgeStore } from '@/stores/bridge'
import { computed, defineAsyncComponent } from 'vue'

const bridgeStore = useBridgeStore()

const activePlane = computed(() => bridgeStore.activePlane)

const dynamicComponent = computed(() => {
  if (activePlane.value === '') {
    return null
  }
  return defineAsyncComponent(() => import(`@/planes/${activePlane.value}/NavComponent.vue`))
})

function handleKeyPressed(key: string) {
  if (navigator.vibrate) navigator.vibrate(15)
  console.log('Key pressed in NAV View:', key)
  bridgeStore.sendCommand(key)
}
</script>

<template>
  <component
    v-if="dynamicComponent !== null"
    :is="dynamicComponent"
    @key-pressed="handleKeyPressed"
  />
</template>

<style scoped></style>
