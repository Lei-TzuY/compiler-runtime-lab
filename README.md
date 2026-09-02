# mini-language-server

A language-independent language-server and compiler-tooling laboratory.

The project starts with a deliberately small, correctness-first JSON-RPC/LSP transport and lifecycle core. Nova will become the first serious language adapter, but protocol, document, indexing, and semantic-query infrastructure remain language-independent.

## Initial scope

The first milestone covers only:

- LSP `Content-Length` message framing
- JSON-RPC request/notification parsing
- `initialize` / `initialized` / `shutdown` / `exit` lifecycle handling
- structured JSON-RPC errors
- deterministic protocol tests

Document synchronization, incremental syntax, symbol indexing, diagnostics, and language adapters come later as separate milestones.
