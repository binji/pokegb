# pokegb

A gameboy emulator that only plays Pokemon Blue, in ~70 lines of c++.

## Features

Plays Pokemon Blue.

## Building

Only builds on Linux AFAIK.

```
$ make
```

## Running

Put Pokemon Blue in `rom.gb`, then run:

```
$ ./pokegb
```

The save file is written to `rom.sav`.

Keys:

| Action | Key |
| --- | --- |
| DPAD-UP | <kbd>↑</kbd> |
| DPAD-DOWN | <kbd>↓</kbd> |
| DPAD-LEFT | <kbd>←</kbd> |
| DPAD-RIGHT | <kbd>→</kbd> |
| B | <kbd>Z</kbd> |
| A | <kbd>X</kbd> |
| START | <kbd>Enter</kbd> |
| SELECT | <kbd>Tab</kbd> |
