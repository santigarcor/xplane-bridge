<script setup lang="ts">
import { RouterLink, RouterView } from 'vue-router'
</script>

<template>
  <div class="flex flex-col h-screen">
    <header>
      <div class="wrapper">
        <h1>X-Plane Bridge</h1>

        <nav>
          <RouterLink to="/">Home</RouterLink>
          <RouterLink to="/fmc">FMC</RouterLink>
        </nav>
      </div>
    </header>

    <main class="grow">
      <RouterView />
    </main>
  </div>
</template>

<style scoped></style>
