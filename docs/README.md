# Documentation

Start with [`../CLAUDE.md`](../CLAUDE.md) — it holds the build commands, the repo map
and the invariants that are expensive to rediscover. Everything here goes deeper.

| Folder | Contents |
|---|---|
| [01-usage](01-usage/) | How to actually use the application, screen by screen |
| [02-architecture](02-architecture/) | How the software is built: targets, threads, data model, export pipeline, and the workarounds that bridge editor concepts to the Decent Sampler format |
| [03-decent-sampler-reference](03-decent-sampler-reference/) | The official Decent Sampler developer guide (PDF) plus format behaviour we have confirmed in practice |
| [04-planning](04-planning/) | Backlog, technical debt, recorded decisions, and the plans behind current work |

## Conventions

- Documents cite **file paths and symbol names, never line numbers**.
- Claims that could not be verified are marked `> **To confirm**`.
- Each document ends with a `Last verified:` line.

*Last verified: 2026-08-19*
