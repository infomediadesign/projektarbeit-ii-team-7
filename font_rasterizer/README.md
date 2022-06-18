# Font Rasterizer

A small applet to create monospace fontmaps from .ttf / .ttc / .otf files. Designed for the university project, but may be used by anyone I guess.

## Compiling

```
cargo build
```

....seriously it's that easy.

## Usage

```
font_rasterizer ./path/to/font.ttf
font_rasterizer ./path/to/font.ttf ABCDEFGHIJKLMNOPQRSTUVWXYZ
```

...will produce `rasterized_font.png` and `rasterized_font.h` in current working directory.
