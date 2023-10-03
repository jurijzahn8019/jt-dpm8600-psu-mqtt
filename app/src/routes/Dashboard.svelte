<script lang="ts">
	import { derived, writable } from 'svelte/store';
	import { apiEndpoint } from '../lib/env';
	import { Gauge, type GaugeOptions } from 'gaugeJS';
	import { onMount } from 'svelte';

	type DashData = Partial<{
		voltage: number;
		max_voltage: number;
		current: number;
		max_current: number;
		temperature: number;
		cccv_status: 'CC' | 'CV';
		power: 'ON' | 'OFF';
		connected: boolean;
	}>;

	const gaugeOptions: GaugeOptions = {
		angle: 0, /// The span of the gauge arc
		lineWidth: 0.3, // The line thickness
		pointer: {
			length: 0.6, // Relative to gauge radius
			strokeWidth: 0.035, // The thickness
			color: '#222222'
		},
		colorStart: '#6FADCF', // Colors
		colorStop: '#8FC0DA', // just experiment with them
		strokeColor: '#E0E0E0', // to see which ones work best for you
		staticLabels: {
			font: '0.7rem sans-serif', // Specifies font
			labels: [0, 10, 20, 30, 40, 50, 60], // Print labels at these values
			color: '#000000', // Optional: Label text color
			fractionDigits: 0 // Optional: Numerical precision. 0=round off.
		},
		renderTicks: {
			divisions: 6,
			divLength: 0.3,
			subDivisions: 5,
			subLength: 0.1,
			divWidth: 0.5,
			subWidth: 0.5,
			divColor: '#fff',
			subColor: '#fff'
		},
		radiusScale: 0.8
	};

	const data = writable<DashData>({});
	const cccvStatus = derived(data, (v) => (v.cccv_status ?? '') as string);
	const powerStatus = derived(data, (v) => (v.power ?? '') as string);

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

	let voltageCanvas: HTMLCanvasElement;
	let voltageGauge: Gauge;
	let currentCanvas: HTMLCanvasElement;
	let currentGauge: Gauge;

	$: {
		voltageGauge?.set($data.voltage ?? 0);
		currentGauge?.set($data.current ?? 0);
	}

	onMount(() => {
		voltageGauge = new Gauge(voltageCanvas).setOptions(gaugeOptions);
		voltageGauge.maxValue = 60;
		voltageGauge.setTextField;

		currentGauge = new Gauge(currentCanvas).setOptions({
			...gaugeOptions,
			staticLabels: {
				...gaugeOptions.staticLabels,
				font: '0.7rem sans-serif',
				labels: [0, 4, 8, 12, 16, 20, 24]
			}
		});
		currentGauge.maxValue = 24;
	});
</script>

<div class="flex two">
	<canvas bind:this={voltageCanvas} />
	<canvas bind:this={currentCanvas} />
	<div style="text-align: center;">{round2($data.voltage)} V</div>
	<div style="text-align: center;">{round2($data.current)} A</div>
</div>

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
	<div>Power: <span class="label power--{$powerStatus.toLowerCase()}">{$powerStatus}</span></div>
</div>

<h3>Targets</h3>
<div class="flex two">
	<div>Voltage: {round2($data.max_voltage)} V</div>
	<div>Current: {round2($data.max_current)} A</div>
</div>

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
