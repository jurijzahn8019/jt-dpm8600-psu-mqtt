import { svelte } from '@sveltejs/vite-plugin-svelte'
import { defineConfig } from 'vitest/config';

export default defineConfig({
	build: {
		outDir: "../data",
		rollupOptions: {
			output: {
				entryFileNames: "assets/[name].js",
				chunkFileNames: "assets/[name].js",
				assetFileNames: "assets/[name].[ext]",
			}
		}
	},
	plugins: [svelte()],

	test: {
		include: ['src/**/*.{test,spec}.{js,ts}']
	}
});
