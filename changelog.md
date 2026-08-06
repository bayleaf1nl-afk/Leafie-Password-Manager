# changelog

all notable changes to leafie are documented here. format loosely follows [keep a changelog](https://keepachangelog.com/), versioned as 0.x until encryption lands and it's usable as a real local password manager.

## [unreleased]

### added

- json-based vault persistence (`PasswordEntry::toJson`/`fromJson`), with import/export/save flows in the main window
- menu bar–driven ui: file/edit/settings/help menus, keyboard shortcuts (search filter, edit password w/ ctrl+e)
- about dialog
- `LoginGate` master password flow: first-launch setup, persisted to `master.txt`, retry loop (3 attempts) on subsequent launches
- window-floating support for tiling WMs via `hyprctl` (hyprland), split into standard/tiling platform backends
- reusable `GenericDialog` (parameterized by title + fields) instead of one dialog class per use case
- clipboard copy support and custom per-row widgets for the password list
- duplicate-entry detection: warns on add if site+username already exists; flags reused passwords across entries with a per-row icon
- email field on password entries, shown as a list column
- `DialogUtils` namespace consolidating `GenericDialog`, `confirmationWindow`, and related ui helpers
- delete confirmation dialog

### changed

- source tree reorganized into `core/`, `ui/`, `vault/`, `platform/`, `scripts/` with updated includes throughout
- `MainWindow` layout split into `createLeftPanel`/`createRightPanel`; menu construction split into per-menu methods
- import now also triggers a save, so imported entries persist
- toolbar buttons replaced by menu actions

### removed

- old toolbar-based import/export buttons (superseded by menu actions)

### fixed

- filter logic now checks against entry data instead of item display text
- string formatting bug in the hyprctl tiling command
- missing namespace qualifier on `confirmationWindow`

### notes

- vault encryption (key derivation from master password + salt, libsodium likely) is the main piece left before calling this 1.0
