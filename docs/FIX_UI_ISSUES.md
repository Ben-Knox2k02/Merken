# Fix UI issues

Replay plan for branch `fix-ui-issues`, cut from `main` at `1fea29d` ("Enable rich text in prompt control").

Check a box only after that step builds and runs on Windows and someone has confirmed it. One step at a time. Do not push until the team asks.

You run `build.bat` and launch `Merken.exe`. The app stays closed during the link, because a running `Merken.exe` locks the output.

## How a commit is applied

`origin/UIWork` is the Mac UI history to replay (`olaboutcode`, 2026-09-14 through 2026-10-07). On 2026-10-09 `main` deleted `UI/` and the build scripts, then put a new panel UI in their place, so many of these patches will not apply cleanly. Port the behavior of the commit onto the current files, then keep the Windows build green.

Local `UIWork` (`3cfd356`) is an earlier Windows attempt. Use it as reference when a replayed commit fails to build or paint. Do not replay that branch as its own history.

`origin/Berken` has no common ancestor with `main` (root `558986b`). Its files live in `include/` and `src/`. Port each commit's behavior. If that behavior is already on the branch, check the box after confirming there is nothing left to bring across.

These `main` behaviors stay in place through every step:

- Find/replace dialog
- Undo/redo
- Rich text on the front, back, and prompt controls
- Deck deletion with confirmation
- A single Windows manifest that links (see [SETUP.md](SETUP.md))

Windows fixes to apply at the first commit that needs them, then keep:

- Common Controls 6 manifest, so `GetWindowSubclass` resolves at startup
- PerMonitorV2 DPI awareness
- `FromDIP` on sizes that were written as raw pixels (the deck column is 213 DIP)
- Custom-painted panels clear `WS_CLIPCHILDREN`, or labels leave black rectangles
- Transparent widgets use two-phase creation (`SetBackgroundStyle` before `Create`)
- Link line uses the wxWidgets 3.3 `33u` libraries, including TIFF and WebP

`build.bat` links `-lcomctl32_subclass`. Commit `927ff37` removed `libcomctl32_subclass.a` from the repo. [SETUP.md](SETUP.md) documents regenerating that import library and the linker error `.rsrc merge failure: multiple non-default manifests`.

## 1. Branch

- [x] Confirm local branch `fix-ui-issues` exists at `1fea29d` and matches `main`, with a clean tree

## 2. Baseline the current UI

- [x] Run `build.bat` on the unchanged branch and record the result. Recorded: `build.bat` selected `C:\mingw64`, then `windres` and `g++` were not found. `g++` 16.1.0 is at `C:\mingw-w64\mingw64\bin`.
- [x] Fix link, manifest, and startup until the current UI builds
- [x] Launch `Merken.exe` and confirm the current UI opens

## 3. Replay origin/UIWork

Oldest first. The three merge commits at the end are listed so the walk is complete. This branch already contains that `main` plus the later commits, so they should be checked once we confirm they add nothing new.

