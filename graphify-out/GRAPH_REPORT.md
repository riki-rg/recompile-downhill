# Graph Report - recompile-downhill  (2026-11-18)

## Corpus Check
- Corpus is ~8,975 words - fits in a single context window. You may not need a graph.

## Summary
- 172 nodes · 291 edges · 13 communities (7 shown, 6 thin omitted)
- Extraction: 95% EXTRACTED · 5% INFERRED · 0% AMBIGUOUS · INFERRED: 14 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- DiagnosticActivity & OpenGL ES View
- Stages, Changelog & Runtime Plan
- C++20 ISO-9660 Parser Core
- IsoExplorerResult Model & Unit Tests
- MainActivity & SAF URI Launcher
- JNI Native Bridge Entrypoints
- Recursive ISO Tree Statistics
- CMake Native Build & NDK Targets
- ISO Image Format Validator

## God Nodes (most connected - your core abstractions)
1. `NativeSurface` - 15 edges
2. `TreeStats` - 12 edges
3. `readDirectoryEntries()` - 10 edges
4. `walkTree()` - 10 edges
5. `inspectIso()` - 10 edges
6. `DiagnosticActivity` - 10 edges
7. `FileRecord` - 9 edges
8. `MainActivity` - 9 edges
9. `Stage 01: ISO Explorer` - 7 edges
10. `Stage 04: Bounded Recursive ISO Index` - 7 edges

## Surprising Connections (you probably didn't know these)
- `Stage 01: ISO Explorer` --references--> `Android SAF Read-Only URI Flow`  [EXTRACTED]
  docs/stages/01-iso-explorer.md → README.md
- `Java-to-C++20 JNI Bridge (ParcelFileDescriptor)` --calls--> `CMake Shared Library Target: downhillnative`  [INFERRED]
  README.md → app/src/main/cpp/CMakeLists.txt
- `Java-to-C++20 JNI Bridge (ParcelFileDescriptor)` --implements--> `IsoExplorerResult JNI Contract`  [INFERRED]
  README.md → .hermes/plans/2026-11-12_160000-native-iso-runtime.md
- `ISO-9660 Primary Volume Descriptor Validation (Sector 16)` --conceptually_related_to--> `Legal & No-Bundled-Assets Policy`  [INFERRED]
  docs/stages/01-iso-explorer.md → README.md
- `Bounded SYSTEM.CNF Lookup (16 KiB)` --conceptually_related_to--> `Legal & No-Bundled-Assets Policy`  [INFERRED]
  docs/stages/02-downhill-usa-fingerprint.md → README.md

## Import Cycles
- None detected.

## Communities (13 total, 6 thin omitted)

### Community 1 - "Stages, Changelog & Runtime Plan"
Cohesion: 0.12
Nodes (26): Downhill Native Changelog, Release v0.1.0 (Initial Android/NDK Proof-of-Concept), Release v0.1.1 (Adaptive Launcher Icon), Downhill Native USA Runtime Implementation Plan, Stage 05: Playable Vertical Slice Gate, Stage 06: Compatibility & Release Hardening, Legal & No-Bundled-Assets Policy, Downhill Native Project (+18 more)

### Community 2 - "C++20 ISO-9660 Parser Core"
Cohesion: 0.17
Nodes (12): cleanText(), FileRecord, directory, extent, name, size, inspectIso(), readDirectoryEntries() (+4 more)

### Community 3 - "IsoExplorerResult Model & Unit Tests"
Cohesion: 0.15
Nodes (3): IsoExplorerResult, IsoExplorerResultTest, IsoImageValidatorTest

### Community 5 - "JNI Native Bridge Entrypoints"
Cohesion: 0.35
Nodes (6): clearPanel(), Java_id_dev_rikirg_downhillnative_DiagnosticActivity_nativeGlCreate(), Java_id_dev_rikirg_downhillnative_DiagnosticActivity_nativeGlFrame(), Java_id_dev_rikirg_downhillnative_DiagnosticActivity_nativeGlResize(), Java_id_dev_rikirg_downhillnative_MainActivity_nativeExploreIso(), Java_id_dev_rikirg_downhillnative_MainActivity_nativeRuntimeName()

### Community 6 - "Recursive ISO Tree Statistics"
Cohesion: 0.22
Nodes (8): TreeStats, largestFile, largestSize, shownLines, totalBytes, totalDirectories, totalFiles, truncated

### Community 7 - "CMake Native Build & NDK Targets"
Cohesion: 0.25
Nodes (8): Android NDK Log Library (log), CMake Shared Library Target: downhillnative, OpenGL ES 3.0 System Library (GLESv3), IsoExplorerResult JNI Contract, C++20 ISO-9660 Parser Core, Java-to-C++20 JNI Bridge (ParcelFileDescriptor), Android SAF Read-Only URI Flow, DiagnosticActivity & GLSurfaceView (OpenGL ES 3.0)

### Community 8 - "ISO Image Format Validator"
Cohesion: 0.29
Nodes (5): IsoImageValidator, Result, NOT_ISO_9660, UNREADABLE, VALID

## Knowledge Gaps
- **20 isolated node(s):** `name`, `extent`, `directory`, `totalFiles`, `totalDirectories` (+15 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 53 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **6 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `CMake Shared Library Target: downhillnative` connect `CMake Native Build & NDK Targets` to `C++20 ISO-9660 Parser Core`?**
  _High betweenness centrality (0.069) - this node is a cross-community bridge._
- **What connects `name`, `extent`, `directory` to the rest of the system?**
  _20 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `DiagnosticActivity & OpenGL ES View` be split into smaller, more focused modules?**
  _Cohesion score 0.09672830725462304 - nodes in this community are weakly interconnected._
- **Why does `Stage 01: ISO Explorer` connect `Stages, Changelog & Runtime Plan` to `CMake Native Build & NDK Targets`?**
  _High betweenness centrality (0.066) - this node is a cross-community bridge._
- **Should `Stages, Changelog & Runtime Plan` be split into smaller, more focused modules?**
  _Cohesion score 0.12 - nodes in this community are weakly interconnected._