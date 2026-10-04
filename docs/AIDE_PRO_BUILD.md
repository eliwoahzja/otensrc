# Building & running the menu on-device with AIDE Pro

This repo has **no Android SDK/NDK in the Freebuff sandbox** (`ANDROID_HOME` and
`ANDROID_NDK_HOME` are unset, no `sdkmanager` / `ndk-build`). So the APK cannot be
produced here — it needs your AIDE Pro + NDK on the device.

The UI itself is previewed as a web page under `site/` (see section 6), which
mirrors `src/main/jni/ImGui/equinox_menu.h`. This document covers getting the full
APK built and running on the device.

## 1. Point AIDE Pro at the repo

1. Copy/clone the repo onto the device (or open the folder AIDE Pro already has).
2. In AIDE Pro: **Open project → pick the repo root** (the folder containing
   `build.gradle` and `src/`).
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

`equinox_menu.h` is a header included by `Main.cpp`, so editing it requires no
`Android.mk` change.

### Always do a clean rebuild after pulling

The repo ships AIDE's build outputs (`build/`, `src/main/obj/`) from an older
build, including compiled `ImGui` objects. `make` compares timestamps, so an
incremental AIDE build can link objects built from **old** sources and keep
reproducing bugs you already fixed in the tree — or fail on a header that no
longer exists in the new sources.

Before building after a pull, delete both output trees from the device:

```sh
rm -rf build src/main/obj src/main/libs
```

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

## 4. Launcher activity (this is what made the app close on open)

`src/main/AndroidManifest.xml` declared the launcher as
`com.kazex404.imguigr.MainActivity`, but **no such class exists** — the only
activity in the project (and in the built `classes.dex`) is
`com.android.support.MainActivity`. Android instantiates the activity named in
the manifest, fails with `ClassNotFoundException`, and finishes the task again:
the icon flashes and the app closes immediately.

The manifest now points at the real class:

```xml
<activity
    android:name="com.android.support.MainActivity"
    android:exported="true"
    android:screenOrientation="sensorLandscape">
```

If you rebuild and it still closes the moment it opens, capture the reason from
the device instead of guessing:

```sh
adb logcat -d | grep -E "AndroidRuntime|LoadLibrary|StartGame|ActivityManager: .*com.kazex404"
```

`AndroidRuntime: FATAL EXCEPTION` gives the class and line; `LoadLibrary` /
`StartGame` come from `MainActivity` itself.

## 5. What the native side expects at runtime

- Package: `com.kazex404.imguigr` (`build.gradle` `applicationId`).
- `MainActivity` downloads its payload lib into the app's files dir, `System.load`s
  it (now inside a `try/catch`, so a corrupt download shows a toast instead of
  killing the app) and then launches `com.tencent.tmgp.cod.CODMainActivity`.
- The native lib installs an **xhook on `eglSwapBuffers`** so the menu draws once
  per frame on the game's GL context.
- Fonts are built in `Main.cpp` lines ~258–305: `inter_semibold` 16f,
  `icomoon_page` 18f, `F107` icon font 25f (merged over `{0xe000, 0xf8ff}`),
  `F50` 30f.
- Accent colour comes from `main_runtime_theme::g_menuHue = 0.78f`
  (`imgui_settings.h`) — the purple in the preview (`#BF38FF`).

## 6. The site preview

`site/` is a dependency-free page that recreates the Equinox shell in HTML/CSS/JS
from the same constants as `equinox_menu.h` (780x480 shell, 186px sidebar, six
tabs, purple accent, kCard/kEdge/kTextHi/kTextLo tokens). It is interactive:

- click the sidebar tabs — the accent pill slides and the content fades
- type in the header search box — rows filter live, and `nomatch` shows the
  "NO MATCHES" card on the four filtering tabs
- click a combo row — the dropdown opens (and flips above near the bottom)
- drag the header strip — the window moves, clamped to the viewport
- traffic lights: red collapses to the floating pill, yellow jumps to VISUALS,
  green swaps the backdrop

Serve it locally:

```sh
node site/serve.mjs        # http://localhost:4173
```

It is static, so any file server or host works; there is no build step.

## 7. Known-good state

On device: the menu opens centred, the sidebar is darker than the content pane,
one tab shows the accent pill, and toggles/sliders/combos respond to touch. In the
site preview the same five behaviours are verifiable in a browser, which is the
quickest place to check a layout-only change without a device round-trip.
