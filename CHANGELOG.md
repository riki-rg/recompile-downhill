# Changelog

All notable project changes are documented here. This project follows [Semantic Versioning](https://semver.org/) for release labels while it remains in pre-1.0 development.

## [0.1.5] — 2026-11-13

### Added

- Bounded recursive ISO-9660 tree index: depth 4, 512 indexed entries, 96 displayed lines.
- Reusable bounds-checked directory-record parser in the native runtime.
- Image statistics: indexed files, directories, payload bytes, and largest indexed file.
- Stage 04 operational documentation.

### Changed

- Application version raised to `0.1.5` (`versionCode 6`).
- ISO Explorer report now shows a recursive tree and statistics before the compatibility verdict.

### Not included

- No archive extraction, no new verdict semantics, no PS2 emulation, and no proprietary game assets.

## [0.1.4] — 2026-11-13

### Added

- Stage 02 conservative Downhill Domination USA fingerprint gate.
- Native `SYSTEM.CNF` lookup bounded to 16 KiB, read-only, metadata only.
- Compatibility verdicts: `UNSUPPORTED_LAYOUT`, `NON_TARGET_PS2_DISC`, and `USA_CANDIDATE` for the documented NTSC-U serial `SLUS_206.47`.
- Stage 02 fingerprint documentation, including the evidence contract and reasons a candidate is not yet “supported”.

### Changed

- Application version raised to `0.1.4` (`versionCode 5`).
- ISO Explorer report now ends with a compatibility verdict.

### Not included

- No full supported-build manifest, ISO extraction, PS2 emulation, or proprietary game assets.

## [0.1.3] — 2026-11-12

### Added

- Stage 03 native OpenGL ES 3.0 diagnostic activity in landscape orientation.
- Original schematic downhill test scene rendered by the C++20/NDK runtime; it uses no game data.
- Real-time FPS and normalized input telemetry overlay.
- Diagnostic touch controls for steering, throttle, and brake.
- Android gamepad input for joystick/D-pad steering, triggers, and common face/shoulder buttons.
- Stage 03 operational documentation.

### Changed

- Application version raised to `0.1.3` (`versionCode 4`).
- README now documents the native rendering and input diagnostic milestone.

### Not included

- No playable game implementation, PS2 emulation, USA fingerprint decision, ISO/BIOS, or proprietary game assets.

## [0.1.2] — 2026-11-12

### Added

- Stage 01 native ISO Explorer foundation for the Downhill Domination USA compatibility path.
- Read-only Android Storage Access Framework to JNI file-descriptor flow.
- Bounded ISO-9660 Primary Volume Descriptor and root-directory inspection in C++20.
- Metadata report with ISO volume identifier and up to 32 root entries.
- Safe handling for unreadable or malformed ISO directory metadata.
- `IsoExplorerResultTest` coverage for success, truncation, and failure display states.
- Stage 01 technical documentation and a complete project roadmap.

### Changed

- Application version raised to `0.1.2` (`versionCode 3`).
- README expanded with project status, architecture, requirements, build instructions, data policy, scope, and roadmap.

### Not included

- No game-playable implementation, PS2 emulation, game assets, ISO, BIOS, or executable content.
- No Downhill Domination USA fingerprint verdict yet; this is planned for Stage 02.

## [0.1.1] — 2026-11-12

### Added

- Original black-and-gold downhill-bike adaptive launcher icon.

## [0.1.0] — 2026-11-12

### Added

- Initial Android/NDK proof-of-concept.
- User-owned ISO selection and ISO-9660 primary-volume-descriptor validation.
- C++20 JNI runtime baseline and MIT-licensed repository foundation.

[0.1.5]: https://github.com/riki-rg/recompile-downhill/releases/tag/v0.1.5
[0.1.4]: https://github.com/riki-rg/recompile-downhill/releases/tag/v0.1.4
[0.1.3]: https://github.com/riki-rg/recompile-downhill/releases/tag/v0.1.3
[0.1.2]: https://github.com/riki-rg/recompile-downhill/releases/tag/v0.1.2
[0.1.1]: https://github.com/riki-rg/recompile-downhill/releases/tag/v0.1.1
[0.1.0]: https://github.com/riki-rg/recompile-downhill/releases/tag/v0.1.0
