<script lang="ts">
	import { Router, Link, Route, useRouter } from 'svelte-routing';
	import Config from './routes/Config.svelte';
	import Dashboard from './routes/Dashboard.svelte';
	import Restart from './routes/Restart.svelte';
	import { globalHistory } from 'svelte-routing/src/history';
	import { writable } from 'svelte/store';
	import { useConfig } from './lib/config';
	import { onMount } from 'svelte';

	const [getConfig, , refreshConfig] = useConfig();
	const config = getConfig();

	export let url = '';
	let menu: HTMLInputElement;

	const route = writable(globalHistory.location);
	globalHistory.listen((history) => {
		route.set(history.location);
		navbarMenuActive = false;
	});

	// history.pushState = new Proxy(history.pushState, {
	// 	apply(target, thisArg, argumentsList) {
	// 		Reflect.apply(target, thisArg, argumentsList);
	// 		scrollTo(0, 0);
	// 		menu.checked = false;
	// 	}
	// });

	let navbarMenuActive = false;
	function navBurgerClick() {
		navbarMenuActive = !navbarMenuActive;
	}

	const routes = [
		{ title: 'Dashboard', href: '/' },
		{ title: 'Config', href: '/config' },
		{ title: 'Restart', href: '/restart' }
	];

	let activeRoute = routes[0];
	$: activeRoute =
		[...routes].reverse().find((r) => $route.pathname.startsWith(r.href)) ?? routes[0];

	onMount(() => {
		refreshConfig();
	});
</script>

<svelte:head>
	<title>{$config.device_name}</title>
</svelte:head>

<Router {url}>
	<nav class="navbar is-fixed-top has-shadow is-dark" aria-label="main navigation">
		<div class="container">
			<div class="navbar-brand">
				<Link class="navbar-item" to="/">{$config.device_name}</Link>
				<span class="navbar-item" style="font-weight: 100;">{activeRoute.title}</span>

				<!-- svelte-ignore a11y-missing-attribute -->
				<!-- svelte-ignore a11y-interactive-supports-focus -->
				<!-- svelte-ignore a11y-click-events-have-key-events -->
				<a
					role="button"
					class="navbar-burger {navbarMenuActive ? 'is-active' : ''}"
					aria-label="menu"
					aria-expanded={navbarMenuActive ? 'true' : 'false'}
					on:click={navBurgerClick}
				>
					<span aria-hidden="true"></span>
					<span aria-hidden="true"></span>
					<span aria-hidden="true"></span>
				</a>
			</div>

			<div class="navbar-menu {navbarMenuActive ? 'is-active' : ''}">
				<div class="navbar-end">
					{#each routes.slice(1) as route}
						<Link class="navbar-item is-light" to={route.href}>{route.title}</Link>
					{/each}

					<a class="navbar-item" href="/update" target="_blank">Update</a>
				</div>
			</div>
		</div>
	</nav>

	<div class="container">
		<section class="section">
			<Route path="/config" component={Config} />
			<Route path="/restart" component={Restart} />
			<Route path="/"><Dashboard /></Route>
		</section>
	</div>
</Router>
