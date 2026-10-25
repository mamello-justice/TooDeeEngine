# Data-Driven Project Example

This project is a data-driven game definition, not a standalone app binary. Run it with the repo's CLI wrapper from the repository root:

```bash
just run examples/tde_project
```

`main.tde` defines the window and startup scene. Its `[config.assets]
file=assets.ini` reference loads global assets relative to the project manifest.
Assets can be split across files with sections such as `[config.fonts]
file=fonts.ini` and `[config.textures] file=textures.ini`; the suffix is only a
label for users. Each referenced file contains the named asset entries such as
`[font.default]`. The section hierarchy represents
scene data using singular dotted names, for example `[scene.hello.entity.greeting]`
and `[scene.hello.entity.greeting.component.label]`.

Scene definitions can be moved to separate files by setting `file=...` in a
`[scene.<id>]` section; the referenced file must define the same section. The
project can be launched with `tde examples/tde_project` from the repository root.

## Next Steps

- [Scenes](../tde_scenes/README.md) - Separate config into multiple files
