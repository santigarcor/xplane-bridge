<script setup lang="ts">
import { useBridgeStore } from '@/stores/bridge'
import { computed, defineAsyncComponent, onMounted, watch } from 'vue'

const bridgeStore = useBridgeStore()

const activePlane = computed(() => bridgeStore.activePlane)
const planeStore = computed(() => bridgeStore.activePlaneStore)

onMounted(() => {
  init()
  bridgeStore.setActivePlane('ff_757')
})
watch(activePlane, init)

function init() {
  if (activePlane.value === '' || planeStore.value === null) {
    console.log('No active plane selected')
    return
  }

  console.log('FMC View mounted with active plane:', activePlane.value)
  planeStore.value.debug = false
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
  <component
    v-if="dynamicComponent !== null"
    :is="dynamicComponent"
    @key-pressed="handleKeyPressed"
  />
</template>

<style scoped></style>
