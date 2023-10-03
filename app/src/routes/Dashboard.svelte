<script lang="ts">
	import { derived, writable } from 'svelte/store';
	import { apiEndpoint } from '../lib/env';

	let data = writable<Record<string, string | number>>({});
	let table = derived(data, (v) => Object.entries(v));
	const cccvStatus = derived(data, (v) => (v.cccv_status ?? '') as string);

	const events = new EventSource(new URL('events', apiEndpoint));
	events.addEventListener(
		'dashboard',
		function (e) {
			data.set(JSON.parse(e.data));
		},
		false
	);

	function round2(value: string | number = 0) {
		return (Math.round(Number.parseFloat('' + value) * 100) / 100).toFixed(2);
	}
</script>

<h3>State</h3>
<div class="flex two">
	<div>
		Connection:
		<span class="label connection--{$data.connected ? 'connected' : 'disconnected'}"
			>{$data.connected ? 'connected' : 'disconnected'}</span
		>
	</div>
	<div>Temp: {round2($data.temperature)} C°</div>

	<!-- <div>RSSI: {$data.RSSI}</div> -->

	<div>CC/CV: <span class="label max_type--{$cccvStatus.toLowerCase()}">{$cccvStatus}</span></div>
	<div>
		Power: <span class="label power--{$data.power ? 'on' : 'off'}"
			>{$data.power ? 'on' : 'off'}</span
		>
	</div>

	<div>Voltage: {round2($data.voltage)} V</div>
	<div>Current: {round2($data.current)} A</div>
</div>

<h3>Targets</h3>
<div class="flex two">
	<div>Voltage: {round2($data.max_voltage)} V</div>
	<div>Current: {round2($data.max_current)} A</div>
</div>

<!-- <div class="flex">
	<table class="">
		<tbody>
			{#each $table as prop}
				<tr> <td>{prop[0]}</td> <td>{prop[1]}</td> </tr>
			{/each}
		</tbody>
	</table>
</div> -->

<style lang="scss">
	.indicator {
		width: 1.5rem;
		height: 1.5rem;
		display: inline-block;
		border-radius: 50%;
		background-color: grey;
		margin-bottom: -0.35rem;
	}

	.power {
		&--on {
			background-color: green;
		}

		&--off {
			background-color: red;
		}
	}

	.max_type {
		&--cc {
			background-color: orange;
		}
		&--cv {
			background-color: green;
		}
	}

	.connection {
		&--disconnected {
			background-color: red;
		}

		&--connected {
			background-color: green;
		}
	}
</style>
