import { initializeMappings as initFF757 } from './ff_757.js'
import type { MappingsInitializer, SupportedAircraft } from './types.js'
import { initializeMappings as initZibo737 } from './zibo_737.js'
import { initializeMappings as initC172 } from './c172.js'
export { detectPlane } from './detector.js'

const initializer: MappingsInitializer = {
  zibo_737: initZibo737,
  ff_757: initFF757,
  c172: initC172,
}

const supportedAircrafts: { id: SupportedAircraft; label: string }[] = [
  { id: 'zibo_737', label: 'Zibo 737' },
  { id: 'ff_757', label: 'FF 757' },
  { id: 'c172', label: 'Cessna 172' },
]

export {
  initializer,
  type MappingsInitializer,
  type SupportedAircraft,
  supportedAircrafts,
}
