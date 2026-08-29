# Feature request: let GenericDialog host arbitrary QWidgets

Status: proposal / request for comment. No implementation is included or prescribed here.

## Problem

`DialogUtils::GenericDialog` can only build one kind of UI: a vertical list of
label + `QLineEdit` pairs, plus two hard-coded extras toggled by flags
(`hasGenerateButton` on a field, `showFileSelectionButton` on the dialog).
Anything outside that shape — a checkbox, a combo box, a spin box, a text area,
a preview pane, a pre-built composite widget — cannot be expressed.

So the codebase has two classes of dialog and neither is happy:

- Dialogs that fit the text-field mould go through `GenericDialog` and pay for
  it with growing constructor flags, an implicit "browse targets the last line
  edit" coupling, and positional string readback via `inputText(int)`.
- Dialogs that don't fit are written by hand and duplicate the same
  boilerplate: `QVBoxLayout`, widget creation, an OK/Cancel `QDialogButtonBox`,
  the accept/reject connects, `exec()`. `PasswordGenerationWindow` is the clear
  example. `confirmationWindow` was going to be one of these too and only ended
  up a `QMessageBox` because hand-rolling it wasn't worth the boilerplate — the
  boilerplate is what pushed it out of the family.

## What is being asked for

Make `GenericDialog` the common substrate for **every** window the app spawns,
so that a caller can hand it any `QWidget` and get it laid out, included in the
accept/reject lifecycle, and read back afterwards — without the dialog knowing
anything about the widget's type.

### Preferred shape

Discussed on this PR; recorded here as the direction to design against.

`GenericDialog` owns only the things every dialog shares: the layout, the
button box, the accept/reject wiring, `exec()`, window title, and whatever
platform/floating behaviour the app needs. It takes a list of widgets to lay
out and nothing else — no property descriptions, no behaviour flags.

Each concrete window is then a small function or subclass that:

- creates its own widgets and keeps them as typed members,
- does its own `connect()` calls and property setup in its constructor,
- hands the widgets to `GenericDialog` for layout,
- reads its own typed members back after `exec()` returns.

The point is that type information never leaves the concrete dialog, so no
casting, no `QVariant`, no positional lookup.

### Why not a property/behaviour list

The alternative considered was entries of `{widget, {properties…}}` —
a `WidgetList` carrying sizes, names and connections per widget, iterated by
the constructor. Rejected because:

- `connect()` cannot be stored as data: it needs the concrete widget type and a
  live receiver, and can only run after the widget exists.
- A heterogeneous property bag means `QVariant` and erases the widget's type,
  reintroducing casting on readback — the same problem `inputText(int)` has.
- The constructor would need to grow a branch per property kind, which is the
  flag problem again in a new costume.

`QObject::setProperty` remains available for the purely declarative bits
(`objectName`, `placeholderText`, geometry) inside a concrete dialog's own
constructor, where a typo is a local bug rather than a framework feature.

## Requirements

1. **Any widget is accepted.** Nothing in the dialog special-cases `QLineEdit`;
   a widget the dialog has never heard of must work.
2. **Every dialog in the app can be expressed through it**, including
   confirmation/message windows, the password generator, the login and
   first-run master password prompts, the new-entry form, and import/export
   file pickers. If a window still needs to be hand-rolled, the abstraction is
   not done.
3. **Confirmation windows regain the behaviour `QMessageBox` gave for free.**
   Since `confirmationWindow` is in scope, the design must account for standard
   button sets and their roles, default and escape buttons, platform-native
   button ordering, keyboard handling, and icon/spacing conventions — either by
   providing them in the base or by a deliberate, documented decision to drop
   them. Migrating it must not make confirmations feel worse than the current
   `QMessageBox`.
4. **The common cases stay short.** A yes/no confirmation and a labelled
   password prompt should each remain roughly a one-liner at the call site;
   small helpers may exist for these, built on the same substrate rather than
   beside it.
5. **Type-safe, non-positional readback.** Callers retrieve values without
   relying on insertion order or on everything being a `QString`.
6. **Ownership and lifetime are unambiguous.** If the caller creates the
   widgets and keeps typed pointers, the dialog reparents for layout only and
   must not become their sole owner; widgets must stay readable after `exec()`
   returns. Whatever the rule is, it is written down.
7. **Existing behaviour is preserved.** Password generation, strength feedback
   and file browsing remain available as composable pieces owned by the dialogs
   that want them, not as constructor flags.
8. **Secret handling does not regress.** Password input paths keep the
   memory-hygiene guarantees the vault code relies on; a generic widget API
   must not quietly turn secrets into extra long-lived `QString` copies.

## Out of scope

- Restyling dialogs or changing visual layout beyond what the refactor
  requires.
- New dialog features unrelated to widget hosting.

## Open questions for discussion

- Should callers always construct widgets themselves, or should the dialog also
  accept factories (`parent -> QWidget*`) so it controls parenting?
- Should concrete dialogs subclass `GenericDialog` or compose it? Composition
  keeps the base sealed; subclassing gives easier access to accept/reject.
- How is validation expressed — per widget, or a dialog-level predicate gating
  the OK button?
- Does the confirmation case get a `QMessageBox`-equivalent helper on top of
  the base, or does the base itself learn standard button sets?
- Should `inputText(int)` survive as a deprecated shim during migration, or
  should all call sites convert in one change?

## Affected code

- `src/ui/GenericDialog.h`, `src/ui/GenericDialog.cpp`
- `src/ui/DialogUtils.h`, `src/ui/DialogUtils.cpp` (`confirmationWindow`,
  `PasswordGenerationWindow`)
- Call sites: `src/ui/MainWindow.cpp` (new password, import/export vault,
  confirmations), `src/core/LoginGate.cpp` (login, first-run master password)
