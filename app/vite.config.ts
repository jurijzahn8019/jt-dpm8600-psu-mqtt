import { svelte } from '@sveltejs/vite-plugin-svelte'
import { globbySync } from 'globby';
import { readFileSync, writeFileSync } from 'node:fs';
import { join } from 'node:path';
import type { ResolvedConfig } from 'vite';
import { defineConfig } from 'vitest/config';

let viteConfig: ResolvedConfig;

export default defineConfig({
	build: {
		// outDir: "../data",
		rollupOptions: {
			output: {
				entryFileNames: "[name].js",
				chunkFileNames: "[name].js",
				assetFileNames: "[name].[ext]",
			}
		},
		assetsInlineLimit: 9186
	},
	plugins: [svelte(), {
		name: "vite:save-as-progmem",
		configResolved(config) {
			viteConfig = config
		},
		buildEnd() {
			const files = globbySync("*", { cwd: viteConfig.build.outDir })
			files.forEach(f => {
				const signature = f.replaceAll(/[\s\W]/g, "_");
				const data = readFileSync(join(viteConfig.build.outDir, f));

				const converted = [...new Uint8Array(data)]
				.map(x => `0x${x.toString(16).toUpperCase().padStart(2, '0')}`)
				.join(',')

				writeFileSync(join("../include", `${signature}.h`),  `
#ifndef ${signature}_h
#define ${signature}_h

const char ${signature}[] PROGMEM = {${converted},0x00};

#endif
				`);
			});
		}
	}],

	test: {
		include: ['src/**/*.{test,spec}.{js,ts}']
	}
});
