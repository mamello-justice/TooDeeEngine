# TooDeeEngine Website

Documentation site for TooDeeEngine, built with [Astro](https://astro.build) + [Starlight](https://starlight.astro.build).

Content lives in `src/content/docs/` and is routed by file path:

| Directory | Route | Purpose |
|---|---|---|
| `docs/guides/` | `/guides/...` | Task-oriented how-to guides |
| `docs/reference/` | `/reference/...` | Requirements and architecture reference |
| `docs/project/` | `/project/...` | Contributing, roadmap, license |

The sidebar is defined in `astro.config.mjs`.

## Commands

The workspace is managed with [Nx](https://nx.dev). Run these from the repository root:

```bash
pnpm install
pnpm nx dev website       # dev server on localhost:4321
pnpm nx build website     # build to ./dist/
pnpm nx preview website   # preview the production build
```

See [Starlight's docs](https://starlight.astro.build/) and the [Astro documentation](https://docs.astro.build) for more.
