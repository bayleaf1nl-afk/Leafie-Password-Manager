# Feature request: let GenericDialog host arbitrary QWidgets

Status: proposal / request for comment. No implementation is included or prescribed here.

## Problem

`DialogUtils::GenericDialog` can only build one kind of UI: a vertical list of
label + `QLineEdit` pairs, plus two hard-coded extras toggled by flags
(`hasGenerateButton` on a field, `showFileSelectionButton` on the dialog).
Anything outside that shape — a checkbox, a combo box, a spin box, a text area,
a preview pane, a pre-built composite widget — cannot be expressed, so callers
either bend their data into text fields or write a bespoke `QDialog`.

Consequences visible today:

- Every new control type means another boolean parameter on the constructor and
  another branch in the construction loop; the flag set only grows.
- `browseForFile()` is wired to "the last line edit", an implicit coupling that
  breaks as soon as the layout is not a flat list of line edits.
- Results are read back positionally via `inputText(int)`, so callers depend on
  field ordering and on every value being a `QString`.

## What is being asked for

Rework `GenericDialog` so that a caller can hand it any `QWidget` (owned by the
caller or created inline) and have it laid out, labelled and included in the
dialog's accept/reject lifecycle, while keeping the existing "just give me some
text fields" usage a one-liner.

Requirements the design should satisfy:

1. **Any widget is accepted.** Nothing in the dialog should special-case
   `QLineEdit`; a widget the dialog has never heard of must work.
2. **A simple helper keeps the common case short.** Constructing a labelled
   password field, a generated-password field or a file-picker row should stay
   as terse as it is now — the helper produces the widget, the dialog just
   hosts it. Existing call sites in `MainWindow` and `LoginGate` should be
   expressible without becoming more verbose.
3. **Type-safe, non-positional readback.** Callers should retrieve values
   without relying on insertion order or on everything being a string.
4. **Ownership and lifetime are unambiguous.** It must be clear whether the
   dialog reparents/owns the widgets handed to it, and widgets must survive
   being read after the dialog closes if the API implies that.
5. **Existing behaviour is preserved.** Password generation, strength feedback
   and file browsing remain available, but as composable pieces rather than
   constructor flags.
6. **Secret handling does not regress.** Password input paths must keep the
   memory-hygiene guarantees the vault code relies on; a generic widget API
   must not quietly turn secrets into long-lived `QString` copies.

## Out of scope

- Restyling dialogs or changing their visual layout beyond what the refactor
  requires.
- Adding new dialog features unrelated to widget hosting.

## Open questions for discussion

- Should the dialog own a builder/registry of widget factories, or should
  callers always pass fully-constructed widgets?
- How should validation be expressed — per widget, or as a dialog-level
  predicate gating the OK button?
- Should `inputText(int)` remain as a deprecated shim during migration, or
  should all call sites be converted in one change?
- Does the same abstraction want to cover `DialogUtils`' existing message/
  confirmation helpers, or should those stay separate?

## Affected code

- `src/ui/GenericDialog.h`, `src/ui/GenericDialog.cpp`
- Call sites: `src/ui/MainWindow.cpp` (new password, import/export vault),
  `src/core/LoginGate.cpp` (login, first-run master password)
