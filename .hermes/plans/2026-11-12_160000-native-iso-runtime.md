# Downhill Native USA Runtime Implementation Plan

> **For Hermes:** Use subagent-driven-development skill to implement this plan task-by-task.

**Goal:** Build and document a legal, native Android foundation that reads a user-selected USA (NTSC-U) game ISO without bundling or copying game content, then progressively reaches a tested playable vertical slice.

**Architecture:** Java Android code owns lifecycle, Storage Access Framework permissions, and UI. C++20 via Android NDK owns ISO parsing, compatibility detection, renderer, input, and runtime subsystems. The ISO remains user-owned and read-only; only small derived metadata/cache files may be stored in app-private storage.

**Tech Stack:** Android API 26+, Java, C++20, JNI, CMake, OpenGL ES 3.0, JUnit, Android NDK 27.2.

---

## Scope and non-negotiable constraints

- Target first supported retail layout: **Downhill Domination USA / NTSC-U**.
- No ISO, BIOS, executable, texture, model, music, video, extracted archive, or copyrighted game data may enter Git, APK, release assets, or documentation examples.
- The project may only inspect a user-selected local ISO through Android SAF.
- This is **native reimplementation**, not an emulator. Executing PS2 MIPS/VU binaries is out of scope.
- Each stage must add `docs/stages/NN-*.md`, tests, an APK build check, commit, and push.
- “Ready to use” is stage-specific: Stage 1 is ready as an ISO Explorer; a playable game requires later verified decoding and runtime milestones plus real-device testing with a user-owned USA ISO.

## Milestone map

| Stage | User-visible deliverable | Completion evidence |
|---|---|---|
| 01 | ISO Explorer: list ISO-9660 root entries | deterministic native parser tests + debug APK |
| 02 | USA compatibility gate and report | fixture-based manifest/fingerprint tests |
| 03 | Android graphics/input diagnostic runtime | device/emulator lifecycle + renderer tests |
| 04 | Asset inspection adapters | documented format findings and safe metadata index |
| 05 | Playable vertical slice | one course segment, input, camera, collision, checkpoint |
| 06 | Compatibility/release hardening | device matrix, quality profiles, release procedure |

---

## Stage 01 — Native ISO Explorer

### Task 1: Define JNI result contract

**Objective:** Define a stable Java-native boundary that returns safe display metadata, never proprietary bytes.

**Files:**
- Create: `app/src/main/java/id/dev/rikirg/downhillnative/IsoExplorerResult.java`
- Modify: `app/src/main/java/id/dev/rikirg/downhillnative/MainActivity.java`
- Test: `app/src/test/java/id/dev/rikirg/downhillnative/IsoExplorerResultTest.java`

**Steps:**
1. Write JUnit tests for a result object with ISO label, volume identifier, root-entry count, and user-safe error message.
2. Run the test and observe RED.
3. Implement only immutable result handling and safe rendering helpers.
4. Run targeted and full unit tests.
5. Commit: `feat: define ISO explorer result contract`.

### Task 2: Build a testable ISO-9660 parser core

**Objective:** Parse the PVD and root-directory record from an abstract random-access byte source.

**Files:**
- Create: `app/src/main/cpp/iso9660_reader.h`
- Create: `app/src/main/cpp/iso9660_reader.cpp`
- Create: `app/src/test/cpp/iso9660_reader_test.cpp`
- Modify: `app/src/main/cpp/CMakeLists.txt`

**Steps:**
1. Write host-native tests using synthetic 2048-byte sectors for valid PVD/root record, invalid PVD, truncated read, malformed directory record, and record padding.
2. Run CMake/CTest and observe RED.
3. Implement little-endian field reading with bounds checks, ASCII-only safe display labels, and root directory enumeration with an explicit entry cap.
4. Run CTest to GREEN; run Android build to ensure cross-compile compatibility.
5. Commit: `feat: parse ISO-9660 root directory safely`.

### Task 3: Add Android file-descriptor bridge

**Objective:** Pass a read-only detached file descriptor from Android SAF to native C++ and return a bounded text/structured report.

**Files:**
- Modify: `app/src/main/java/id/dev/rikirg/downhillnative/MainActivity.java`
- Modify: `app/src/main/cpp/native_runtime.cpp`
- Test: `app/src/test/java/id/dev/rikirg/downhillnative/IsoExplorerResultTest.java`

