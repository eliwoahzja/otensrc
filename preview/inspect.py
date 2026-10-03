#!/usr/bin/env python3
"""Structural checks on the rendered menu screenshots.

Menu window is 780x480 centred on 1080x1920 -> shell rect (150,720)-(930,1200).
Accent comes from main_runtime_theme g_menuHue = 0.78 -> ~ (191,56,255) purple.
"""
import os
import sys
from collections import Counter

from PIL import Image

OUT = os.path.join(os.path.dirname(os.path.abspath(__file__)), "out")
MX0, MY0, MX1, MY1 = 150, 720, 930, 1200

SHOTS = [
    ("01_tab_aim", 1),
    ("02_tab_esp", 0),
    ("03_tab_memory", 2),
    ("04_tab_skins", 3),
    ("05_search_hit", 0),
    ("06_search_none", 0),
    ("07_combo_open", 0),
]


def px(im, x, y):
    return im.getpixel((x, y))[:3]


def is_accent(c):
    return c[0] > 130 and c[1] < 130 and c[2] > 190 and (c[0] - c[1]) > 60


def accent_rows(im, x0, x1, y0, y1, min_per_row=6, step=1):
    """Return list of (y, count) where accent pixels are dense."""
    out = []
    for y in range(y0, y1, step):
        n = 0
        for x in range(x0, x1, step):
            if is_accent(px(im, x, y)):
                n += 1
        if n >= min_per_row:
            out.append((y, n))
    return out


def bands(seq, min_len=3):
    """Group a sorted list of ys into contiguous runs."""
    res = []
    start = prev = None
    for y, _ in seq:
        if start is None:
            start = prev = y
            continue
        if y - prev <= 2:
            prev = y
        else:
            if prev - start + 1 >= min_len:
                res.append((start, prev))
            start = prev = y
    if start is not None and prev - start + 1 >= min_len:
        res.append((start, prev))
    return res


def text_rows(im, x0, x1, y0, y1):
    """Horizontal bands containing light (label) pixels."""
    res = []
    run = None
    for y in range(y0, y1):
        lit = 0
        for x in range(x0, x1):
            r, g, b = px(im, x, y)
            if r > 110 and g > 110 and b > 110:
                lit += 1
        if lit >= 3:
            if run is None:
                run = [y, y]
            else:
                run[1] = y
        else:
            if run is not None and run[1] - run[0] >= 1:
                res.append(tuple(run))
            run = None
    if run is not None and run[1] - run[0] >= 1:
        res.append(tuple(run))
    return res


