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

## Deployment

The `Deploy website` GitHub Actions workflow builds the site for pull requests and deploys it to GitHub Pages on pushes to `main` (or when run manually). In the repository settings, set **Pages → Build and deployment → Source** to **GitHub Actions**. The deployed site is served from `https://mamello-justice.github.io/TooDeeEngine/`.
