#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
OUT=${OUT:-"$ROOT/build/tensor-elf-integration"}
TENSOR="$ROOT/projects/tiny-tensor-compiler"
TINY_C="$ROOT/projects/tiny-c-compiler"
LIBC="$ROOT/projects/mini-libc"
ELF="$ROOT/projects/mini-elf-toolchain"

rm -rf "$OUT"
mkdir -p "$OUT/libc"

PYTHONPATH="$TENSOR/src${PYTHONPATH:+:$PYTHONPATH}" \
    python3 "$ROOT/integration/tensor-elf/generate_tensor_c.py" "$OUT/generated.c"

grep -q 'void tiny_tensor_run' "$OUT/generated.c"

cc -std=c11 -O1 -ffreestanding -fno-builtin -fno-pie -fno-pic \
    -fno-stack-protector -fno-asynchronous-unwind-tables -fno-unwind-tables \
    -mno-red-zone -c "$OUT/generated.c" -o "$OUT/tensor.o"

if nm -u "$OUT/tensor.o" | grep -q .; then
    echo "generated tensor object has unresolved symbols:" >&2
    nm -u "$OUT/tensor.o" >&2
    exit 1
fi

make -C "$TINY_C" minicc
MINICC="$TINY_C/minicc"
test -x "$MINICC"

objects=""
for source in $(find "$LIBC/src" -type f -name '*.c' -print | sort); do
    name=$(printf '%s' "$source" | sed "s#^$LIBC/##" | tr '/.' '__')
    object="$OUT/libc/$name.o"
    "$MINICC" -nostdinc -I"$LIBC/include" -c "$source" -o "$object"
    objects="$objects $object"
done

cc -fno-pie -c "$LIBC/src/syscall/syscall.S" -o "$OUT/syscall.o"
cc -fno-pie -c "$LIBC/src/crt/crt0.S" -o "$OUT/crt0.o"
ar rcs "$OUT/libc.a" $objects "$OUT/syscall.o"

"$MINICC" -nostdinc -I"$LIBC/include" \
    -c "$ROOT/integration/tensor-elf/harness.c" -o "$OUT/harness.o"

cargo build --quiet --manifest-path "$ELF/Cargo.toml" --bin mini-elf-toolchain \
    --target-dir "$OUT/mini-elf-target"
LINKER="$OUT/mini-elf-target/debug/mini-elf-toolchain"
test -x "$LINKER"

"$LINKER" link -o "$OUT/tensor-elf" \
    "$OUT/harness.o" "$OUT/tensor.o" "$OUT/crt0.o" "$OUT/libc.a"

output=$("$OUT/tensor-elf")
if [ "$output" != "tensor-elf-ok" ]; then
    echo "unexpected tensor executable output: $output" >&2
    exit 1
fi

"$LIBC/tests/verify-no-host-libc.sh" "$OUT/tensor-elf"
readelf -h "$OUT/tensor-elf" | grep -q 'Class:.*ELF64'
readelf -h "$OUT/tensor-elf" | grep -q 'Type:.*EXEC'

echo "tiny-tensor-compiler -> C object -> mini-libc -> mini-elf-toolchain executable integration passed"
