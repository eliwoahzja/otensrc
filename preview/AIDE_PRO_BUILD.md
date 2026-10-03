# Building & running the menu on-device with AIDE Pro

This repo has **no Android SDK/NDK in the Freebuff sandbox** (`ANDROID_HOME` and
`ANDROID_NDK_HOME` are unset, no `sdkmanager` / `ndk-build`). So the APK cannot be
produced here — it needs your AIDE Pro + NDK on the device.

The headless preview under `preview/` already renders the real
`src/main/jni/ImGui/equinox_menu.h` offscreen, so the menu code itself is verified.
This document covers getting the full APK built and injected.

## 1. Point AIDE Pro at the repo

1. Copy/clone the repo onto the device (or open the folder AIDE Pro already has).
2. In AIDE Pro: **Open project → pick the repo root** (the folder containing
   `build.gradle`, `src/`, `settings.gradle`).
3. AIDE Pro must treat it as an Android Studio/Gradle project so that
   `externalNativeBuild { ndkBuild { path file('src/main/jni/Android.mk') } }`
   in `build.gradle` is honoured.

## 2. NDK settings that matter

`build.gradle` already pins what the sources expect:

| Setting | Value | Where |
|---|---|---|
| `ndkVersion` | `22.0.7026061` | `build.gradle` |
| `compileSdkVersion` / `targetSdkVersion` | `29` | `build.gradle` |
| `minSdkVersion` | `19` | `build.gradle` |
| ABI filters | `arm64-v8a`, `armeabi-v7a` | `build.gradle` `ndk.abiFilters` |
| GL | `ImGui_ImplOpenGL3_Init("#version 300 es")`, links `-lGLESv3 -lGLESv2` | `Main.cpp`, `Android.mk` |

If your AIDE Pro NDK is a different revision, either set it to r22b
(`22.0.7026061`) or delete the `ndkVersion` line — the code uses only long-stable
NDK features, so any recent NDK works.

`Android.mk` builds everything by wildcard, so no new source files need to be
registered for a menu-only change:

```
FILE_LIST := $(wildcard $(LOCAL_PATH)/ImGui/*.c*)
...
FILE_LIST += $(wildcard $(LOCAL_PATH)/System/Texture/*.c*)
FILE_LIST += $(wildcard $(LOCAL_PATH)/*.c*)
```

`equinox_menu.h` is a header included by `Main.cpp` (line 28) and
`runtime_preview_menu.h` (line 4), so editing it requires no `Android.mk` change.

## 3. Build

From AIDE Pro use **Build → Build project** (or the gradle task). Equivalent on a
device shell with the NDK on `PATH`:

```sh
cd /path/to/repo
# AIDE Pro's bundled NDK lives under its private dir; adjust to yours:
export ANDROID_NDK_HOME=/data/user/0/com.aide.plus/no_backup/ndksupport-*/android-ndk-aide
export PATH="$ANDROID_NDK_HOME/ndk-build:$PATH"

ndk-build -C src NDK_PROJECT_PATH=. NDK_APPLICATION_MK=src/main/jni/Android.mk \
           APP_BUILD_SCRIPT=src/main/jni/Android.mk \
           NDK_OUT=src/main/obj NDK_LIBS_OUT=src/main/libs \
           APP_ABI=arm64-v8a
```

The output shared library lands in `src/main/libs/arm64-v8a/`.

Gradle path (what AIDE Pro's *Build project* runs):

```sh
./gradlew assembleDebug     # or: gradle assembleDebug
```

## 4. What the hook expects at runtime

- Package: `com.kazex404.imguigr` (`build.gradle` `applicationId`).
- Native lib loaded into the game process; `Main.cpp` installs an **xhook on
  `eglSwapBuffers`** so the menu draws once per frame on the game's GL context.
- Fonts are built in `Main.cpp` lines ~258–305: `inter_semibold` 16f,
  `icomoon_page` 18f, `F107` icon font 25f (merged over `{0xe000, 0xf8ff}`),
  `F50` 30f. The preview harness loads the exact same four blobs.
- Accent colour comes from `main_runtime_theme::g_menuHue = 0.78f`
  (`imgui_settings.h`), i.e. the purple you see in the screenshots. The theme
  buttons in the header retint it at runtime.

## 5. Verifying a menu-only change quickly

If you only touched `equinox_menu.h`, you do not need the device round-trip to
check layout. The preview renders that exact header headlessly:

```sh
sh preview/build.sh          # compiles ImGui + the header, writes PNGs
python3 preview/inspect.py   # structural assertions over the PNGs
```

Requires `libEGL1-mesa-dev libgles2-mesa-dev libosmesa6-dev` and `python3-pil`,
and runs under `EGL_PLATFORM=surfaceless LIBGL_ALWAYS_SOFTWARE=1` (llvmpipe).

## 6. Known-good state

`preview/inspect.py` must end with `ALL STRUCTURAL CHECKS PASSED`. It asserts, per
shot: the glass shell is present, the sidebar is darker than the content pane, an
accent pill marks the active tab, content rows render, the search filter reduces
rows, and the combo dropdown paints a real panel.