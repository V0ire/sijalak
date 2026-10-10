#!/usr/bin/env bash
# Run all firmware host suites under ASan+UBSan. Used as audit/sanitizer evidence.
set -euo pipefail
cd "$(dirname "$0")/../.."
export TMPDIR="$PWD/.buildtmp"
mkdir -p "$TMPDIR"

INC="-I firmware/components/jalak_logic/include -I firmware/components/jalak_hal/include"
SRCS="firmware/components/jalak_logic/src/api_client.cpp \
      firmware/components/jalak_logic/src/audio.cpp \
      firmware/components/jalak_logic/src/expression.cpp \
      firmware/components/jalak_logic/src/framebuffer.cpp \
      firmware/components/jalak_logic/src/state_machine.cpp \
      firmware/components/jalak_logic/src/oled_codec.cpp"

fail=0
for t in test_main test_hal test_api test_integration test_oled; do
    echo "=== ASan/UBSan: $t ==="
    if g++ -std=c++20 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
         $INC "firmware/test/$t.cpp" $SRCS -o "$TMPDIR/asan_$t" 2>&1; then
        "$TMPDIR/asan_$t" || fail=1
    else
        echo "COMPILE FAILED: $t"; fail=1
    fi
done
exit $fail
