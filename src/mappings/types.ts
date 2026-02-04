import type { XPlaneBridge } from '../bridge/index.js'

export type MappingsInitializer = {
  ff_757: (bridge: XPlaneBridge) => void
  zibo_737: (bridge: XPlaneBridge) => void
}

export type SupportedAircraft = keyof MappingsInitializer
