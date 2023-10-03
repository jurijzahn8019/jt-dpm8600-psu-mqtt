import { svelte } from '@sveltejs/vite-plugin-svelte';
import { globbySync } from 'globby';
import { writeFileSync, createReadStream } from 'node:fs';
import { join } from 'node:path';
import type { ResolvedConfig } from 'vite';
import { defineConfig } from 'vitest/config';
import { createGzip, constants } from 'zlib';

let viteConfig: ResolvedConfig;

export default defineConfig({
	build: {
		// outDir: "../data",
		rollupOptions: {
			output: {
				entryFileNames: '[name].js',
				chunkFileNames: '[name].js',
				assetFileNames: '[name].[ext]'
			}
		},
		assetsInlineLimit: 9186
	},
	plugins: [
		svelte(),
		{
			name: 'vite:save-as-progmem',
			configResolved(config) {
				viteConfig = config;
			},
			async buildEnd() {
				const files = globbySync('*', { cwd: viteConfig.build.outDir });
				await Promise.all(files.map(async (f) => {
					const signature = f.replaceAll(/[\s\W]/g, '_');


					const chunks = [];
					for await (const chunk of createReadStream(join(viteConfig.build.outDir, f)).pipe(
						createGzip({ level: constants.Z_BEST_COMPRESSION })
					)) {
						chunks.push(chunk);
					}
					const gz = Buffer.concat(chunks);

					const converted = [...new Uint8Array(gz)]
						.map((x) => `0x${x.toString(16).toUpperCase().padStart(2, '0')}`)
						.join(',');

					writeFileSync(
						join('../include', `${signature}.h`),
						`
#ifndef ${signature}_h
#define ${signature}_h

#define ${signature}_gz_len ${gz.length}
const uint8_t ${signature}_gz[] PROGMEM = {${converted}};

#endif
				`
					);
				}));
			}
		}
	],

	test: {
		include: ['src/**/*.{test,spec}.{js,ts}']
	}
});
