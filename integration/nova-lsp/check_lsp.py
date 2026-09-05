from __future__ import annotations

from pathlib import Path
from typing import Any

from mini_language_server.nova import NovaLanguageServer


def request(method: str, request_id: int, params: dict[str, Any] | None = None) -> dict[str, Any]:
    message: dict[str, Any] = {"jsonrpc": "2.0", "id": request_id, "method": method}
    if params is not None:
        message["params"] = params
    return message


def notification(method: str, params: dict[str, Any]) -> dict[str, Any]:
    return {"jsonrpc": "2.0", "method": method, "params": params}


def analyze(path: Path) -> tuple[list[tuple[str, str]], list[tuple[str | None, str]]]:
    server = NovaLanguageServer()
    initialized = server.handle(request("initialize", 1, {"capabilities": {}}))
    assert initialized is not None and "result" in initialized

    text = path.read_text(encoding="utf-8")
    uri = path.resolve().as_uri()
    server.handle(
        notification(
            "textDocument/didOpen",
            {
                "textDocument": {
                    "uri": uri,
                    "languageId": "nova",
                    "version": 1,
                    "text": text,
                }
            },
        )
    )

    semantics = server.semantics.get(uri)
    diagnostics = server.diagnostics.get(uri)
    assert semantics is not None
    assert diagnostics is not None
    symbols = [(symbol.name, symbol.kind) for symbol in semantics.symbols.symbols]
    emitted = [(item.code, item.message) for item in diagnostics.diagnostics]
    return symbols, emitted


def main() -> None:
    root = Path(__file__).resolve().parent

    valid_symbols, valid_diagnostics = analyze(root / "valid.nv")
    assert valid_symbols == [
        ("helper", "function"),
        ("main", "function"),
        ("value", "parameter"),
        ("local", "variable"),
    ]
    assert valid_diagnostics == []

    unresolved_symbols, unresolved_diagnostics = analyze(root / "unresolved.nv")
    assert unresolved_symbols == [("main", "function")]
    assert unresolved_diagnostics == [
        ("nova.unresolved-name", "unresolved name 'missing'")
    ]


if __name__ == "__main__":
    main()
