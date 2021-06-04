# pokegb

A gameboy emulator that only plays Pokemon Blue, in ~70 lines of c++.

## Features

Plays Pokemon Blue.

## Building

Only builds on Linux and macOS AFAIK.

```
$ make
```

On macOS, you'll need to create a save file too (just the first time):

```
$make rom.sav
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

## Updating keys

Look for [line 30](https://github.com/binji/pokegb/blob/9c03ea41714b24373a352f75d27b0dc86a5b2250/pokegb.cc#L30) the source.
The following table shows which numbers map to which keyboard keys:

| number | default key | gameboy button |
| - | - | - |
| 27 | X | A Button |
| 29 | Z | B Button |
| 43 | Tab | Select Button |
| 40 | Return | Start Button |
| 79 | Arrow Right | DPAD Right |
| 80 | Arrow Left | DPAD Left |
| 81 | Arrow Down | DPAD Down |
| 82 | Arrow Up | DPAD Up |

Replace the numbers on this line with one from the [SDL scancode list](https://www.libsdl.org/tmp/SDL/include/SDL_scancode.h).
