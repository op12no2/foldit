# foldit

foldit is a folding editor inspired by the editor that was integrated into the Inmos Transputer Development System (TDS). You don't open and close folds in place: you *enter* a fold, and the screen shows just the lines inside it. It can also enter files as if they were folds(*), which makes navigating whole directory structures easy.

This is a terminal version for Linux/WSL. The original 1995 Windows MFC version is archived in [`mfc/`](mfc/). That folder includes a Windows binary, which needs `foldit.ini` beside it; its help file is missing.

(*) Entering a line that names a file in single or double quotes opens that file, and leaving its top level returns you to the line that referenced it. This makes it easy to reference other files from a comment.

## Build and install

Needs a C++17 compiler and ncurses (`sudo apt install build-essential libncurses-dev`).

    make            # builds ./foldit
    make test       # parser round-trip tests
    make install    # ~/.local/bin/foldit, and ~/.config/foldit/foldit.ini if not present

    foldit file

## Keys

| Key | Action |
| --- | --- |
| Home / Ctrl+Left | enter fold, or open the file named in quotes on the line (elsewhere Home goes to the start of the line) |
| PgUp / Ctrl+Right | exit fold; at the top of a file go back to the referencing file, or quit |
| Ctrl+N | create fold (from the selected lines, or an empty one at the cursor column) |
| Ctrl+U | unfold |
| F3 / Ctrl+Y, F4 / Ctrl+K, F5 / Ctrl+B | cut / copy / paste line |
| Ctrl+X, Ctrl+C, Ctrl+V | cut / copy / paste selection (Shift+arrows to select) |
| Ctrl+Q | choose an earlier paste buffer |
| Ctrl+F / Ctrl+G | find / find next, in this fold and the folds inside it |
| Ctrl+R / Ctrl+E | indent / outdent lines (selected, or all in the fold) |
| Ctrl+D, Ctrl+T, Ctrl+A | insert date, fold title, find string |
| Ctrl+Z | undo |
| F6 / F7 / F8 | record keys / stop / play back |
| Ctrl+S, F10, F1 | save, quit, help |

## Clipboard

Cuts and copies also go to the Windows clipboard (or, outside WSL, to the terminal's clipboard via OSC 52). Windows Terminal handles Ctrl+V itself, and foldit receives the result as a bracketed paste. If the pasted text is what foldit last copied, foldit pastes its own buffer, which keeps folds intact even when pasting into a file that uses different comment markers. Text from other programs is read as lines, and any fold markers in it become folds.

## File format

Folds are stored as comments, so files stay ordinary source:

    //{{{  fold title
    ...lines in the fold...
    //}}}

The comment characters depend on the language, and languages are defined in `foldit.ini`. On save, tabs are expanded to spaces, trailing white space is removed, and the file's line endings (LF or CRLF) are kept.
