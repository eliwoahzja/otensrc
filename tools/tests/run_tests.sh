#!/bin/sh
# Build and run the headless ImGui verification suites for the Ethnir menu.
#
# These suites need only a host C++ compiler and the ImGui sources that are
# already vendored in this repository -- no Android SDK, no NDK and no device.
# That is deliberate: the menu is a header-only UI layer, so its behaviour can
# be regression-tested in CI far faster than an APK round-trip.
#
# Usage:  sh tools/tests/run_tests.sh
# Exits non-zero if any suite fails to build or fails a check.
#
# Optional: CXX=<compiler> to pick a compiler other than g++.

set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../.." && pwd)
BUILD="$ROOT/build/menu-tests"
CXX=${CXX:-g++}
JNI="$ROOT/src/main/jni"
IMGUI="$JNI/ImGui"

CXXFLAGS="-std=c++17 -O0 -g -w -include cstdint -I $JNI -I $IMGUI"
IMGUI_SRCS="$IMGUI/imgui.cpp $IMGUI/imgui_draw.cpp $IMGUI/imgui_widgets.cpp $IMGUI/imgui_tables.cpp"

SUITES="menu_harness controls_harness"
FAILED=""

# Compile-only check that no public ethnir name can collide with an ImGui name
# in a translation unit that imports both namespaces (the shape used by the tab
# bodies). This is what stopped the device build failing with
# "call to 'NextColumn' is ambiguous".
compile_shape_check() {
    echo ""
    echo "=== compile_shape_check ==="
    if "$CXX" $CXXFLAGS -fsyntax-only "$ROOT/tools/tests/compile_shape_check.cpp"; then
        echo "ok   public API does not collide with ImGui names under 'using namespace'"
        return 0
    fi
    echo "FAIL compile_shape_check (public API collides with ImGui names)"
    FAILED="$FAILED compile_shape_check"
    return 1
}

command -v "$CXX" >/dev/null 2>&1 || { echo "error: $CXX not found" >&2; exit 127; }
mkdir -p "$BUILD"

run_suite() {
    name=$1
    src=$2
    echo ""
    echo "=== $name ==="
    if ! "$CXX" $CXXFLAGS "$ROOT/tools/tests/$src" $IMGUI_SRCS -o "$BUILD/$name"; then
        echo "FAIL $name (compile error)"
        FAILED="$FAILED $name"
        return 1
    fi
    # Preserve the suite's exit status: it returns the number of failed checks.
    if "$BUILD/$name"; then
        return 0
    fi
    FAILED="$FAILED $name"
    return 1
}

for suite in $SUITES; do
    run_suite "$suite" "$suite.cpp" || true
done

compile_shape_check || true

# SaveConfig getter suite.
# SaveConfig.h binds to the game's `Config` settings struct, which lives in a
# private SDK header that is not in this repository, so the whole header cannot
# be compiled here. The ethcfg getter namespace is extracted verbatim instead,
# which keeps the checks bound to the live source rather than a stale copy.
echo ""
echo "=== saveconfig_getters_harness ==="
if awk '/^namespace ethcfg/{f=1} f{print} f&&/^}$/{exit}' "$JNI/System/Core/SaveConfig.h" \
        > "$BUILD/ethcfg_extracted.h"; then
    if "$CXX" -std=c++17 -O0 -g -w -I "$JNI" -I "$BUILD" \
            "$ROOT/tools/tests/saveconfig_getters_harness.cpp" -o "$BUILD/saveconfig_getters_harness"; then
        if "$BUILD/saveconfig_getters_harness"; then
            :
        else
            FAILED="$FAILED saveconfig_getters_harness"
        fi
    else
        echo "FAIL saveconfig_getters_harness (compile error)"
        FAILED="$FAILED saveconfig_getters_harness"
    fi
else
    echo "FAIL could not extract the ethcfg namespace from SaveConfig.h"
    FAILED="$FAILED saveconfig_getters_harness"
fi

echo ""
if [ -n "$FAILED" ]; then
    echo "MENU TESTS FAILED:$FAILED"
    exit 1
fi
echo "MENU TESTS PASSED"