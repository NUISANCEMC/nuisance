# The NUISANCE Command Line Interface

THe NUISANCE v3 command line interface is designed to work in a similar way to many other modern command line tools with an entrypoint command, `nuis`, which is then passed one of a number of 'verbs' to interact with the various nuisance tools and helper scripts. The possible verbs are described below.

## `nuis` Verbs

* [`gen`](nuis-gen.md)
* [`flux`](nuis-flux.md)
* [`samples`](nuis-gen.md)
* [`comp`](nuis-comp.md)
* [`fit`](nuis-fit.md)
* [`flat`](nuis-flat.md)
* [`plot`](nuis-plot.md)
* [`info`](nuis-info.md)

## Implementing new Verbs

The main entrypoint command, `nuis`, forwards all arguments to the specialized commands `nuis-<verb>`. To implement a new verb, you simply need to write a new executable (shell script or binary) named `nuis-<verb>` that is findable via the `PATH` environment variable. 

When calling sub-commands, the `NUIS_BREADCRUMBS` environment variable is set so that sub-commands can print helpful error messages about how they are called: _e.g._ a script, named `nuis-gen-NEUT` should know that the user actually ran `nuis gen NEUT`. If new verbs implement similar sub-verb menus, the `NUIS_BREADCRUMBS` variable should be extended.