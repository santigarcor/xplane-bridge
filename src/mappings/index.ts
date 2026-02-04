import { initializeMappings as initFF757 } from './ff_757.js'
import type { MappingsInitializer, SupportedAircraft } from './types.js'
import { initializeMappings as initZibo737 } from './zibo_737.js'

const initializer: MappingsInitializer = {
  ff_757: initFF757,
  zibo_737: initZibo737,
}

const supportedAircrafts: { id: SupportedAircraft; label: string }[] = [
  { id: 'ff_757', label: 'FF 757' },
  { id: 'zibo_737', label: 'Zibo 737' },
]

export {
  initializer,
  type MappingsInitializer,
  type SupportedAircraft,
  supportedAircrafts,
}
