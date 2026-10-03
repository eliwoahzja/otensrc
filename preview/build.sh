#!/usr/bin/env bash
# Build + run the headless Equinox menu preview.
#
# Compiles the project's own ImGui sources and the real equinox_menu.h, links
# them with a surfaceless EGL/GLES2 context (llvmpipe), and writes PNGs.
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
JNI="$ROOT/src/main/jni"
OUT="${1:-$ROOT/preview/out}"
OBJ="$ROOT/preview/.build"

mkdir -p "$OUT" "$OBJ"

# The menu header includes ImGui headers with quoted paths, so the include dirs
# mirror the NDK layout. box_shadow.h lives under System/Texture.
INCS=(
  -I"$JNI"
)

# NDK builds with C++14 (see build.gradle / Android.mk).
FLAGS=(-std=c++14 -O1 -fPIC -w -include cstdint)

echo ">> compiling ImGui core"
for f in imgui imgui_draw imgui_tables imgui_widgets; do
  g++ "${FLAGS[@]}" "${INCS[@]}" -c "$JNI/ImGui/$f.cpp" -o "$OBJ/$f.o" &
done
# GL ES 3 backend: matches the "#version 300 es" string Main.cpp passes, and
# makes the backend include <GLES3/gl3.h> instead of the desktop gl3w loader.
g++ "${FLAGS[@]}" "${INCS[@]}" -DIMGUI_IMPL_OPENGL_ES3 \
    -c "$JNI/ImGui/imgui_impl_opengl3.cpp" -o "$OBJ/gl3.o" &
# AddShadowRect lives here.
g++ "${FLAGS[@]}" "${INCS[@]}" -c "$JNI/System/Texture/box_shadow.cpp" -o "$OBJ/box_shadow.o" &
wait

echo ">> compiling preview harness"
g++ "${FLAGS[@]}" "${INCS[@]}" -c "$ROOT/preview/eq_preview.cpp" -o "$OBJ/eq_preview.o"

echo ">> linking"
# Link only the objects this harness needs rather than globbing preview/.build.
g++ "$OBJ"/imgui.o "$OBJ"/imgui_draw.o "$OBJ"/imgui_tables.o "$OBJ"/imgui_widgets.o \
    "$OBJ"/gl3.o "$OBJ"/box_shadow.o "$OBJ"/eq_preview.o \
    -o "$OBJ/eq_preview" -lEGL -lGLESv2 -lpthread

echo ">> rendering to $OUT"
EGL_PLATFORM=surfaceless LIBGL_ALWAYS_SOFTWARE=1 "$OBJ/eq_preview" "$OUT"

# Derive the easy-to-open view crops and contact sheet. Skipped with a note if
# Pillow is missing -- the PNGs above are already written either way.
if command -v python3 >/dev/null 2>&1 && python3 -c 'import PIL' >/dev/null 2>&1; then
    echo ">> deriving views"
    python3 "$ROOT/preview/make_views.py"
else
    echo ">> skipping view generation (python3 + Pillow not available)"
fi
