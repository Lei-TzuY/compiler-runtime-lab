"""Regenerate the assembler fixtures in isolation and compare their outputs."""

from pathlib import Path
import shutil
import subprocess
import sys
import tempfile


ROOT = Path(__file__).resolve().parent
CASES = ("test", "test_macro", "test_csect", "test_xe")
OUTPUT_SUFFIXES = ("expanded.asm", "int", "sym", "obj", "lst")


def main() -> int:
    failures = []

    with tempfile.TemporaryDirectory(prefix="sicxe-verify-") as temp_name:
        temp = Path(temp_name)
        for case in CASES:
            source = ROOT / f"{case}.asm"
            generated_source = temp / source.name
            shutil.copy2(source, generated_source)

            result = subprocess.run(
                [sys.executable, str(ROOT / "assembler.py"), str(generated_source)],
                cwd=ROOT,
                capture_output=True,
                text=True,
            )
            if result.returncode:
                failures.append(f"{case}: assembler exited {result.returncode}\n{result.stderr}")
                continue

            for suffix in OUTPUT_SUFFIXES:
                expected = ROOT / f"{case}.{suffix}"
                actual = temp / f"{case}.{suffix}"
                if not expected.exists():
                    failures.append(f"{case}: missing golden file {expected.name}")
                elif expected.read_bytes() != actual.read_bytes():
                    failures.append(f"{case}: {expected.name} differs from regenerated output")

    if failures:
        print("Assembler verification failed:")
        for failure in failures:
            print(f"- {failure}")
        return 1

    print(f"Assembler verification passed: {len(CASES)} fixtures, {len(OUTPUT_SUFFIXES)} outputs each")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
