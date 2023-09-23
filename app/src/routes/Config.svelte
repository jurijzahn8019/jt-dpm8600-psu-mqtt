<script lang="ts">
	import { Link } from 'svelte-routing';
	import { apiEndpoint } from '../lib/env';
	import { onMount } from 'svelte';

	let data: Partial<{
		device_name: string;
		mqtt_server: string;
		mqtt_port: string;
		mqtt_user: string;
		mqtt_pass: string;
		mqtt_prefix: string;
		dpm_bus_id: string;
	}> = {};

	let modal: HTMLInputElement;

	onMount(async () => {
		data = await (await fetch(new URL('config', apiEndpoint))).json();
	});

	async function onSave() {
		data = await (
			await fetch(new URL('config', apiEndpoint), {
				method: 'POST',
				headers: { 'content-type': 'application/json' },
				body: JSON.stringify(data)
			})
		).json();

		modal.checked = true;
	}
</script>

<h3>Configuration</h3>

<fieldset class="flex one">
	<label>
		<input type="text" placeholder="Device Name" bind:value={data.device_name} />
		<small>Used as hostname and also for mqtt and home assistant discovery</small>
	</label>
	<label>
		<input type="text" placeholder="MQTT Server" bind:value={data.mqtt_server} />
		<small>hostname or ip address of mqtt server</small>
	</label>
	<label>
		<input type="number" placeholder="MQTT Server Port" bind:value={data.mqtt_port} />
		<small>port of mqtt server</small>
	</label>
	<label>
		<input type="text" placeholder="MQTT User" bind:value={data.mqtt_user} />
		<small>username used to authorize on mqtt server</small>
	</label>
	<label>
		<input type="text" placeholder="MQTT Password" bind:value={data.mqtt_pass} />
		<small>password used to authorize on mqtt server</small>
	</label>
	<label>
		<input type="text" placeholder="MQTT Prefix" bind:value={data.mqtt_prefix} />
		<small>prefix used to publish messages to (prefix/device/sensor/...)</small>
	</label>
	<label>
		<input type="text" placeholder="DPM Bus ID" bind:value={data.dpm_bus_id} />
		<small>Bus id configured on the DPM8600 Buck Converter</small>
	</label>
</fieldset>

<div class="flex">
	<button class="button" on:click={onSave}>Save and Restart</button>
</div>

<div class="modal">
	<input id="modal_1" type="checkbox" bind:this={modal} />
	<label for="modal_1" class="overlay" />
	<article>
		<header>
			<h3>Configuration Changed</h3>
			<!-- <label for="modal_1" class="close">&times;</label> -->
		</header>
		<section class="content">
			Configuration has beeen successfully changed. Device will restart.
		</section>
		<footer>
			<Link class="button" to="/">Go Back</Link>
		</footer>
	</article>
</div>
