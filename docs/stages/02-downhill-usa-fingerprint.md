# Stage 02 — Downhill Domination USA Fingerprint

## Goal

Stage 02 adds a **metadata-only, conservative** compatibility report for the project target: **Downhill Domination USA / NTSC-U**. It does not extract or distribute game content, and it does not claim playability.

## Evidence contract

The native reader evaluates only ISO data supplied locally by the user:

1. ISO-9660 Primary Volume Descriptor and root-directory bounds are valid.
2. A root `SYSTEM.CNF` record is present and is no larger than the reader's 16 KiB safety limit.
3. The file's textual boot record contains the documented NTSC-U executable serial `SLUS_206.47`.

The app reports one of these conservative statuses:

| Status | Meaning |
|---|---|
| `NOT_AN_ISO` | The selected file has no readable ISO-9660 Primary Volume Descriptor. |
| `UNSUPPORTED_LAYOUT` | The ISO is readable, but the required root `SYSTEM.CNF` metadata cannot be safely read. |
| `NON_TARGET_PS2_DISC` | `SYSTEM.CNF` was readable but does not advertise the target serial. |
| `USA_CANDIDATE_NEEDS_LAYOUT_VERIFICATION` | The user-local ISO advertises `SLUS_206.47`, the known NTSC-U executable serial. This is a candidate, **not** a game-playability approval. |

## Why this is not “supported” yet

A PS2 boot serial identifies a disc build family, but it does not verify all archive layouts, revisions, integrity, or behavior. Before the project may show **Supported USA build**, we need a reproducible, non-proprietary manifest from a lawful local dump: selected path names, byte lengths, and cryptographic digests of metadata—not copied asset data. That evidence is intentionally absent from this release.

## Privacy and asset policy

- The app reads at most 16 KiB from `SYSTEM.CNF` and only displays its classification/serial.
- It does not export, cache, upload, hash, extract, or show proprietary file contents.
- ISO, BIOS, executable, archive, texture, audio, and model files remain excluded from the repository.

## Validation procedure

1. Select a user-owned ISO through the Android document picker.
2. Confirm the ISO Explorer report is readable.
3. Review the `Compatibility verdict` section.
4. A `USA candidate` result proves only that `SYSTEM.CNF` contains the expected serial; retain the ISO locally for later device tests.

## Next gate

The next fingerprint improvement must be based on independently collected, documented metadata from a lawful USA disc image. Until then, a candidate verdict cannot be promoted to “supported” and must not unlock proprietary asset extraction.