def median_luma(im, x0, x1, y0, y1):
    """Median luminance over a rectangle; robust to glyphs and borders."""
    vals = []
    for y in range(y0, y1):
        for x in range(x0, x1):
            r, g, b = px(im, x, y)
            vals.append(0.299 * r + 0.587 * g + 0.114 * b)
    vals.sort()
    return vals[len(vals) // 2]


def main():
    failures = []
    pill_ys = {}

    for name, tab in SHOTS:
        path = os.path.join(OUT, name + ".png")
        if not os.path.exists(path):
            failures.append("%s: missing png" % name)
            continue
        im = Image.open(path).convert("RGB")
        if im.size != (1080, 1920):
            failures.append("%s: size %s" % (name, im.size))

        # 1. glass shell: the window is present at its top-left glass edge. Sample
        # a point that is inside the shell but outside the sidebar and the header,
        # since the exact centre lands on a card that may be lighter or darker.
        probe = ((MX0 + MX1) // 2, MY0 + 58)
        centre = px(im, *probe)
        outside = px(im, 40, 40)
        shell = sum(centre) > sum(outside) + 8

        # 2. Sidebar panel darker than the content area. Compare median luminance
        # over the empty gutter strip next to the tab icons against the content
        # pane. A median ignores glyphs and card borders, which a single pixel
        # sample can easily land on.
        side = median_luma(im, MX0 + 18, MX0 + 26, MY0 + 90, MY1 - 70)
        cont = median_luma(im, MX0 + 470, MX1 - 30, MY0 + 90, MY1 - 70)
        sidebar = side < cont

        # 3. active-tab accent pill inside the sidebar (x 160..346)
        pill = bands(accent_rows(im, MX0 + 12, MX0 + 190, MY0 + 20, MY1 - 20, 6), 8)
        pill_ys[name] = pill

        # 4. content rows of light text in the right pane
        rows = text_rows(im, MX0 + 215, MX1 - 25, MY0 + 95, MY1 - 25)

        print(
            "%-16s shell=%-5s sidebar=%-5s(luma %.0f<%.0f) pill=%-22s text_rows=%d centre=%s"
            % (name, shell, sidebar, side, cont, pill, len(rows), centre)
        )

        if not shell:
            failures.append("%s: glass shell not detected" % name)
        if not sidebar:
            failures.append("%s: sidebar not darker than content" % name)
        if not pill:
            failures.append("%s: no accent tab pill in sidebar" % name)
        if len(rows) < 2:
            failures.append("%s: too few content rows (%d)" % (name, len(rows)))

    # 5. the pill must move to a different y for a different active tab
    def pill_mid(n):
        p = pill_ys.get(n) or []
        return sum((a + b) / 2 for a, b in p) / len(p) if p else None

    m_esp, m_aim = pill_mid("02_tab_esp"), pill_mid("01_tab_aim")
    m_mem = pill_mid("03_tab_memory")
    m_skin = pill_mid("04_tab_skins")
    print("pill mid-y: esp=%s aim=%s memory=%s skins=%s" % (m_esp, m_aim, m_mem, m_skin))
    for a, b, na, nb in ((m_esp, m_aim, "esp", "aim"),
                         (m_esp, m_mem, "esp", "memory"),
                         (m_esp, m_skin, "esp", "skins")):
        if a is None or b is None or abs(a - b) < 20:
            failures.append("tab pill did not move between %s and %s (%s vs %s)" % (na, nb, a, b))

    # 6. search: no-match shot must be sparser than the hit shot
    hit = Image.open(os.path.join(OUT, "05_search_hit.png")).convert("RGB")
    non = Image.open(os.path.join(OUT, "06_search_none.png")).convert("RGB")
    rh = text_rows(hit, MX0 + 215, MX1 - 25, MY0 + 95, MY1 - 25)
    rn = text_rows(non, MX0 + 215, MX1 - 25, MY0 + 95, MY1 - 25)
    print("search text rows: hit=%d none=%d" % (len(rh), len(rn)))
    if len(rn) >= len(rh):
        failures.append("search: no-match shot not sparser (%d vs %d)" % (len(rn), len(rh)))

    # 7. combo popup: shot 07 must differ from 01 (same AIM tab, no popup)
    a = Image.open(os.path.join(OUT, "01_tab_aim.png")).convert("RGB")
    b = Image.open(os.path.join(OUT, "07_combo_open.png")).convert("RGB")
    diff = 0
    box = [10 ** 9, 10 ** 9, -1, -1]
    for y in range(MY0, MY1):
        for x in range(MX0, MX1):
            ca, cb = px(a, x, y), px(b, x, y)
            if sum(abs(ca[i] - cb[i]) for i in range(3)) > 30:
                diff += 1
                box[0] = min(box[0], x); box[1] = min(box[1], y)
                box[2] = max(box[2], x); box[3] = max(box[3], y)
    print("combo popup diff px=%d bbox=%s" % (diff, box))
    if diff < 300:
        failures.append("07_combo_open: no popup drawn (diff=%d)" % diff)
    else:
        # The footer status dot pulses continuously, so it always shows up in the
        # diff bbox. Ignore diff texels below the popup region to get the real box.
        pbox = [10 ** 9, 10 ** 9, -1, -1]
        for y in range(MY0, MY1 - 70):
            for x in range(MX0, MX1):
                ca, cb = px(a, x, y), px(b, x, y)
                if sum(abs(ca[i] - cb[i]) for i in range(3)) > 30:
                    pbox[0] = min(pbox[0], x); pbox[1] = min(pbox[1], y)
                    pbox[2] = max(pbox[2], x); pbox[3] = max(pbox[3], y)
        box = pbox
        bw, bh = box[2] - box[0], box[3] - box[1]
        print("  popup bbox (footer dot excluded):", box)
        if not (MX0 + 200 <= box[0] and box[2] <= MX1 and MY0 + 100 <= box[1] and box[3] <= MY1):
            failures.append("07_combo_open: popup bbox outside content pane %s" % box)
        if bh > 300 or bw > 700:
            failures.append("07_combo_open: popup bbox implausible %dx%d" % (bw, bh))
        # The popup panel is ImVec4(0.07,0.07,0.10,0.98) over the glass fill, so
        # its interior must be a solid, much darker block. Count those texels:
        # a real dropdown fills thousands, an unopened popup none.
        dark = 0
        for y in range(MY0, MY1):
            for x in range(MX0, MX1):
                r, g, bl = px(b, x, y)
                if 14 <= r <= 24 and 14 <= g <= 24 and 22 <= bl <= 32:
                    dark += 1
        print("  popup panel texels:", dark)
        if dark < 8000:
            failures.append("07_combo_open: popup panel not filled (%d texels)" % dark)

    print()
    if failures:
        print("FAIL (%d)" % len(failures))
        for f in failures:
            print("  - " + f)
        return 1
    print("ALL STRUCTURAL CHECKS PASSED")
    return 0


if __name__ == "__main__":
    sys.exit(main())