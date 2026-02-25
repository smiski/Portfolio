import Vue from 'vue'
import VueRouter from 'vue-router'

import ClientDashboard from "../views/ClientDashboard.vue";
import ClientManagement from "../views/ClientManagement.vue";

Vue.use(VueRouter)

/*
 * Application routes
 * - "/"              → dashboard (list/create clients)
 * - "/clients/:id"   → management page for a single client
 * - "/about"         → about page
 */
const routes = [
  {
    path: '/',
    name: 'ClientDashboard',
    component: ClientDashboard,
  },
  {
    path: '/clients/:id',
    name: 'ClientManagement',
    component: ClientManagement,
    props: true,
  },
  {
    path: '/about',
    name: 'About',
    // route level code-splitting
    // this generates a separate chunk (about.[hash].js) for this route
    // which is lazy-loaded when the route is visited.
    component: () => import(/* webpackChunkName: "about" */ '../views/About.vue')
  }
];

const router = new VueRouter({
  mode: 'history',
  base: process.env.BASE_URL,
  routes
})

export default router;