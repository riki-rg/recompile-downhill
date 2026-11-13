# Stage 04 — Bounded Recursive ISO Index

## Purpose

Stage 04 upgrades the ISO Explorer from a flat root listing to a **bounded recursive tree index** of the user-selected image. It gives the runtime the structural awareness needed before any Downhill-specific archive adapter work, while strictly limiting reads to directory metadata.

## What the native runtime now does

1. Parses the Primary Volume Descriptor as before.
2. Reads directory records via a reusable, bounds-checked parser.
3. Recurses into subdirectories with explicit safety caps:
   - maximum depth: **4 levels**;
   - maximum indexed entries: **512**;
   - maximum displayed lines: **96**;
   - maximum single-directory read: **1 MiB**.
4. Aggregates image statistics:
   - total indexed files;
   - total indexed directories;
   - indexed payload bytes;
   - the largest indexed file.
5. Performs the Stage 02 `SYSTEM.CNF` serial verdict on the indexed root records.

No file contents other than the 16 KiB `SYSTEM.CNF` boot record are ever read. The tree shows only names and byte sizes.

## Report format

The on-screen report now contains, in order:

```text
ISO-9660 volume: <name>

Image tree (bounded depth 4):
  [D] DIRECTORY
    FILE  |  123456 B
  ...

Image statistics:
- Files: N
- Directories: N
- Indexed payload: N B
- Largest indexed file: NAME (N B)

Compatibility verdict: ...
```

If any cap is reached, the report states that the tree is truncated instead of failing.

## Scope and limits

- Directory metadata is read only; no data extraction, caching, or export occurs.
- Statistics cover only indexed entries within the caps; they are diagnostic, not a byte-exact image audit.
- This stage does not change the compatibility verdict semantics; `USA_CANDIDATE` remains a candidate status pending a full manifest.

## Verification

```bash
JAVA_HOME="$PWD/.tooling/jdk" PATH="$PWD/.tooling/jdk/bin:$PATH" \
  ./gradlew testDebugUnitTest assembleDebug --no-daemon
```

Passed before the `v0.1.5` release candidate, with native code compiled for `arm64-v8a`, `armeabi-v7a`, `x86`, and `x86_64`.

## Next work

1. Promote the USA candidate gate with a reproducible metadata manifest from a lawful user-local dump (path names, sizes, digests of metadata only).
2. Begin a Downhill-specific archive adapter design once the supported-build manifest exists.