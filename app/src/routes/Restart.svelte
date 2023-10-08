<script lang="ts">
	import { Link } from 'svelte-routing';
	import { apiEndpoint } from '../lib/env';

	async function onClick() {
		await (await fetch(new URL('restart', apiEndpoint), { method: 'POST' })).json();
		showModal = true;
	}

	let showModal = false;
</script>

<div class="box">
	<p>This will restart the device</p>
	<div class="field is-grouped is-grouped-right">
		<p class="control">
			<button class="button is-primary" on:click={onClick}>Restart Device</button>
		</p>
	</div>
</div>

<div class="modal {showModal ? 'is-active' : ''}">
	<div class="modal-background"></div>
	<div class="modal-card">
		<header class="modal-card-head">
			<p class="modal-card-title">Restart inhibited</p>
			<!-- <button class="delete" aria-label="close"></button> -->
		</header>
		<section class="modal-card-body">
			The device will no restart, this can take several seconds
		</section>
		<footer class="modal-card-foot">
			<Link class="button is-success" to="/">Go to Dashboard</Link>
		</footer>
	</div>
</div>
