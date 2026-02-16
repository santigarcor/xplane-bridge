<script setup lang="ts">
import { useWebCockpitStore } from '@/stores/web_cockpit_store'
import { computed, defineAsyncComponent } from 'vue'

const webCockpitStore = useWebCockpitStore()

const activePlane = computed(() => webCockpitStore.activePlane)

const dynamicComponent = computed(() => {
  if (activePlane.value === '') {
    return null
  }
  return defineAsyncComponent(() => import(`@/planes/${activePlane.value}/NavComponent.vue`))
})

function handleKeyPressed(key: string) {
  if (navigator.vibrate) navigator.vibrate(15)
  console.log('Key pressed in NAV View:', key)
  webCockpitStore.sendCommand(key)
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
