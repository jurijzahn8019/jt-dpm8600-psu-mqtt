<script lang="ts">
	import { Link } from 'svelte-routing';
	import { onMount } from 'svelte';
	import { useConfig, type Config } from '../lib/config';
	import { derived } from 'svelte/store';

	const [get, update, refresh] = useConfig();
	let data = get();
	let showModal = false;

	const modelConfig: Record<keyof Config, { help: string; type?: string }> = {
		device_name: { help: 'Used as hostname and also for mqtt and home assistant discovery' },
		mqtt_server: { help: 'hostname or ip address of mqtt server' },
		mqtt_port: { help: 'port of mqtt server' },
		mqtt_user: { help: 'username used to authorize on mqtt server' },
		mqtt_pass: { help: 'password used to authorize on mqtt server', type: 'password' },
		mqtt_prefix: { help: 'prefix used to publish messages to (prefix/device/sensor/...)' },
		dpm_bus_id: { help: 'Bus id configured on the DPM8600 Buck Converter' }
	};

	let model = Object.entries(modelConfig) as {
		[P in keyof typeof modelConfig]: [P, (typeof modelConfig)[P]];
	}[keyof typeof modelConfig][];

	onMount(async () => {
		await refresh();
	});

	async function saveAndRestart() {
		await update($data);
		showModal = true;
	}

	async function cancelAndReload() {
		console.log($data);
		await refresh();
	}

	function humanize(v: string) {
		const res = v
			.split('_')
			.map((p) => p[0].toUpperCase() + p.slice(1))
			.join(' ');
		return res;
	}
</script>

{#each model as [key, conf]}
	<div class="field is-horizontal">
		<div class="field-label is-normal is-hidden-mobile has-text-left">
			<label for={key} class="label">{humanize(key)}</label>
		</div>
		<div class="field-body">
			<div class="field">
				<div class="control">
					{#if (conf.type ?? typeof $data[key]) === 'number'}
						<input
							id={key}
							class="input is-primary"
							type="number"
							placeholder={humanize(key)}
							bind:value={$data[key]}
						/>
					{:else if (conf.type ?? typeof $data[key]) === 'password'}
						<input
							id={key}
							class="input is-primary"
							type="password"
							placeholder={humanize(key)}
							bind:value={$data[key]}
						/>
					{:else}
						<input
							id={key}
							class="input is-primary"
							type="text"
							placeholder={humanize(key)}
							bind:value={$data[key]}
						/>
					{/if}
				</div>
				<p class="help">{conf.help}</p>
			</div>
		</div>
	</div>
{/each}

<div class="field is-grouped is-grouped-right">
	<p class="control">
		<button class="button is-primary" on:click={saveAndRestart}>Save and Restart</button>
	</p>
	<p class="control">
		<button class="button is-light" on:click={cancelAndReload}>Cancel and Reload</button>
	</p>
</div>

<div class="modal {showModal ? 'is-active' : ''}">
	<div class="modal-background"></div>
	<div class="modal-card">
		<header class="modal-card-head">
			<p class="modal-card-title">Configuration Changed</p>
			<!-- <button class="delete" aria-label="close"></button> -->
		</header>
		<section class="modal-card-body">
			Configuration has beeen successfully changed. Device will restart.
		</section>
		<footer class="modal-card-foot">
			<Link class="button is-success" to="/">Go Back</Link>
		</footer>
	</div>
</div>
