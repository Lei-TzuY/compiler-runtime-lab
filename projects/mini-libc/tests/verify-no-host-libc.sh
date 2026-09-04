#!/bin/sh
set -eu

if [ "$#" -eq 0 ]; then
    echo "usage: $0 EXECUTABLE..." >&2
    exit 2
fi

for executable in "$@"; do
    if readelf -l "$executable" | grep -q 'INTERP'; then
        echo "$executable unexpectedly contains a PT_INTERP segment" >&2
        exit 1
    fi

    if readelf -d "$executable" 2>/dev/null | grep -q '(NEEDED)'; then
        echo "$executable unexpectedly has a dynamic dependency" >&2
        exit 1
    fi

    undefined="$(nm -u "$executable")"
    if [ -n "$undefined" ]; then
        echo "$executable contains undefined symbols:" >&2
        echo "$undefined" >&2
        exit 1
    fi

done

echo "ELF dependency checks passed"