- [x] `4ff5eb6` Wire remaining UI screens through use cases
- [x] `0d3da65` Replace deck and card tables with redesigned lists
- [x] `506dcde` Add separator between deck cards
- [x] `8ab737f` Tighten deck cards and round the deck list panel
- [x] `824158d` Replace the card table with FlashCard list items
- [x] `6263e6a` Add deck and flash card delete actions
- [x] `1d9e325` Add a dashboard deck navbar and two-column cards
- [x] `ddd250c` Add theme, split layout, and per-card flash card actions
- [x] `c7fd4c4` Show deck card counts and restyle the deck list
- [x] `5fe9dc4` Wrap flash card text and pack list columns
- [x] `99a038e` Add study header buttons and fit flash card wells
- [x] `83700e4` Disable study buttons when a deck has no cards
- [x] `9460847` Follow system light and dark appearance
- [x] `d080b20` Restructure app menus and add menu icons
- [ ] `f42dc7b` Open the main window at 1366 by 768 DIP
- [ ] `f4cd8fb` Add a calendar icon to the View Calendar menu
- [ ] `63bc618` Theme dialogs and restyle the study card
- [ ] `cf58b3f` Place the study side label inside the card
- [ ] `e9f2c21` Extend the window under the macOS title bar
- [ ] `beca47c` Open the study back card with a page turn
- [ ] `170fc08` Lock the window during study and keep card text regular
- [ ] `f64ad46` Give the study card more padding and smaller text
- [ ] `b346472` Match the study deck title to the card list
- [ ] `5bfcd94` Slide the study answer open beside the front card
- [ ] `750006e` Show a message when a deck has no cards
- [ ] `a18a34a` Show a retention donut when a study session ends
- [ ] `823795c` Remove unnecessary frame size constants
- [ ] `3c4e3b4` Center short study card text
- [ ] `ed1a8e3` Show the profile at the top of the deck list
- [ ] `e2813c9` Show onboarding before the main window
- [ ] `303345a` Add the first-run guide
- [ ] `7f04ee6` Enable only the next guide button
- [ ] `0086653` Keep a single flash card at half width
- [ ] `d59e675` Keep the deck name gap when a deck has no description
- [ ] `f0ff5e5` Center icon buttons
- [ ] `5d2dd57` Disable deck and study actions when nothing is available
- [ ] `b2a238e` Give the deck list buttons equal widths
- [ ] `9dc273e` Small code cleanup
- [ ] `0c5352f` Add app.manifest file for application assembly
- [ ] `b592d3c` Add manifest.rc for application assets
- [ ] `da1dca2` Merge branch `BackEnd` into `UIWork` (confirm nothing new to take)
- [ ] `01178f3` Merge `origin/main` into `UIWork` (confirm nothing new to take)
- [ ] `20268c1` Merge `origin/main` into `UIWork` (confirm nothing new to take)
- [ ] Confirm find/replace, undo/redo, rich text, and deck-delete confirmation still work

## 4. Port origin/Berken

Oldest first. Same rule: one commit, then a Windows build and run.

- [ ] `558986b` Initial UI-only commit
- [ ] `43b9741` Delete `Berken.exe`
- [ ] `a20774b` Add comment indicating file is generated by Copilot
- [ ] `333fd78` Delete `obj` directory
- [ ] `a862106` Add generated-by-Copilot comment to build script
- [ ] `a40d175` Add new event handlers and update menu structure
- [ ] `10dba69` Refactor menu and event handling in MainFrame
- [ ] `0228bc6` Add StudyPanel class definition in `study_panel.h`
- [ ] `209652b` Add `data_structures.h` with temporary Card and Deck definitions
- [ ] `e48ce0d` Add wxPanel members for styling and layout
- [ ] `655d17c` Implement StudyPanel with navigation and event handling
- [ ] `b647c64` Enhance menu items with keyboard shortcuts
- [ ] `03a2e6a` Remove custom event IDs and add keyboard shortcuts
- [ ] `556e3b6` Remove Card struct from `card_list_panel.h`
- [ ] `1af8e5d` Update background color of DeckPanel to white
- [ ] `059ffe1` Refactor MainFrame class and update shortcuts array
- [ ] `3f5adcf` Add CalendarPanel class header file
- [ ] `8d78db6` Add header file for AIPanel class
- [ ] `55e5f60` Add EventDialog with fields for event creation
- [ ] `8e339a0` Implement CalendarPanel with event handling
- [ ] `66c79b7` Implement AIPanel with layout and paint handling
- [ ] `084804b` Add EventDialog class to `dialog.h`
- [ ] `c03c64a` Add function keys to data structure enum
- [ ] `69a6723` Remove Deck struct from `deck_panel.h`
- [ ] `817aba8` Enhance MainFrame with additional menus and shortcuts
- [ ] `2cb6d31` Set background style and color for StudyPanel
- [ ] `8d09aac` Update to faster and more robust wxWidgets build script
- [ ] `d3fe228` Remove redundant function key IDs
- [ ] `f543f86` Add new controls to AIPanel class
- [ ] `5521426` Define Event structure in `data_structures.h`
- [ ] `36d1564` Add `SetSidebarPanel` method and update panel references
- [ ] `adc4845` Refactor AI panel layout and add input controls
- [ ] `7d70b2b` Update header font size and card list column widths
- [ ] `0cfdecc` Update header font to a specific wxFont configuration
- [ ] `e18a8fa` Replace `activePanel` with `contentPanel` in MainFrame
- [ ] `ae162af` Adjust padding in study panel layout
- [ ] Confirm find/replace, undo/redo, rich text, deck-delete confirmation, and the replayed UIWork behavior still work
