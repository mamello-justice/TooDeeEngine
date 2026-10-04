// @ts-check
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { defineConfig } from 'astro/config';
import starlight from '@astrojs/starlight';
import starlightBlog from 'starlight-blog';
import { normalizePath } from 'vite';
import { viteStaticCopy } from 'vite-plugin-static-copy';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const ASSETS_DIR = normalizePath(path.resolve(__dirname, '../../assets'))
const ICONS = normalizePath(path.resolve(__dirname, '../../assets/too-dee-icons'))
const base = process.env.ASTRO_BASE ?? '/'

// https://astro.build/config
export default defineConfig({
	site: process.env.ASTRO_SITE,
	base,
	integrations: [
		starlight({
			plugins: [starlightBlog()],
			title: 'TooDeeEngine',
			description: 'Documentation for the TooDeeEngine 2D game engine.',
			favicon: '/assets/too-dee-icons/svg/too-dee-icon-gradient.svg',
			logo: {
				src: '#root-assets/too-dee-icons/svg/too-dee-icon-gradient.svg',
				replacesTitle: false,
			},
			social: [{ icon: 'github', label: 'GitHub', href: 'https://github.com/mamello-justice/TooDeeEngine' }],
			sidebar: [
				{
					label: 'Guides',
					items: [
						{ label: 'Getting Started', slug: 'guides/getting-started' },
						{ label: 'Installation', slug: 'guides/installation' },
						{ label: 'Configuration', slug: 'guides/configuration' },
						{ label: 'Examples', slug: 'guides/examples' },
						{ label: 'Packaging', slug: 'guides/packaging' },
					],
				},
				{
					label: 'Reference',
					items: [
						{ autogenerate: { directory: 'reference' } },
					],
				},
				{
					label: 'Project',
					items: [
						{ label: 'Contributing', slug: 'project/contributing' },
						{ label: 'Roadmap', slug: 'project/roadmap' },
						{ label: 'License', slug: 'project/license' },
					],
				},
			],
		}),
	],
	vite: {
		resolve: {
			alias: {
				// This maps a shortcut alias straight back to your monorepo root folder
				'#root-assets': ASSETS_DIR
			},
		},
		plugins: [
			viteStaticCopy({
				targets: [
					{
						src: ICONS,
						dest: '.',
					},
				],
			})
		]
	}
});
