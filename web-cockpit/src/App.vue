<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'

import { useBridgeStore } from '@/stores/bridge'
import { onMounted } from 'vue'

const routes = [
  {
    route: '/fmc',
    name: 'FMC',
  },
  {
    route: '/nav',
    name: 'NAV',
  },
]

const bridgeStore = useBridgeStore()
// bridgeStore.initializeWebSocket()

onMounted(() => {
  console.log('App mounted, initializing bridge store')
  // bridgeStore.setActivePlane('zibo_737')
  // bridgeStore.activePlaneStore!.debug = true
})
</script>

<template>
  <div class="flex flex-col h-screen">
    <header
      id="page-header"
      class="z-1 flex flex-none items-center bg-white shadow-xs dark:bg-gray-800"
    >
      <div class="container mx-auto px-2 lg:px-2 xl:max-w-7xl">
        <div class="flex justify-between py-2">
          <!-- Left Section -->
          <div class="flex items-center gap-2 lg:gap-6">
            <!-- Logo -->
            <div
              class="group inline-flex items-center gap-2 text-lg font-bold tracking-wide text-gray-900 hover:text-gray-600 dark:text-gray-100 dark:hover:text-gray-300"
            >
              <span>WebCockpit</span>
            </div>
            <div
              class="group inline-flex items-center gap-2 text-sm font-bold tracking-wide text-gray-900 hover:text-gray-600 dark:text-gray-100 dark:hover:text-gray-300"
            >
              <span>Active Plane:</span>
              <span class="text-xs">{{ bridgeStore.activePlane }}</span>
            </div>
            <!-- END Logo -->

            <!-- Desktop Navigation -->
            <nav class="items-center gap-2 flex">
              <RouterLink
                v-for="route in routes"
                :key="route.route"
                :to="route.route"
                class="group flex items-center gap-2 rounded-lg border border-transparent px-3 py-1 text-sm font-medium text-gray-800 hover:bg-blue-50 hover:text-blue-600 active:border-blue-100 dark:text-gray-200 dark:hover:bg-gray-700 dark:hover:text-white dark:active:border-gray-600"
                activeClass="group flex items-center gap-2 rounded-lg border border-blue-100 bg-blue-50 px-3 py-1 text-sm font-medium text-blue-600 dark:border-transparent dark:bg-gray-700 dark:text-white"
                >{{ route.name }}</RouterLink
              >
            </nav>
            <!-- END Desktop Navigation -->
          </div>
          <!-- END Left Section -->
        </div>
      </div>
    </header>

    <main class="grow p-2">
      <RouterView />
    </main>
  </div>
</template>

<style scoped></style>
