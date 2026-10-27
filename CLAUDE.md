# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

foldit is a terminal folding editor (C++17 + ncurses, Linux/WSL) modelled on the Inmos TDS editor: you *enter* a fold and see only its lines, rather than opening folds in place. The repo root holds the current editor. `mfc/` is an archive of the original 1995 Windows MFC version: don't modify it. Use it as the reference for intended behaviour and key bindings (`mfc/foldivw.cpp` for keys and view logic, `mfc/foldline.cpp` for fold operations). Its source files also serve as round-trip test data.

## Commands

    make                          # build ./foldit
    make test                     # build and run tests/test_fold.cpp (unit tests + round trip of mfc/*.cpp, mfc/*.h)
    ./build/test_fold FILE...     # also round-trip check extra files
    make install                  # ~/.local/bin/foldit (+ ~/.config/foldit/foldit.ini if absent)

There is no unit test for the UI. To check editor behaviour, drive it in tmux: `tmux new-session -d -s t -x 100 -y 30 "./foldit FILE"`, `tmux send-keys -t t Home`, `tmux capture-pane -p -t t`. Don't override TERM inside tmux, or Home/PgUp won't decode. `tmux send-keys -l ';'` is swallowed by tmux.

## Architecture

- `src/text.*`: text is `ustr` (`std::u32string`). Bytes that aren't valid UTF-8 decode to U+DC80..U+DCFF and encode back unchanged, so old cp1252 files round-trip exactly.
- `src/lang.*`: `Ini` reader, `Language` (comment and fold-mark strings), `Config` (loaded from `$FOLDIT_INI`, then `~/.config/foldit/foldit.ini`, then next to the executable, else built-in defaults). `Config::detect` picks a language from markers near the top of the file, then the extension, then a comment start. The format is compatible with the old `mfc/foldit.ini`.
- `src/fold.*`: the document is a tree of `Line`s. A text line holds its text, including indentation relative to its fold. A fold line holds its title, an indent relative to its parent, `kids` and `parent`. `parse_folds`/`write_folds` convert to and from text with markers `<indent><SCom><SFold>"  "title<ECom>` / `<indent><SCom><EFold><ECom>`. Folds always contain at least one line (`ensure_not_empty`).
- `src/editor.*`: a single `View` shows one fold of one `Doc`, full screen. Entering a fold or a file reference pushes a `Place` onto `View::stack`, and exiting pops it. Exiting the root of a file returns to the referencing file (releasing the doc), or quits at the bottom of the stack. Display columns on fold lines are `indent + prefix + 2 spaces + title`; editing is only allowed in the indent area or the title (`title_start`).
- Undo: `begin_edit()` is called before every modification. It snapshots the whole doc tree (typing on one line is coalesced). `undo()` re-resolves `Line*` pointers in the view and stack via index paths, so any new pointers into the tree must be handled there too.
- Folds that the view or the stack is inside (`in_use`) must not be deleted, cut, unfolded or moved.
- Clipboard: `push_paste` also `mirror`s each buffer to the system clipboard (detached `clip.exe`, else OSC 52) and remembers it in `last_mirror`. Bracketed paste is enabled and its markers are `define_key`'d to `PASTE_START`/`PASTE_END`; `paste_text` uses the internal buffer when the pasted text matches `last_mirror`, otherwise it parses the text as lines. Windows Terminal intercepts Ctrl+V, so this is how Ctrl+V works there.
- `Language::tab` (`Tab=`) is only for expanding tab characters on load; the Tab key uses `Indent=` (per language, else `[Defaults] Indent`, default 2).

## Conventions

- Source files are themselves folded with `//{{{  title` ... `//}}}` markers and start with `//{{{}}}`. Keep the markers balanced, and put new code inside an appropriate fold.
- Keep it simple: no multiple windows, threads, or mouse support (by design).
- The original author uses this daily. Their muscle memory of the old MFC hotkeys (see the accelerator table in `mfc/foldit.rc` and `CFolditView::OnKeyDown`) is the spec. Deliberate deviations: Home off a fold goes to line start; Ctrl+Z is undo (was serial number); F10 quits; multi-line selections are whole lines; trailing white space is stripped on save.
- Known limitation: tabs are expanded on load, so Makefiles get broken by saving.
