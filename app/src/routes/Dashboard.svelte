<script lang="ts">
	import { derived, writable } from 'svelte/store';
	import { apiEndpoint } from '../lib/env';

	let data = writable<Record<string, string | number>>({});
	let table = derived(data, (v) => Object.entries(v));

	const events = new EventSource(new URL('events', apiEndpoint));
	events.addEventListener(
		'dashboard',
		function (e) {
			data.set(JSON.parse(e.data));
		},
		false
	);
</script>

<div class="flex">
	<table class="">
		<tbody>
			{#each $table as prop}
				<tr> <td>{prop[0]}</td> <td>{prop[1]}</td> </tr>
			{/each}
		</tbody>
	</table>
</div>
