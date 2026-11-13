# Downhill Native

[![Android API](https://img.shields.io/badge/Android-8.0%2B%20(API%2026)-3DDC84?logo=android&logoColor=white)](#requirements)
[![Language](https://img.shields.io/badge/runtime-C%2B%2B20%20%2B%20Java-00599C?logo=cplusplus&logoColor=white)](#architecture)
[![License](https://img.shields.io/badge/license-MIT-D6B35C.svg)](LICENSE)

A native Android compatibility-runtime foundation focused exclusively on **Downhill Domination USA / NTSC-U**. The long-term objective is a native Android reimplementation that reads a legally user-supplied disc image locally, rather than shipping commercial game data or a generic PlayStation 2 emulator.

> **Current release candidate: v0.1.5 — Stage 04 Recursive ISO Index.** The app reads a bounded recursive directory tree (depth 4, 512 entries, 96 displayed lines) plus image statistics from a user-selected ISO, classifies it against the Downhill Domination NTSC-U boot serial (`SLUS_206.47`), and runs the native OpenGL ES 3.0 diagnostic scene. It is **not playable yet**, does not execute PS2 code, and a USA candidate verdict is not yet a supported-build approval.

## Project principles

- **One target:** Downhill Domination USA / NTSC-U. This is not a multi-game PS2 compatibility project.
- **Native runtime:** Android UI/lifecycle uses Java; performance-critical runtime components use C++20, JNI, Android NDK, and CMake.
- **User-owned media:** the user supplies an ISO dumped from a disc they own through Android's system file picker.
- **No bundled game content:** no ISO, BIOS, executable, model, texture, music, movie, or extracted game asset is included.
- **Evidence before claims:** compatibility and performance claims are only made after automated tests and real-device validation.

## What v0.1.2 does

- Supports Android 8.0+ (API 26) with `targetSdk 35`.
- Opens a user-selected image with Android Storage Access Framework (SAF).
- Persists **read-only** URI access; the ISO remains at its original location.
- Passes a read-only file descriptor to the native C++ runtime through JNI.
- Validates the ISO-9660 Primary Volume Descriptor at sector 16.
- Reads bounded ISO-9660 root-directory metadata safely using positional native reads.
- Displays the volume identifier and up to 32 root entries with byte sizes.
- Indexes a bounded recursive directory tree (depth 4, 512 entries, 96 lines) with aggregated image statistics.
- Classifies the image against the Downhill Domination NTSC-U boot serial with conservative verdicts: `UNSUPPORTED_LAYOUT`, `NON_TARGET_PS2_DISC`, or `USA_CANDIDATE` (see [Stage 02](docs/stages/02-downhill-usa-fingerprint.md)).
- Launches an original OpenGL ES 3.0 landscape diagnostic scene through the C++/JNI runtime.
- Reports diagnostic FPS and normalized touch/gamepad input values.
- Supports diagnostic touch steering/throttle/brake plus Android joystick, D-pad, trigger, and common face/shoulder button input.
- Builds native libraries for `arm64-v8a`, `armeabi-v7a`, `x86`, and `x86_64`.
- Uses an original black-and-gold adaptive launcher icon; no third-party game artwork is used.

Detailed behavior and limits: [Stage 01 ISO Explorer](docs/stages/01-iso-explorer.md).

## What v0.1.2 does **not** do

- Play Downhill Domination.
- Emulate the PS2 Emotion Engine, VU units, Graphics Synthesizer, BIOS, or execute PS2 MIPS binaries.
- Confirm a selected ISO is the supported USA build yet.
- Extract, copy, cache, decrypt, or redistribute game assets.
- Support PAL/Japan revisions, arbitrary PS2 titles, UDF/Joliet filesystems, subdirectories, or multi-extent files at this stage.

## Compatibility roadmap

| Stage | Deliverable | Status |
|---|---|---|
| 01 | Native ISO-9660 Explorer, SAF read-only flow, root metadata report | **Complete — v0.1.2** |
| 02 | Strict Downhill Domination USA layout/fingerprint gate | **Partial — v0.1.4** (serial verdict; full manifest gate pending) |
| 03 | Native OpenGL ES diagnostic runtime, lifecycle, touch/gamepad input | **Complete — v0.1.3** |
| 04 | Bounded recursive ISO index and image statistics | **Complete — v0.1.5** |
| 05 | Playable vertical slice: course segment, rider, input, camera, collision, checkpoint | Planned |
| 06 | Device compatibility matrix, quality profiles, release hardening | Planned |

The engineering plan is available at [`.hermes/plans/2026-11-12_160000-native-iso-runtime.md`](.hermes/plans/2026-11-12_160000-native-iso-runtime.md).

## Architecture

```text
User-owned Downhill Domination USA ISO
              │
              ▼
Android Storage Access Framework (read-only URI permission)
              │
              ▼
Java Android shell ── ParcelFileDescriptor / JNI ──► C++20 NDK runtime
                                                      │
                                                      ▼
                                            ISO-9660 parser and
                                            Downhill-USA compatibility gate
                                                      │
                                                      ▼
                                    future: asset adapters → renderer → gameplay
```

The ISO remains external to the application. Native code reads only the metadata necessary for the current stage and uses hard bounds checks for directory parsing.

## Requirements

### Device

- Android 8.0 or newer (API 26+).
- A file provider that supports Android's system document picker.
- A personally dumped ISO for local inspection. The app deliberately includes no media.

### Local development

- JDK 21
- Android SDK Platform 35
- Android Build Tools 35.x
- Android NDK `27.2.12479018`
- Android SDK CMake `3.22.1`

The SDK/NDK, Gradle caches, generated binaries, ISO files, and other local data are intentionally excluded from Git.

## Build and test

From the canonical working directory:

```bash
cd /opt/data/apps/recompile-downhill

JAVA_HOME="$PWD/.tooling/jdk" \
PATH="$PWD/.tooling/jdk/bin:$PATH" \
./gradlew testDebugUnitTest assembleDebug --no-daemon
```

The debug APK is generated at:

```text
app/build/outputs/apk/debug/app-debug.apk
```

### Test coverage today

- `IsoImageValidatorTest`: valid, invalid, and truncated ISO-9660 Primary Volume Descriptor sectors.
- `IsoExplorerResultTest`: safe success, truncation, and failure report presentation.
- Native runtime is compiled for all configured ABIs during `assembleDebug`.

## Install a debug build

Use Android Debug Bridge with a connected test device:

```bash
adb install -r app/build/outputs/apk/debug/app-debug.apk
```

Then open **Downhill Native**, choose an ISO through the system picker, and review the metadata report. A successful report is only evidence that the ISO structure was read; it is not a game-compatibility confirmation.

## Legal and data policy

- Repository code is original and licensed under [MIT](LICENSE).
- Users are responsible for creating and using lawful personal disc backups.
- Do not submit ISO images, BIOS files, executable files, screenshots containing extracted proprietary assets, or archive dumps in issues or pull requests.
- Git ignores `*.iso`, `*.bin`, `*.cue`, `*.chd`, and `*.elf`, plus Android build outputs and local toolchains.
- This project is not affiliated with, endorsed by, or sponsored by the owners of Downhill Domination or PlayStation.

## Project layout

```text
app/
  src/main/java/     Android UI, SAF permissions, JNI bridge
  src/main/cpp/      Native C++20 ISO/runtime foundation
  src/test/java/     JUnit behavior tests
docs/stages/         Stage-specific technical documentation
.hermes/plans/       Implementation roadmap and acceptance gates
```

## Contributing

Changes must remain scoped to the Downhill Domination USA compatibility path, avoid proprietary content, include appropriate automated tests, update stage documentation, and pass the documented build before being committed.

## License

Source code in this repository is licensed under the [MIT License](LICENSE). Game titles, trademarks, and related assets remain property of their respective owners.
