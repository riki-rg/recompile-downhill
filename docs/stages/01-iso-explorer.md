# Stage 01 — ISO Explorer (Downhill Domination USA Foundation)

## Purpose

Stage 01 replaces the earlier ISO signature-only check with a bounded, native ISO-9660 explorer. It is the first required gate for the **Downhill Domination USA / NTSC-U** compatibility path.

It does **not** claim that an ISO is Downhill Domination yet and does **not** run PS2 code.

## User workflow

1. Install the debug APK.
2. Tap **Choose ISO image**.
3. Select an ISO personally dumped from a disc the user owns.
4. Android grants persistent, read-only SAF access.
5. The native C++ runtime reads the ISO Primary Volume Descriptor and root directory through the selected file descriptor.
6. The app displays volume ID and up to 32 root-entry names and sizes.

The ISO stays where the user selected it. The app does not copy it into its storage, the APK, or the repository.

## Implementation

- Android Java opens a read-only `ParcelFileDescriptor` from the Storage Access Framework.
- JNI passes the descriptor integer to the C++20 NDK runtime.
- Native code uses positional reads (`pread`) and never depends on mutable stream position.
- The parser validates the ISO-9660 PVD at sector 16, root-directory record bounds, directory record lengths, and filename bounds.
- Directory enumeration is capped at 32 visible entries and root-directory data is capped at 8 MiB.
- Parser output is metadata only: volume label, visible file names, and byte sizes.

## Current limits

- The current reader supports ISO-9660 root-directory data only.
- It does not yet support Joliet, UDF, multi-extent files, subdirectories, compressed archives, or file-content extraction.
- It cannot determine Downhill Domination USA compatibility yet. That is Stage 02, after a documented USA disc-layout fingerprint is defined and tested.
- It is not an emulator and never executes PS2 MIPS/VU binaries.

## Verification

From the repository root:

```bash
JAVA_HOME="$PWD/.tooling/jdk" PATH="$PWD/.tooling/jdk/bin:$PATH" \
  ./gradlew testDebugUnitTest assembleDebug --no-daemon
```

Stage 01 verification completed successfully on 2026-11-12:

- JUnit unit tests passed, including the safe explorer report contract.
- JNI/C++ code cross-compiled for `arm64-v8a`, `armeabi-v7a`, `x86`, and `x86_64`.
- Debug APK was assembled.

## Next: Stage 02

Stage 02 will add a strict Downhill Domination USA / NTSC-U compatibility gate. It will compare only documented, non-bundled structural metadata from a user-local ISO and report one of: supported USA build, unsupported revision, or non-target image.

No game files, BIOS, executable, or extracted assets may be committed or redistributed at any stage.
