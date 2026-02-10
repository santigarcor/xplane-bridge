import { use757Store } from '@/planes/ff_757/store'
import { use737Store } from '@/planes/zibo_737/store'
import type { SupportedAircraft } from '@/types'
import { defineStore, type _StoreWithState } from 'pinia'
import { computed, ref } from 'vue'

export const useActivePlaneStore = defineStore('activePlane', () => {
  const activePlaneMapping: Record<
    SupportedAircraft,
    ReturnType<typeof use757Store> | ReturnType<typeof use737Store>
  > = {
    ff_757: use757Store(),
    zibo_737: use737Store(),
  }

  const activePlane = ref<SupportedAircraft | ''>('')
  const activePlaneComputed = computed(() => activePlane.value)

  function setActivePlane(plane: SupportedAircraft): void {
    activePlane.value = plane
  }

  function getActivePlaneStore(): ReturnType<typeof use757Store> | ReturnType<typeof use737Store> {
    return activePlaneMapping[activePlane.value as SupportedAircraft]
  }

  return { activePlane: activePlaneComputed, setActivePlane, getActivePlaneStore }
})
