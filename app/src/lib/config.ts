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

async function load(): Promise<Config> {
  return (await fetch(new URL('config', apiEndpoint))).json();
}

async function save(value: Config): Promise<Config> {
  return (
    await fetch(new URL('config', apiEndpoint), {
      method: 'POST',
      headers: { 'content-type': 'application/json' },
      body: JSON.stringify(value)
    })
  ).json();
}

const config = writable<Config>({} as never);
load().then(d => config.set(d));

setInterval(async () => {
  config.set(await load());
}, 5000);

export function useConfig(): [current: Readable<Config>, update: (value: Config) => void] {
  return [config, save];
}
