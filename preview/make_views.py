#!/usr/bin/env python3
"""Derive easy-to-open views from the rendered menu PNGs.

The harness writes full-resolution 1080x1920 frames. Those are ~8 MB each, which
is awkward to open and impossible to eyeball several of at once, so this writes:

  view_menu_<shot>.png  - just the menu window (800x500), the thing to inspect
  view_full_<shot>.png  - whole screen downscaled to 540x960, for context
  contact_sheet.png     - all seven shots in one labelled 2-column grid

Run after build.sh:

    python3 preview/make_views.py
"""
import os
import sys

from PIL import Image, ImageDraw, ImageFont

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "out")

# Menu window is 780x480 centred on a 1080x1920 canvas; crop with a small margin.
BX0, BY0, BX1, BY1 = 140, 710, 940, 1210

SHOTS = [
    ("01_tab_aim", "01  AIMBOT tab"),
    ("02_tab_esp", "02  VISUALS (ESP) tab"),
    ("03_tab_memory", "03  MEMORY tab"),
    ("04_tab_skins", "04  SKINS tab"),
    ("05_search_hit", '05  Search: "esp"'),
    ("06_search_none", "06  Search: no matches"),
    ("07_combo_open", "07  Combo dropdown open"),
]


def load_font(size):
    for path in (
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf",
    ):
        if os.path.exists(path):
            try:
                return ImageFont.truetype(path, size)
            except OSError:
                pass
    return None


def main():
    missing = [s for s, _ in SHOTS if not os.path.exists(os.path.join(OUT, s + ".png"))]
    if missing:
        print("missing rendered shots: %s" % ", ".join(missing), file=sys.stderr)
        print("run: sh preview/build.sh", file=sys.stderr)
        return 1

    tiles = []
    for shot, label in SHOTS:
        im = Image.open(os.path.join(OUT, shot + ".png")).convert("RGB")
        crop = im.crop((BX0, BY0, BX1, BY1))
        crop.save(os.path.join(OUT, "view_menu_%s.png" % shot))
        im.resize((540, 960), Image.LANCZOS).save(os.path.join(OUT, "view_full_%s.png" % shot))
        tiles.append((label, crop))

    tw, th = tiles[0][1].size
    cols = 2
    rows = (len(tiles) + cols - 1) // cols
    pad, hdr = 16, 34
    sheet = Image.new(
        "RGB",
        (cols * tw + (cols + 1) * pad, rows * (th + hdr) + (rows + 1) * pad),
        (16, 17, 20),
    )
    draw = ImageDraw.Draw(sheet)
    font = load_font(19)
    for i, (label, im) in enumerate(tiles):
        r, c = divmod(i, cols)
        x = pad + c * (tw + pad)
        y = pad + r * (th + hdr + pad)
        draw.text((x + 2, y + 6), label, fill=(235, 235, 240), font=font)
        sheet.paste(im, (x, y + hdr))
    sheet.save(os.path.join(OUT, "contact_sheet.png"))

    print("wrote contact_sheet.png %dx%d and %d view files"
          % (sheet.size[0], sheet.size[1], len(tiles) * 2))
    return 0


if __name__ == "__main__":
    sys.exit(main())