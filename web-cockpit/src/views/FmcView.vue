<script setup lang="ts">
import { useWebCockpitStore } from '@/stores/web_cockpit_store'
import { computed, defineAsyncComponent, onMounted, watch } from 'vue'

const webCockpitStore = useWebCockpitStore()

const activePlane = computed(() => webCockpitStore.activePlane)
const planeStore = computed(() => webCockpitStore.activePlaneStore)

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
</script>

<template>
  <div
    class="image-container relative w-full lg:max-w-110 xl:max-w-120 mx-auto aspect-271/415 shadow-2xl bg-black"
  >
    <component v-if="dynamicComponent !== null" :is="dynamicComponent" />
  </div>
</template>

<style scoped></style>
