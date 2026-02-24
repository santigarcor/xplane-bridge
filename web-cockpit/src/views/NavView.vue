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
</script>

<template>
  <component v-if="dynamicComponent !== null" :is="dynamicComponent" />
</template>

<style scoped></style>
