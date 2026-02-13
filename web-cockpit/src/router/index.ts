import { createRouter, createWebHistory } from 'vue-router'

const router = createRouter({
  history: createWebHistory(import.meta.env.BASE_URL),
  routes: [
    {
      path: '/',
      name: 'home',
      redirect: '/fmc',
    },
    {
      path: '/fmc',
      name: 'FMC',
      // route level code-splitting
      // this generates a separate chunk (Fmc.[hash].js) for this route
      // which is lazy-loaded when the route is visited.
      component: () => import('../views/FmcView.vue'),
    },
    {
      path: '/nav',
      name: 'NAV',
      // route level code-splitting
      // this generates a separate chunk (Nav.[hash].js) for this route
      // which is lazy-loaded when the route is visited.
      component: () => import('../views/NavView.vue'),
    },
  ],
})

export default router
