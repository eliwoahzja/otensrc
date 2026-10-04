# Building & running the menu on-device with AIDE Pro

This repo has **no Android SDK/NDK in the Freebuff sandbox** (`ANDROID_HOME` and
`ANDROID_NDK_HOME` are unset, no `sdkmanager` / `ndk-build`). So the APK cannot be
produced here — it needs your AIDE Pro + NDK on the device.

This document covers getting the full APK built and running on the device.

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

`ethnir_menu.h` is a header included by `Main.cpp`, so editing it requires no
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
- Accent colour comes from `main_runtime_theme::g_menuHue = 0.5833f`
  (`imgui_settings.h`) — iOS system blue `#0A84FF`. Switches are iOS green
  `#34C759`. The six accent presets (Blue / Teal / Green / Gold / Pink / Violet)
  live in `ethnir::kAccents[]`.
- Log output is tagged `ETHNIR` (`Includes/Logger.h`).

## 6. Known-good state

On device: the menu opens centred, the sidebar sits in its own `BeginChild` pane
beside (never over) the content pane, one tab shows the sliding accent pill, and
toggles/sliders/combos respond to touch. Controls commit on **release** inside
their rect (`ethnir::EqPress`), so a drag that turns into a scroll never flips a
toggle.

## 7. Auto-save

`ethnir::MenuState` debounces every change by 500 ms (`SaveDebounce`) and calls
`MenuState::OnSave` — wired to `SaveConfiguration("ethnir")` in `Main.cpp`. The
debounce timer never advances while `InputActive` is set, so a slider being held
mid-drag cannot trigger a write; the save fires once the finger lifts. Writes go
through `System/Core/SaveConfig.h`, which writes `ethnir.json.tmp`, flushes,
checks the stream and renames over `ethnir.json` so a crash mid-write can never
leave a truncated config. The file carries `"version": 2` and every key is read
through `ethcfg::GetBool/GetFloat/GetInt`, which fall back to defaults for any
key the file is missing.

## 8. Local-only tooling

`site/` (browser preview of the shell) and `tools/` (headless C++ suites for the
menu layer) are git-ignored. They live on the development machine only and are
never part of the repo you build on the device:

```sh
sh tools/tests/run_tests.sh      # headless menu regression suites, exits non-zero on failure
node site/serve.mjs              # browser preview, http://localhost:4173
```

They deliberately are not wired into `assemble`/`preBuild`: AIDE builds this
project on an Android device where neither `sh` nor `g++` exists, so making
them a build dependency would break the on-device build.
