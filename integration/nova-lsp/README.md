# Nova ↔ mini-language-server integration contract

This directory contains shared Nova source fixtures consumed by both imported projects.

The contract is deliberately bounded:

- `valid.nv` must be accepted by the imported Nova CLI and must publish no mini-language-server Nova diagnostics.
- `unresolved.nv` must be rejected by Nova with semantic diagnostic `N3003` and must publish exactly one mini-language-server diagnostic, `nova.unresolved-name`, for `missing`.
- The mini-language-server side also proves that the legal typed Nova signature surface produces function, parameter and local symbols.

This is not a claim that mini-language-server implements the full Nova grammar or type system. The adapter still owns a bounded tooling subset; the shared fixtures lock only the syntax/semantic surface both projects explicitly support.
