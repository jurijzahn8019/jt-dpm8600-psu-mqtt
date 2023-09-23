<script lang="ts">
	import { Router, Link, Route, useRouter } from 'svelte-routing';
	import Config from './routes/Config.svelte';
	import Dashboard from './routes/Dashboard.svelte';
	import Restart from './routes/Restart.svelte';
	import { globalHistory } from 'svelte-routing/src/history';
	import { writable } from 'svelte/store';

	export let url = '';
	let menu: HTMLInputElement;

	const route = writable(globalHistory.location);
	globalHistory.listen((history) => {
		route.set(history.location);
	});

	history.pushState = new Proxy(history.pushState, {
		apply(target, thisArg, argumentsList) {
			Reflect.apply(target, thisArg, argumentsList);
			scrollTo(0, 0);
			menu.checked = false;
		}
	});

	const routes = [
		{ title: 'Dashboard', href: '/' },
		{ title: 'Config', href: '/config' },
		{ title: 'Restart', href: '/restart' }
	];

	let activeRoute = routes[0];
	$: activeRoute =
		[...routes].reverse().find((r) => $route.pathname.startsWith(r.href)) ?? routes[0];
</script>

<Router {url}>
	<nav>
		<Link class="brand" to="/">DPM Controller</Link>
		<span class="brand" style="font-weight: 100;">{activeRoute.title}</span>

		<input id="bmenub" type="checkbox" class="show" bind:this={menu} />
		<label for="bmenub" class="burger pseudo button">&#9776;</label>

		<div class="menu">
			{#each routes.slice(1) as route}
				<Link class="button" to={route.href}>{route.title}</Link>
			{/each}
		</div>
	</nav>
	<div class="main">
		<Route path="/config" component={Config} />
		<Route path="/restart" component={Restart} />
		<Route path="/"><Dashboard /></Route>
	</div>
</Router>
