import type { XPlaneBridge } from '../bridge/index.js'

export type MappingsInitializer = {
  zibo_737: (bridge: XPlaneBridge) => void
  ff_757: (bridge: XPlaneBridge) => void
}

export type SupportedAircraft = keyof MappingsInitializer
