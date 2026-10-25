# Project with External Scene Example

This is a data-driven project manifest, not a standalone executable. Run it with
the repo's CLI wrapper from the repository root:

```bash
just run examples/tde_scenes
```

The project manifest references its asset config, window settings, and startup
scene. Each path is resolved relative to the document that contains the
reference. Use one or more `[config.<name>] file=...` sections to load combined
or split asset config files; the section suffix is a user-facing label. Asset
entries in those files keep their named sections such as `[font.default]` and
`[texture.player]`.
