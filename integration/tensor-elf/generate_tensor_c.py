from __future__ import annotations

import sys
from pathlib import Path

from tiny_tensor_compiler import GraphBuilder, generate_c, lower_to_cpu, lower_to_loops


def main() -> None:
    if len(sys.argv) != 2:
        raise SystemExit("usage: generate_tensor_c.py OUTPUT.c")

    builder = GraphBuilder()
    lhs = builder.input((5,), dtype="int32")
    rhs = builder.input((5,), dtype="int32")
    program = lower_to_loops(lower_to_cpu(builder.finish(lhs + rhs)))
    source = generate_c(program)

    if "tiny_tensor_run" not in source:
        raise SystemExit("generated C ABI is missing tiny_tensor_run")
    Path(sys.argv[1]).write_text(source, encoding="utf-8")


if __name__ == "__main__":
    main()
