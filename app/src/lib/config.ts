import { writable, type Readable } from 'svelte/store';
import { apiEndpoint } from './env';

export type Config = {
  device_name: string;
  mqtt_server: string;
  mqtt_port: string;
  mqtt_user: string;
  mqtt_pass: string;
  mqtt_prefix: string;
  dpm_bus_id: string;
};

const config = writable<Config>({} as never);

async function refresh(): Promise<void> {
  const c = await (await fetch(new URL('config', apiEndpoint))).json();
  config.set(c)
}

async function update(value: Config): Promise<void> {
  const res: Config = await (
    await fetch(new URL('config', apiEndpoint), {
      method: 'POST',
      headers: { 'content-type': 'application/json' },
      body: JSON.stringify(value)
    })
  ).json();

  config.set(res);
}

function get(): Readable<Config> {
  refresh();
  return config;
}

export function useConfig(): [
  get: () => Readable<Config>,
  update: (value: Config) => Promise<void>,
  refresh: () => Promise<void>,
] {
  return [get, update, refresh];
}
