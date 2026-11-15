/**
 * Prefixes every console.log/warn/error line with the local time (HH:MM:SS.mmm),
 * so the logs show when the Arduino link went silent or reset.
 * Imported first in index.ts so it applies before any other module logs.
 */
const pad = (value: number, length: number = 2) =>
  String(value).padStart(length, '0')

const timestamp = (): string => {
  const now = new Date()
  return `${pad(now.getHours())}:${pad(now.getMinutes())}:${pad(now.getSeconds())}.${pad(now.getMilliseconds(), 3)}`
}

for (const level of ['log', 'warn', 'error'] as const) {
  const original = console[level].bind(console)
  console[level] = (...args: any[]) => original(timestamp(), ...args)
}
