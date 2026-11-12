# Downhill Native

Native Android foundation for a user-supplied PlayStation 2 game image workflow.

> **Project status: proof of concept.** This build can select an ISO with Android's system file picker and verifies the ISO-9660 primary volume descriptor. It does **not** play, emulate, decrypt, extract, or include any commercial game content.

## Legal and asset policy

- This repository contains only original application code under the [MIT License](LICENSE).
- No ISO, PS2 BIOS, executable, texture, model, music, movie, or other copyrighted game asset is included.
- Users must dump their own disc and select that image locally.
- The selected ISO remains at its original location. The app persists read-only access through Android's Storage Access Framework; it does not copy the image into the APK or repository.
- `*.iso`, `*.bin`, `*.cue`, `*.chd`, and `*.elf` are ignored by Git. Never commit proprietary game content.

## Current capabilities

- Android 8.0+ (API 26)
- Package ID: `id.dev.rikirg.downhillnative`
- English UI
- System document picker for ISO images
- ISO-9660 validation at the primary volume descriptor (sector 16)
- Minimal native C++20/Android NDK library, connected through JNI

## Planned engineering stages

1. Parse the ISO filesystem without copying the image.
2. Define an asset inspection layer for user-provided data.
3. Add native OpenGL ES 3.0 renderer and a diagnostic scene.
4. Add touch/gamepad input mapping and frame pacing.
5. Implement only independently verified runtime components. This is not a generic PS2 emulator and it cannot convert an ISO into an APK.

## Local build prerequisites

- JDK 21
- Android SDK Platform 35
- Android SDK Build Tools 35.x
- Android NDK 27.x
- CMake 3.22.1 via Android SDK

The repository intentionally does not include an SDK, NDK, Gradle cache, or generated APK.

## Build

Set `ANDROID_HOME` and `ANDROID_NDK_HOME`, then run:

```bash
./gradlew testDebugUnitTest
./gradlew assembleDebug
```

The debug APK is generated at:

```text
app/build/outputs/apk/debug/app-debug.apk
```

## Test focus

`IsoImageValidatorTest` checks valid, invalid, and truncated ISO-9660 primary-volume-descriptor sectors.

## License

The code in this repository is licensed under [MIT](LICENSE).

Game titles and related assets belong to their respective copyright holders. This project is not affiliated with or endorsed by them.