**Steps:**
1. Write a Java test around formatting parser status for success/failure paths.
2. Add `ParcelFileDescriptor` handling; ensure every descriptor is closed in Java and native code duplicates only when needed.
3. In C++, use `pread`, not shared stream position; cap directory entries and report only filenames/sizes.
4. Confirm invalid ISO, inaccessible URI, and malformed directory data produce non-crashing UI errors.
5. Commit: `feat: inspect selected ISO through native file descriptor`.

### Task 4: Present results in the UI

**Objective:** Show volume ID, a bounded root-entry list, and a plain-language status in the existing activity.

**Files:**
- Modify: `app/src/main/res/layout/activity_main.xml`
- Modify: `app/src/main/res/values/strings.xml`
- Modify: `app/src/main/java/id/dev/rikirg/downhillnative/MainActivity.java`

**Steps:**
1. Write UI-model unit test for success/empty/error presentation.
2. Add `ScrollView`-safe explorer report view; do not display raw executable or proprietary asset contents.
3. Validate landscape and small-screen overflow through Android resource/layout checks.
4. Commit: `feat: display ISO explorer report`.

### Task 5: Stage documentation and verification

**Objective:** Document behavior, limitations, data policy, and reproducible build/test commands.

**Files:**
- Create: `docs/stages/01-iso-explorer.md`
- Modify: `README.md`

**Steps:**
1. Document input/output contract, no-copy guarantee, malformed-image behavior, entry cap, and known ISO-9660 limitations (no UDF/Joliet/multi-extent support until intentionally added).
2. Run `testDebugUnitTest`, native parser tests, and `assembleDebug`.
3. Record APK checksum and local verification results in stage doc (not proprietary content).
4. Commit and push: `docs: document stage 01 ISO explorer`.

---

## Stage 02 — USA compatibility gate

1. Define public, non-copyrighted metadata requirements (expected disc label/structural paths and safe file-size/hash metadata only).
2. Require an explicit supported-version match before game-specific inspection.
3. Show `Supported USA build`, `Unsupported revision`, or `Not target game` without extracting game files.
4. Document what evidence is required to add a revision and add synthetic tests.

**Documentation:** `docs/stages/02-usa-compatibility.md`.

## Stage 03 — Native Android runtime diagnostic

1. Add an OpenGL ES 3.0 surface and original diagnostic scene only.
2. Add pause/resume/context-loss tests and touch/gamepad event mapping.
3. Add frame-time telemetry and a safe quality-profile selection.
4. Verify on ARM64 physical device first, then emulator/x86_64.

**Documentation:** `docs/stages/03-native-runtime.md`.

## Stage 04 — Asset inspection research gate

This stage cannot be implemented responsibly until the exact USA ISO file layout and formats are independently characterized from a user-owned dump. Work must be format-by-format, test-first, and must never commit extracted material. Every new parser requires a documented format note, synthetic fixture, and a legal review of whether the implementation reproduces protected expressive content.

**Documentation:** `docs/stages/04-asset-inspection.md`.

## Stage 05 — Vertical-slice gate

Only start after Stage 04 has parsers sufficient for one map subset, one controllable rider, collision data, and required animation/audio metadata. The acceptance target is one bounded course segment with original runtime code, touch/gamepad mapping, pause, checkpoint, and deterministic diagnostics—not full game parity.

**Documentation:** `docs/stages/05-vertical-slice.md`.

## Tests and release gates

At every stage:

```bash
./gradlew testDebugUnitTest
./gradlew assembleDebug
```

Native parser components must additionally run host CTest or equivalent deterministic tests. A stage may only be marked complete when tests pass, APK is produced, source is committed, source is pushed, and the corresponding documentation is committed.

## Risks and stop conditions

- Full native reimplementation cannot be honestly scheduled to “finish” without user-owned test media, research, reverse-engineering work, and device validation.
- If required asset formats are encrypted/obfuscated or require executing PS2 code, stop and reassess rather than silently becoming an emulator.
- Compatibility claims require a tested device matrix; no “all Android devices” claim without that evidence.
- Any use of protected game assets in a release/repo is prohibited; the app must rely on user-local media.

## Definition of ready

- **Stage 01 ready:** stable ISO Explorer APK for arbitrary ISO-9660 images.
- **Game-ready vertical slice:** only after Stage 05 acceptance tests are demonstrated against a user-owned USA ISO and actual Android hardware.
- **Full port ready:** requires separate scope, test matrix, and feature parity checklist after the vertical slice; do not claim it before measured evidence.
