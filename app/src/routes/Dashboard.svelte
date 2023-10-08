<script lang="ts">
	import { derived, writable } from 'svelte/store';
	import { apiEndpoint } from '../lib/env';
	import { Gauge, type GaugeOptions } from 'gaugeJS';
	import { onMount } from 'svelte';
	// @ts-ignore
	import bulmaSlider from 'bulma-slider/src/js/index.js';

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
		radiusScale: 0.7
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

		bulmaSlider.attach();
	});
</script>

<div class="columns is-mobile is-gapless">
	<div class="column">
		<canvas style="width:100%;" bind:this={voltageCanvas} />
	</div>
	<div class="column">
		<canvas style="width:100%;" bind:this={currentCanvas} />
	</div>
</div>
<div class="columns is-mobile is-gapless">
	<div class="column has-text-centered">
		{round2($data.voltage)} V
	</div>
	<div class="column has-text-centered" style="text-align: center;">
		{round2($data.current)} A
	</div>
</div>

<div class="box">
	<div class="columns is-mobile">
		<div class="column has-text-centered">Connection</div>
		<div class="column has-text-centered">Temp</div>
		<div class="column has-text-centered">Mode</div>
		<div class="column has-text-centered">Power</div>
	</div>

	<div class="columns is-mobile">
		<div class="column has-text-centered">
			<!-- <div class="tags has-addons"> -->
			<span class="tag {$data.connected ? 'is-success' : 'is-danger'}"
				>{$data.connected ? 'connected' : 'disconnected'}</span
			>
			<!-- </div> -->
		</div>
		<div class="column has-text-centered">
			{round2($data.temperature)} C°
		</div>
		<div class="column">
			<div class="tags has-addons is-centered">
				<span class="tag is-success {$cccvStatus === 'CV' ? '' : 'is-light'}">CV</span>
				<span class="tag is-warning {$cccvStatus === 'CC' ? '' : 'is-light'}">CC</span>
			</div>
		</div>

		<div class="column has-text-centered">
			<!-- <div class="tags has-addons"> -->
			<span
				class="tag {$powerStatus.toLowerCase() === 'on'
					? 'is-success'
					: $powerStatus.toLowerCase() === 'off'
					? 'is-danger'
					: 'is-light'}">{$powerStatus || 'unknown'}</span
			>
			<!-- </div> -->
		</div>
	</div>
</div>

<div class="box">
	<div class="columns is-mobile has-text-centered">
		<div class="column">
			Target Voltage: {round2($data.max_voltage)} V
		</div>
		<div class="column">
			Target Current: {round2($data.max_current)} A
		</div>
	</div>
	<div class="columns is-mobile">
		<div class="column">
			<input
				id="input-voltage"
				class="slider is-fullwidth is-success"
				step="0.1"
				min="0"
				max="60"
				bind:value={$data.max_voltage}
				type="range"
			/>
		</div>
		<div class="column">
			<input
				id="input-current"
				class="slider is-fullwidth is-warning"
				step="0.1"
				min="0"
				max="24"
				bind:value={$data.max_current}
				type="range"
			/>
		</div>
	</div>
</div>

<style lang="scss">
</style>
