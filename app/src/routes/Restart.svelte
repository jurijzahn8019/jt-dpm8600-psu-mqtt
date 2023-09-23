<script lang="ts">
	import { Link } from 'svelte-routing';
	import { apiEndpoint } from '../lib/env';

	async function onClick() {
		await (await fetch(new URL('restart', apiEndpoint), { method: 'POST' })).json();
		modal.checked = true;
	}

	let modal: HTMLInputElement;
</script>

This will restart the device
<button on:click={onClick}>Restart Device</button>

<div class="modal">
	<input id="modal_1" type="checkbox" bind:this={modal} />
	<label for="modal_1" class="overlay" />
	<article>
		<header>
			<h3>Restart inhibited</h3>
			<!-- <label for="modal_1" class="close">&times;</label> -->
		</header>
		<section class="content">The device will no restart, this can take several seconds</section>
		<footer>
			<Link class="button" to="/">Go to Dashboard</Link>
		</footer>
	</article>
</div>
