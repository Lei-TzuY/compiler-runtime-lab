# SIC/XE Assembler and Linking Loader

A small systems-programming project that implements macro expansion, a two-pass SIC/XE assembler, control sections, external definitions/references, and a linking loader.

## Run the assembler

```powershell
python assembler.py test_xe.asm
```

For `program.asm`, the assembler writes `program.expanded.asm`, `program.int`, `program.sym`, `program.obj`, and `program.lst` beside the source file.

## Verify the checked-in fixtures

```powershell
python verify.py
```

The verifier assembles `test.asm`, `test_macro.asm`, `test_csect.asm`, and `test_xe.asm` in a temporary directory and byte-compares all generated outputs with the checked-in golden files.

## Scope

This is an educational SIC/XE implementation, not a production toolchain. The fixtures cover ordinary format 1–4 instructions, PC-relative addressing, macros, control sections, external symbols, relocation records, and loader input.
