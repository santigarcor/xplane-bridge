<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'
import { useWebCockpitStore } from '@/stores/web_cockpit_store'
import { onMounted, ref } from 'vue'
import PhoneIcon from './components/icons/PhoneIcon.vue'
import RadioIcon from './components/icons/RadioIcon.vue'
import { WebSocketStatus } from './types'

const routes = [
  {
    route: '/fmc',
    name: 'FMC',
    icon: PhoneIcon,
  },
  {
    route: '/nav',
    name: 'NAV',
    icon: RadioIcon,
  },
]

const mobileSidebarOpen = ref(false)
const desktopSidebarOpen = ref(true)

const webCockpitStore = useWebCockpitStore()

onMounted(() => {
  console.log('App mounted, initializing web cockpit store')
  webCockpitStore.initializeWebSocket()
  // webCockpitStore.setActivePlane('zibo_737')
})
</script>

<template>
  <!-- Page Container -->
  <div
    id="page-container"
    class="mx-auto flex min-h-dvh w-full min-w-80 flex-col bg-gray-100 dark:bg-gray-900 dark:text-gray-100"
    :class="{
      'lg:pl-64': desktopSidebarOpen,
    }"
  >
    <!-- Page Sidebar -->
    <nav
      id="page-sidebar"
      class="fixed top-0 bottom-0 left-0 z-50 flex h-full w-full flex-col border-r border-gray-200 bg-white transition-transform duration-500 ease-out lg:w-64 dark:border-gray-800 dark:bg-gray-800 dark:text-gray-200"
      :class="{
        '-translate-x-full': !mobileSidebarOpen,
        'translate-x-0': mobileSidebarOpen,
        'lg:-translate-x-full': !desktopSidebarOpen,
        'lg:translate-x-0': desktopSidebarOpen,
      }"
      aria-label="Main Sidebar Navigation"
    >
      <!-- Sidebar Header -->
      <div
        class="flex h-12 w-full flex-none items-center justify-between px-4 lg:justify-center dark:bg-gray-600/25"
      >
        <!-- Brand -->
        <a
          href="#"
          class="group inline-flex items-center gap-2 text-lg font-bold tracking-wide text-gray-900 hover:text-gray-600 dark:text-gray-100 dark:hover:text-gray-300"
        >
          <span>WebCockpit</span>
        </a>
        <!-- END Brand -->

        <!-- Close Sidebar on Mobile -->
        <div class="lg:hidden">
          <button
            @click="mobileSidebarOpen = false"
            type="button"
            class="inline-flex items-center justify-center gap-2 rounded-lg border border-gray-200 bg-white px-3 py-2 text-sm leading-5 font-semibold text-gray-800 hover:border-gray-300 hover:text-gray-900 hover:shadow-xs focus:ring-3 focus:ring-gray-300/25 active:border-gray-200 active:shadow-none dark:border-gray-700 dark:bg-gray-800 dark:text-gray-300 dark:hover:border-gray-600 dark:hover:text-gray-200 dark:focus:ring-gray-600/40 dark:active:border-gray-700"
          >
            <svg
              class="hi-mini hi-x-mark -mx-0.5 inline-block size-5"
              xmlns="http://www.w3.org/2000/svg"
              viewBox="0 0 20 20"
              fill="currentColor"
              aria-hidden="true"
            >
              <path
                d="M6.28 5.22a.75.75 0 00-1.06 1.06L8.94 10l-3.72 3.72a.75.75 0 101.06 1.06L10 11.06l3.72 3.72a.75.75 0 101.06-1.06L11.06 10l3.72-3.72a.75.75 0 00-1.06-1.06L10 8.94 6.28 5.22z"
              />
            </svg>
          </button>
        </div>
        <!-- END Close Sidebar on Mobile -->
      </div>
      <!-- END Sidebar Header -->

      <!-- Sidebar Navigation -->
      <div class="overflow-y-auto">
        <div class="w-full p-4">
          <nav class="space-y-1">
            <RouterLink
              v-for="route in routes"
              :key="route.route"
              :to="route.route"
              class="group flex items-center gap-2 rounded-lg border border-transparent px-2.5 text-sm font-medium text-gray-800 hover:bg-blue-50 hover:text-gray-900 active:border-blue-100 dark:text-gray-200 dark:hover:bg-gray-700/75 dark:hover:text-white dark:active:border-gray-600"
              activeClass="group flex items-center gap-2 rounded-lg border border-blue-100 bg-blue-50 px-2.5 text-sm font-medium text-gray-900 dark:border-transparent dark:bg-gray-700/75 dark:text-white"
              v-slot="{ isActive }"
              @click="mobileSidebarOpen = false"
            >
              <span
                :class="{
                  'flex flex-none items-center text-blue-500 dark:text-gray-300': isActive,
                  'flex flex-none items-center text-gray-400 group-hover:text-blue-500 dark:text-gray-500 dark:group-hover:text-gray-300':
                    !isActive,
                }"
              >
                <component :is="route.icon" />
              </span>
              <span class="grow py-2">{{ route.name }}</span>
            </RouterLink>
            <!-- <div
              class="px-3 pt-5 pb-2 text-xs font-semibold tracking-wider text-gray-500 uppercase"
            >
              Projects
            </div> -->
          </nav>
        </div>
      </div>
      <!-- END Sidebar Navigation -->
    </nav>
    <!-- Page Sidebar -->

    <!-- Page Header -->
    <header
      id="page-header"
      class="fixed top-0 right-0 left-0 z-30 flex h-12 flex-none items-center bg-white shadow-xs dark:bg-gray-800"
      :class="{
        'lg:pl-64': desktopSidebarOpen,
      }"
    >
      <div class="mx-auto flex w-full max-w-10xl justify-between px-4 lg:px-8">
        <!-- Left Section -->
        <div class="flex items-center gap-2">
          <!-- Toggle Sidebar on Desktop -->
          <div class="hidden lg:block">
            <button
              @click="desktopSidebarOpen = !desktopSidebarOpen"
              type="button"
              class="inline-flex items-center justify-center gap-2 rounded-lg border border-gray-200 bg-white px-3 py-2 text-sm leading-5 font-semibold text-gray-800 hover:border-gray-300 hover:text-gray-900 hover:shadow-xs focus:ring-3 focus:ring-gray-300/25 active:border-gray-200 active:shadow-none dark:border-gray-700 dark:bg-gray-800 dark:text-gray-300 dark:hover:border-gray-600 dark:hover:text-gray-200 dark:focus:ring-gray-600/40 dark:active:border-gray-700"
            >
              <svg
                class="hi-solid hi-menu-alt-1 inline-block size-5"
                fill="currentColor"
                viewBox="0 0 20 20"
                xmlns="http://www.w3.org/2000/svg"
              >
                <path
                  fill-rule="evenodd"
                  d="M3 5a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1zM3 10a1 1 0 011-1h6a1 1 0 110 2H4a1 1 0 01-1-1zM3 15a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1z"
                  clip-rule="evenodd"
                />
              </svg>
            </button>
          </div>

          <!-- END Toggle Sidebar on Desktop -->

          <!-- Toggle Sidebar on Mobile -->
          <div class="lg:hidden">
            <button
              @click="mobileSidebarOpen = !mobileSidebarOpen"
              type="button"
              class="inline-flex items-center justify-center gap-2 rounded-lg border border-gray-200 bg-white px-3 py-2 text-sm leading-5 font-semibold text-gray-800 hover:border-gray-300 hover:text-gray-900 hover:shadow-xs focus:ring-3 focus:ring-gray-300/25 active:border-gray-200 active:shadow-none dark:border-gray-700 dark:bg-gray-800 dark:text-gray-300 dark:hover:border-gray-600 dark:hover:text-gray-200 dark:focus:ring-gray-600/40 dark:active:border-gray-700"
            >
              <svg
                class="hi-solid hi-menu-alt-1 inline-block size-5"
                fill="currentColor"
                viewBox="0 0 20 20"
                xmlns="http://www.w3.org/2000/svg"
              >
                <path
                  fill-rule="evenodd"
                  d="M3 5a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1zM3 10a1 1 0 011-1h6a1 1 0 110 2H4a1 1 0 01-1-1zM3 15a1 1 0 011-1h12a1 1 0 110 2H4a1 1 0 01-1-1z"
                  clip-rule="evenodd"
                />
              </svg>
            </button>
          </div>
          <!-- END Toggle Sidebar on Mobile -->

          <div class="flex items-center gap-2">
            <div
              class="inline-flex items-center gap-1 rounded-sm bg-blue-200 px-2 py-1 text-xs leading-4 font-semibold text-blue-700"
            >
              <svg
                xmlns="http://www.w3.org/2000/svg"
                viewBox="0 0 24 24"
                fill="none"
                stroke="currentColor"
                stroke-width="2"
                stroke-linecap="round"
                stroke-linejoin="round"
                class="lucide lucide-plane inline-block size-4"
              >
                <path
                  d="M17.8 19.2 16 11l3.5-3.5C21 6 21.5 4 21 3c-1-.5-3 0-4.5 1.5L13 8 4.8 6.2c-.5-.1-.9.1-1.1.5l-.3.5c-.2.5-.1 1 .3 1.3L9 12l-2 3H4l-1 1 3 2 2 3 1-1v-3l3-2 3.5 5.3c.3.4.8.5 1.3.3l.5-.2c.4-.3.6-.7.5-1.2z"
                />
              </svg>
              <span>{{
                webCockpitStore.activePlane !== ''
                  ? webCockpitStore.activePlane
                  : 'No plane selected'
              }}</span>
            </div>
            <div
              class="inline-flex rounded-sm px-2 py-1 text-xs leading-4 font-semibold"
              :class="{
                'bg-emerald-200 text-emerald-700':
                  webCockpitStore.wsStatus === WebSocketStatus.CONNECTED,
                'bg-orange-200 text-orange-700':
                  webCockpitStore.wsStatus === WebSocketStatus.RECONNECTING,
                'bg-red-200 text-red-700': webCockpitStore.wsStatus === WebSocketStatus.ERROR,
                'bg-gray-200 text-gray-700':
                  webCockpitStore.wsStatus === WebSocketStatus.DISCONNECTED,
              }"
            >
              {{ webCockpitStore.wsStatus }}
            </div>
          </div>
          <!-- END Status -->
        </div>
        <!-- END Left Section -->

        <!-- Center Section -->
        <div class="flex items-center lg:hidden">
          <a
            href="#"
            class="group inline-flex items-center gap-2 text-lg font-bold tracking-wide text-gray-900 hover:text-gray-600 dark:text-gray-100 dark:hover:text-gray-300"
          >
            <span class="hidden sm:inline">WebCockpit</span>
          </a>
        </div>
        <!-- END Center Section -->

        <!-- Right Section -->
        <div class="flex items-center gap-2">
          <!-- Debug -->
          <button
            class="inline-flex items-center justify-center gap-2 rounded-lg border border-gray-200 bg-white px-3 py-2 text-sm leading-5 font-semibold text-gray-800 hover:border-gray-300 hover:text-gray-900 hover:shadow-xs focus:ring-3 focus:ring-gray-300/25 active:border-gray-200 active:shadow-none dark:border-gray-700 dark:bg-gray-800 dark:text-gray-300 dark:hover:border-gray-600 dark:hover:text-gray-200 dark:focus:ring-gray-600/40 dark:active:border-gray-700"
            :class="{ 'bg-emerald-100/20!': webCockpitStore.debug }"
            @click="webCockpitStore.toggleDebugMode()"
          >
            <svg
              :class="{ 'text-emerald-500': webCockpitStore.debug }"
              xmlns="http://www.w3.org/2000/svg"
              viewBox="0 0 24 24"
              fill="none"
              stroke="currentColor"
              stroke-width="2"
              stroke-linecap="round"
              stroke-linejoin="round"
              class="lucide lucide-bug inline-block size-5"
            >
              <path d="m8 2 1.88 1.88" />
              <path d="M14.12 3.88 16 2" />
              <path d="M9 7.13v-1a3.003 3.003 0 1 1 6 0v1" />
              <path
                d="M12 20c-3.3 0-6-2.7-6-6v-3a4 4 0 0 1 4-4h4a4 4 0 0 1 4 4v3c0 3.3-2.7 6-6 6"
              />
              <path d="M12 20v-9" />
              <path d="M6.53 9C4.6 8.8 3 7.1 3 5" />
              <path d="M6 13H2" />
              <path d="M3 21c0-2.1 1.7-3.9 3.8-4" />
              <path d="M20.97 5c0 2.1-1.6 3.8-3.5 4" />
              <path d="M22 13h-4" />
              <path d="M17.2 17c2.1.1 3.8 1.9 3.8 4" />
            </svg>
          </button>
          <!-- END Debug -->
          <!-- Refresh -->
          <button
            class="inline-flex items-center justify-center gap-2 rounded-lg border border-gray-200 bg-white px-3 py-2 text-sm leading-5 font-semibold text-gray-800 hover:border-gray-300 hover:text-gray-900 hover:shadow-xs focus:ring-3 focus:ring-gray-300/25 active:border-gray-200 active:shadow-none dark:border-gray-700 dark:bg-gray-800 dark:text-gray-300 dark:hover:border-gray-600 dark:hover:text-gray-200 dark:focus:ring-gray-600/40 dark:active:border-gray-700"
            @click="webCockpitStore.reInitializeWebSocket()"
          >
            <svg
              xmlns="http://www.w3.org/2000/svg"
              viewBox="0 0 16 16"
              fill="currentColor"
              class="hi-micro hi-arrow-path inline-block size-5"
            >
              <path
                fill-rule="evenodd"
                d="M13.836 2.477a.75.75 0 0 1 .75.75v3.182a.75.75 0 0 1-.75.75h-3.182a.75.75 0 0 1 0-1.5h1.37l-.84-.841a4.5 4.5 0 0 0-7.08.932.75.75 0 0 1-1.3-.75 6 6 0 0 1 9.44-1.242l.842.84V3.227a.75.75 0 0 1 .75-.75Zm-.911 7.5A.75.75 0 0 1 13.199 11a6 6 0 0 1-9.44 1.241l-.84-.84v1.371a.75.75 0 0 1-1.5 0V9.591a.75.75 0 0 1 .75-.75H5.35a.75.75 0 0 1 0 1.5H3.98l.841.841a4.5 4.5 0 0 0 7.08-.932.75.75 0 0 1 1.025-.273Z"
                clip-rule="evenodd"
              />
            </svg>
          </button>
          <!-- END Refresh -->
        </div>
        <!-- END Right Section -->
      </div>
    </header>
    <!-- END Page Header -->

    <!-- Page Content -->
    <main id="page-content" class="flex max-w-full flex-auto flex-col pt-12">
      <!-- Page Section -->
      <div class="mx-auto w-full max-w-10xl p-4">
        <RouterView />
      </div>
      <!-- END Page Section -->
    </main>
    <!-- END Page Content -->
  </div>
  <!-- END Page Container -->
</template>
