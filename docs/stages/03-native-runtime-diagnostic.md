# Stage 03 — Native Runtime & Input Diagnostic

## Purpose

Stage 03 verifies Android-native rendering, lifecycle handling, and player-input plumbing required before a Downhill Domination USA compatibility runtime can render game-specific data. It uses only original diagnostic visuals and contains no game data.

## How to use

1. Install the APK on an Android 8.0+ device.
2. Open **Downhill Native**.
3. Tap **Open native renderer diagnostic**.
4. The screen switches to landscape and shows a native OpenGL ES 3.0 diagnostic scene.
5. Test controls:
   - left half: left/right steering;
   - upper-right: throttle;
   - lower-right: brake;
   - gamepad left stick or D-pad: steering;
   - right trigger: throttle;
   - left trigger: brake;
   - gamepad A/R1: throttle; B/L1: brake.
6. Press Back, background/foreground the app, then reopen the diagnostic to validate surface lifecycle behavior on the target device.

## Implementation

- `DiagnosticActivity` is landscape-only and owns a `GLSurfaceView` configured for OpenGL ES 3.0.
- EGL context preservation is requested across pause/resume where the device supports it.
- C++20 NDK functions create, resize, and draw each frame via JNI.
- The original scene uses scissored color panels to represent sky, terrain, movable trail, and input zones. It intentionally does not use a game model, texture, HUD, or extracted asset.
- A Java overlay reports measured presentation-loop FPS and normalized steer/throttle/brake input values.
- Gamepad support follows Android `SOURCE_JOYSTICK`, `SOURCE_GAMEPAD`, and `SOURCE_DPAD` events with a 0.15 stick dead zone.

## Scope and limits

This stage proves a rendering and input foundation only. It does **not** provide:

- a Downhill Domination USA fingerprint or game-compatibility verdict;
- PS2 emulation or execution of MIPS/VU binaries;
- player physics, collision, camera, audio, tracks, gameplay, save data, or menus;
- real-device certification for every Android GPU/driver.

A successful diagnostic scene means the Android rendering/input path worked on that specific device; it is not proof that the target game is playable.

## Verification

From repository root:

```bash
JAVA_HOME="$PWD/.tooling/jdk" PATH="$PWD/.tooling/jdk/bin:$PATH" \
  ./gradlew testDebugUnitTest assembleDebug --no-daemon
```

Stage 03 source was verified with this command before the `v0.1.3` release candidate. The native library cross-compiles for `arm64-v8a`, `armeabi-v7a`, `x86`, and `x86_64`.

## Next work

1. Stage 02 compatibility gate: define and test a metadata-only fingerprint from a lawful user-local USA dump.
2. Convert diagnostic input values into a deterministic bike-physics prototype.
3. Add a legal, original vertical-slice test course before any Downhill-specific asset adapter work.

No proprietary disc data may be added to source control, releases, tests, screenshots, or documentation.