/* ============================================================================
   ETHNIR — site preview behaviour.

   Mirrors src/main/jni/ImGui/ethnir_menu.h: the same 880x520 shell, 200px
   sidebar, six pages (AimBot / Players / World / Skins / Misc / Config),
   two-column cards headed by uppercase sections, rows that read label-left /
   control-right, iOS switches, an 84px accent slider track, a search hint that
   counts the functions on the page, the NO MATCHES card on the four filtered
   pages, combo menus that flip above the row, the floating quick-settings
   panel, the "?" help card, aving Save confirmation and the minimise pill.
   ========================================================================== */
(function () {
  "use strict";

  /* ------------------------------- icons -------------------------------- */

  var PATHS = {
    crosshair: '<circle cx="8" cy="8" r="5.4"/><path d="M8 0.6v3.2M8 12.2v3.2M0.6 8h3.2M12.2 8h3.2"/>',
    users: '<circle cx="6" cy="5.6" r="2.6"/><path d="M1.6 13.4c0-2.4 2-4.1 4.4-4.1s4.4 1.7 4.4 4.1"/><path d="M11 4.2a2.2 2.2 0 0 1 0 4.4"/>',
    globe: '<circle cx="8" cy="8" r="6.2"/><path d="M1.8 8h12.4M8 1.8c1.9 2.1 1.9 10.3 0 12.4M8 1.8c-1.9 2.1-1.9 10.3 0 12.4"/>',
    list: '<path d="M5.6 3.6h9M5.6 8h9M5.6 12.4h9"/><circle cx="2.4" cy="3.6" r="0.9" fill="currentColor" stroke="none"/><circle cx="2.4" cy="8" r="0.9" fill="currentColor" stroke="none"/><circle cx="2.4" cy="12.4" r="0.9" fill="currentColor" stroke="none"/>',
    sliders: '<path d="M2 4.6h12M2 11.4h12"/><circle cx="5.6" cy="4.6" r="1.9"/><circle cx="10.4" cy="11.4" r="1.9"/>',
    cog: '<circle cx="8" cy="8" r="2.9"/><path d="M8 1.2v2.1M8 12.7v2.1M1.2 8h2.1M12.7 8h2.1M3.2 3.2l1.5 1.5M11.3 11.3l1.5 1.5M12.8 3.2l-1.5 1.5M4.7 11.3l-1.5 1.5"/>',
    search: '<circle cx="7.1" cy="7.1" r="4.7"/><path d="M10.6 10.6 14 14"/>',
    save: '<path d="M3 2.2h7.6L14 5.8v8H3z"/><path d="M5.6 2.2v3.6h5V2.2M5.6 13.8v-4h5v4"/>',
    minus: '<path d="M3 8h10"/>',
    times: '<path d="M3.2 3.2l9.6 9.6M12.8 3.2 3.2 12.8"/>',
    check: '<path d="M2.4 8.6 6 12.2 13.6 3.8"/>',
    chev: '<path d="M1.6 4.2 5 7.6l3.4-3.4"/>',
    eye: '<path d="M1.3 8S3.8 3.2 8 3.2 14.7 8 14.7 8 12.2 12.8 8 12.8 1.3 8 1.3 8z"/><circle cx="8" cy="8" r="2.4"/>',
    bolt: '<path d="M9.4 1 3.4 9h3.3l-1.1 6 6-8H8.3z"/>',
    shield: '<path d="M8 1.4l5.4 2v4.1c0 3.2-2.2 5.8-5.4 7.1-3.2-1.3-5.4-3.9-5.4-7.1V3.4z"/>',
    clock: '<circle cx="8" cy="8" r="6"/><path d="M8 4.4V8l2.6 1.6"/>'
  };

  function ic(name, cls) {
    return '<svg class="' + (cls || "ic") + '" viewBox="0 0 16 16" fill="none" stroke="currentColor" ' +
      'stroke-width="1.45" stroke-linecap="round" stroke-linejoin="round">' + (PATHS[name] || PATHS.eye) + "</svg>";
  }

  /* -------------------------------- data -------------------------------- */

  var NAV = [
    { tab: 1, icon: "crosshair", label: "AimBot" },
    { tab: 0, icon: "users", label: "Players" },
    { tab: 2, icon: "globe", label: "World" },
    { tab: 3, icon: "list", label: "Skins" },
    { tab: 4, icon: "sliders", label: "Misc" },
    { tab: 5, icon: "cog", label: "Config" }
  ];

  // hue / 0.78 saturation / 1.0 value — the same maths as ApplyAccentFromHue()
  var ACCENTS = [
    { name: "Blue", hue: 0.5837 },
    { name: "Teal", hue: 0.49 },
    { name: "Green", hue: 0.36 },
    { name: "Gold", hue: 0.115 },
    { name: "Pink", hue: 0.93 },
    { name: "Violet", hue: 0.74 }
  ];

  var BINDS = ["Num 0", "Num 1", "F1", "F4", "Home", "None"];
  var COLOR_CYCLE = ["#00D4FF", "#00D278", "#FF3C50", "#FFC832", "#0A84FF", "#FFFFFF"];

  function toggle(label, on) { return { kind: "toggle", label: label, on: !!on }; }
  function slider(label, value, min, max, fmt) { return { kind: "slider", label: label, value: value, min: min, max: max, fmt: fmt }; }
  function combo(label, options, value) { return { kind: "combo", label: label, options: options, value: value }; }
  function color(label, hex) { return { kind: "color", label: label, hex: hex }; }
  function kv(label, value) { return { kind: "kv", label: label, value: value }; }
  function note(text) { return { kind: "note", text: text }; }
  function bullet(icon, text) { return { kind: "bullet", icon: icon, text: text }; }
  function section(label) { return { kind: "section", label: label }; }
  function card(rows) { return { kind: "card", rows: rows }; }
  function cols(a, b) { return { kind: "cols", cols: [a, b] }; }
  function chips(items, active) { return { kind: "chips", items: items, active: active }; }

  /* segmented control — mirrors SegmentedRow() in ethnir_menu.h */
  function renderSeg(spec) {
    var track = el("span", "segtrack");
    spec.items.forEach(function (item, i) {
      var b = el("button", "segbtn" + (i === spec.active ? " on" : ""), item);
      b.type = "button";
      b.setAttribute("data-v", String(i));
      b.setAttribute("aria-pressed", i === spec.active ? "true" : "false");
      b.addEventListener("click", function (ev) {
        ev.stopPropagation();
        spec.active = i;
        var sibs = track.children;
        for (var k = 0; k < sibs.length; k++) {
          sibs[k].classList.toggle("on", k === i);
          sibs[k].setAttribute("aria-pressed", k === i ? "true" : "false");
        }
        if (spec.onChange) spec.onChange(i);
      });
      track.appendChild(b);
    });
    return track;
  }
  function seg(label, items, active) { return { kind: "seg", label: label, items: items, active: active }; }
  function buttons(items) { return { kind: "buttons", items: items }; }

  /* Row data transcribed from runtime_preview_menu.h and Main.cpp */
  var PAGES = {
    1: [cols(
      [section("AIMBOT"), card([
        toggle("Aimbot 360", true),
        toggle("Bullet Track", true),
        slider("Aim Assist Size", 0, 0, 100, "%.0f")
      ])],
      [section("COMBAT"), card([
        combo("Location", ["Head", "Chest", "Body"], 0),
        combo("Trigger", ["None", "Shooting", "Scoping"], 1),
        combo("Target By", ["Distance", "FOV"], 0),
        slider("FOV Size", 45, 0, 100, "%.0f")
      ])]
    )],
    0: [cols(
      [section("PLAYERS"), card([
        toggle("ESP Line", true), toggle("ESP Box", true), toggle("ESP Skeleton", true),
        toggle("ESP Health", true), toggle("ESP Name", true), toggle("ESP Distance", true),
        toggle("ESP Count", false), toggle("360 Alert", false), toggle("Show AimLine", false),
        toggle("Yellow Wallhack", true), toggle("Red Wallhack", false)
      ])],
      [section("ESP OPTIONS"), card([
        combo("Box Type", ["Fill", "Outline", "Corner", "3D"], 1),
        combo("Line Position", ["Top", "Mid", "Bottom"], 0),
        combo("Health Position", ["Top", "Side"], 0),
        combo("ESP Style", ["None", "3D Sphere", "Player Signal"], 0),
        color("Player ESP Color", "#00D4FF"),
        color("Bot ESP Color", "#00D278")
      ])]
    )],
    2: [cols(
      [section("COMBAT HACKS"), card([
        toggle("Hitbox", true), toggle("No Recoil", true), toggle("No Spread", true),
        toggle("No Shake", false), toggle("No Overheat", false), toggle("Firerate", true),
        toggle("Weapon Kinetic", false)
      ])],
      [section("UTILITY HACKS"), card([
        toggle("Unlock Blueprint", true), toggle("No Parachute", false), toggle("Anti Flashbang", false),
        toggle("Fast Dive", false), toggle("Fast Reload", true), toggle("Fast Scope", false),
        toggle("Quick Switch", false)
      ]),
      section("MOVEMENT"), card([
        slider("Snowboard Speed", 0, 0, 100, "%.1f"),
        slider("Slide Distance", 0, 0, 30, "%.1f"),
        slider("SpeedHack", 1, 0.5, 2, "%.1fx"),
        slider("High Jump", 1, 0.5, 5, "%.2fx")
      ])]
    )],
    3: [
      chips(["Character", "Watch", "Deadbox", "Plane", "Weapon", "Camo"], 4),
      section("WEAPON SKINS"), card([
        kv("AK117", "Default"), kv("QQ9", "Default"), kv("DL Q33", "Default"),
        kv("Kilo Bolt-Action", "Default"), kv("PDW-57", "Default")
      ]),
      section("CAMO MODIFIER"), card([
        toggle("Default / OFF", true), toggle("Diamond Camo", false), toggle("Red Sprite Camo", false),
        note("Only applies to [M] Mythic and [L] Legendary weapon skins.")
      ])
    ],
    4: [
      section("CHANGELOG"), card([
        chips(["FEATURES", "FIXES", "UPDATES"], 0),
        kv("March 10, 2026", ""),
        bullet("shield", "<b>New Ethnir shell</b> — iOS-clean rows, two-column cards and a quiet accent."),
        bullet("bolt", "<b>Quick settings panel</b> — theme, animation speed, accent and menu bind in one place."),
        bullet("clock", "<b>Backdrop fallback</b> — the shell paints its own wallpaper when no texture is bound.")
      ]),
      section("INFO"), card([kv("Version", "v2.0"), kv("Loader", "Online"), kv("Build", "2026")]),
      section("PRICELIST"), card([
        kv("1 Day", "\u20b150 / $1"), kv("1 Week", "\u20b1250 / $5"),
        kv("1 Month", "\u20b1600 / $12"), kv("Lifetime", "\u20b11200 / $24")
      ])
    ],
    5: [cols(
      [section("LICENSE INFO"), card([kv("Licensed to", "White Crowns"), kv("Plan", "Lifetime"), kv("Expires", "Never")]),
       section("LOGO SETTINGS"), card([
         slider("Logo Size", 1, 0.1, 2, "%.2fx"),
         slider("Logo Opacity", 1, 0, 1, "%.2f"),
         toggle("Show Logo", true)
       ])],
      [section("CONFIG MANAGEMENT"), card([buttons(["SAVE", "LOAD", "RESET"])]),
       section("ENHANCEMENT"), card([
         toggle("Streamer Mode", false), toggle("Low Latency", true),
         slider("UI Scale", 1, 0.8, 1.4, "%.2fx")
       ])]
    )]
  };

  var FILTERABLE = { toggle: 1, slider: 1, combo: 1, color: 1 };
  var QUERY_TABS = { 0: 1, 1: 1, 2: 1, 3: 1 };   // pages that run RowToggle/RowSlider/ComboRow

  /* ------------------------------ elements ------------------------------ */

  var stage = document.getElementById("stage");
  var stageInner = document.getElementById("stageInner");
  var shell = document.getElementById("shell");
  var nav = document.getElementById("nav");
  var pane = document.getElementById("pane");
  var content = document.getElementById("content");
  var nomatch = document.getElementById("nomatch");
  var drop = document.getElementById("drop");
  var panel = document.getElementById("panel");
  var panelRows = document.getElementById("panelRows");
  var panelHead = document.getElementById("panelHead");
  var searchInput = document.getElementById("searchInput");
  var clearBtn = document.getElementById("clearBtn");
  var saveBtn = document.getElementById("saveBtn");
  var saveIcon = document.getElementById("saveIcon");
  var saveLabel = document.getElementById("saveLabel");
  var gearBtn = document.getElementById("gearBtn");
  var minBtn = document.getElementById("minBtn");
  var helpBtn = document.getElementById("helpBtn");
  var helpCard = document.getElementById("helpCard");
  var fpsText = document.getElementById("fpsText");
  var pill = document.getElementById("pill");
  var pillFps = document.getElementById("pillFps");
  var heroSwatch = document.getElementById("heroSwatch");
  var heroAccent = document.getElementById("heroAccent");

  var state = {
    tab: 1,
    search: "",
    dark: true,
    panel: true,
    accent: 0,
    anim: 1,
    bind: 0,
    wall: false,
    collapsed: false,
    scale: 1
  };
  var openCombo = null;
  var saveTimer = null;

  function el(tag, cls, html) {
    var n = document.createElement(tag);
    if (cls) n.className = cls;
    if (html != null) n.innerHTML = html;
    return n;
  }

  /* ------------------------------ palette ------------------------------- */

  function hsvToRgb(h, s, v) {
    var c = v * s;
    var hp = (h - Math.floor(h)) * 6;
    var x = c * (1 - Math.abs((hp % 2) - 1));
    var r = 0, g = 0, b = 0;
    if (hp < 1) { r = c; g = x; }
    else if (hp < 2) { r = x; g = c; }
    else if (hp < 3) { g = c; b = x; }
    else if (hp < 4) { g = x; b = c; }
    else if (hp < 5) { r = x; b = c; }
    else { r = c; b = x; }
    var m = v - c;
    return [Math.round((r + m) * 255), Math.round((g + m) * 255), Math.round((b + m) * 255)];
  }

  function accentRgb(i) { return hsvToRgb(ACCENTS[i].hue, 0.78, 1.0); }

  function hex(rgb) {
    return "#" + rgb.map(function (v) { return ("0" + v.toString(16)).slice(-2).toUpperCase(); }).join("");
  }

  function applyAccent() {
    var rgb = accentRgb(state.accent);
    var css = "rgb(" + rgb.join(",") + ")";
    document.documentElement.style.setProperty("--accent", css);
    document.documentElement.style.setProperty("--accent-soft", "rgba(" + rgb.join(",") + ",0.22)");
    heroSwatch.style.background = css;
    heroAccent.textContent = hex(rgb);
    var sws = panelRows.querySelectorAll(".sw");
    for (var i = 0; i < sws.length; i++) {
      var c = accentRgb(i);
      sws[i].style.background = "rgb(" + c.join(",") + ")";
      sws[i].style.color = "rgb(" + c.join(",") + ")";
      sws[i].setAttribute("aria-pressed", i === state.accent ? "true" : "false");
    }
  }

  function applyTheme() {
    stage.dataset.theme = state.dark ? "dark" : "light";
    renderPanelRows();
    applyAccent();
  }

  function applyAnim() {
    document.documentElement.style.setProperty("--anim", String(state.anim));
  }

  /* -------------------------------- layout ------------------------------ */

  function layout() {
    var avail = Math.min(window.innerWidth - 48, 1400);
    var s = Math.min(1.12, Math.max(0.4, avail / 1156));
    state.scale = s;
    stage.style.width = (1156 * s) + "px";
    stage.style.height = (520 * s) + "px";
    stageInner.style.transform = "scale(" + s + ")";
  }

  /* -------------------------------- nav --------------------------------- */

  function renderNav() {
    nav.innerHTML = "";
    var hl = el("span", "nav-hl");
    nav.appendChild(hl);
    NAV.forEach(function (item, i) {
      var b = el("button", "tab", ic(item.icon) + "<span>" + item.label + "</span><i class=\"dot\"></i>");
      b.type = "button";
      b.setAttribute("aria-current", item.tab === state.tab ? "true" : "false");
      if (item.tab === state.tab) hl.style.top = (i * 38) + "px";
      b.addEventListener("click", function () { setTab(item.tab, i); });
      nav.appendChild(b);
    });
    nav._hl = hl;
  }

  function setTab(tab, index) {
    if (state.tab === tab) return;
    state.tab = tab;
    var tabs = nav.querySelectorAll(".tab");
    for (var i = 0; i < tabs.length; i++) {
      tabs[i].setAttribute("aria-current", NAV[i].tab === tab ? "true" : "false");
      if (NAV[i].tab === tab) nav._hl.style.top = (i * 38) + "px";
    }
    closeDrop();
    renderPane();
    applySearch();
    content.scrollTop = 0;
  }

  /* ------------------------------- rows --------------------------------- */

  function fmtValue(fmt, v) {
    if (fmt === "%.0f") return String(Math.round(v));
    if (fmt === "%.1f") return v.toFixed(1);
    if (fmt === "%.2f") return v.toFixed(2);
    if (fmt === "%.0f%%") return Math.round(v) + "%";
    if (fmt === "%.1fx") return v.toFixed(1) + "x";
    if (fmt === "%.2fx") return v.toFixed(2) + "x";
    return String(v);
  }

  function renderRow(spec) {
    var node;
    var label;

    if (spec.kind === "toggle") {
      node = el("div", "row toggle" + (spec.on ? " on" : ""),
        '<span class="rlabel">' + spec.label + '</span><span class="switch"><i></i></span>');
      node.addEventListener("click", function () {
        spec.on = !spec.on;
        node.classList.toggle("on", spec.on);
      });
    } else if (spec.kind === "slider") {
      node = el("div", "row slider",
        '<span class="rlabel">' + spec.label + '</span><span class="rvalue"></span>' +
        '<span class="track"><span class="fill"></span><span class="knob"></span></span>');
      var rvalue = node.querySelector(".rvalue");
      var track = node.querySelector(".track");
      var fill = node.querySelector(".fill");
      var knob = node.querySelector(".knob");
      var paint = function () {
        var t = Math.max(0, Math.min(1, (spec.value - spec.min) / (spec.max - spec.min)));
        rvalue.textContent = fmtValue(spec.fmt, spec.value);
        fill.style.width = (t * 100) + "%";
        knob.style.left = (t * 84) + "px";
      };
      var setFromX = function (clientX) {
        var r = track.getBoundingClientRect();
        var t = Math.max(0, Math.min(1, (clientX - r.left) / (r.width || 1)));
        spec.value = spec.min + t * (spec.max - spec.min);
        paint();
        if (spec.onChange) spec.onChange(spec.value);
      };
      node.addEventListener("pointerdown", function (ev) {
        ev.preventDefault();
        node.classList.add("active");
        setFromX(ev.clientX);
        var move = function (e) { setFromX(e.clientX); };
        var up = function () {
          node.classList.remove("active");
          window.removeEventListener("pointermove", move);
          window.removeEventListener("pointerup", up);
        };
        window.addEventListener("pointermove", move);
        window.addEventListener("pointerup", up);
      });
      paint();
      node._paint = paint;
    } else if (spec.kind === "combo") {
      node = el("div", "row combo",
        '<span class="rlabel">' + spec.label + '</span>' +
        '<span class="rvalue">' + spec.options[spec.value] + "</span>" +
        '<span class="chev">' + ic("chev") + "</span>");
      node.addEventListener("click", function () { openDrop(spec, node); });
    } else if (spec.kind === "color") {
      node = el("div", "row color",
        '<span class="rlabel">' + spec.label + '</span><span class="rhex">' + spec.hex + '</span><span class="chip"></span>');
      var chip = node.querySelector(".chip");
      var paintChip = function () {
        chip.style.background = spec.hex;
        node.querySelector(".rhex").textContent = spec.hex;
      };
      node.addEventListener("click", function () {
        var i = (COLOR_CYCLE.indexOf(spec.hex) + 1) % COLOR_CYCLE.length;
        spec.hex = COLOR_CYCLE[i];
        paintChip();
      });
      paintChip();
    } else if (spec.kind === "kv") {
      node = el("div", "row kv", "<span>" + spec.label + "</span>" + (spec.value ? "<b>" + spec.value + "</b>" : ""));
    } else if (spec.kind === "note") {
      node = el("div", "row note", spec.text);
    } else if (spec.kind === "seg") {
      node = el("div", "row segrow");
      node.appendChild(el("span", "rlabel", spec.label || ""));
      node.appendChild(renderSeg(spec));
    } else if (spec.kind === "bullet") {
      node = el("div", "row bullet", ic(spec.icon) + "<span>" + spec.text + "</span>");
    } else if (spec.kind === "buttons") {
      node = el("div", "row btnrow");
      spec.items.forEach(function (labelText) {
        var b = el("button", "minibtn", labelText);
        b.type = "button";
        b.addEventListener("click", function () {
          b.textContent = labelText === "RESET" ? labelText : "OK";
          window.setTimeout(function () { b.textContent = labelText; }, 900);
        });
        node.appendChild(b);
      });
    }
    node._spec = spec;
    return node;
  }

  function renderBlocks(blocks) {
    var frag = document.createDocumentFragment();
    blocks.forEach(function (block) {
      if (block.kind === "cols") {
        var wrap = el("div", "cols");
        block.cols.forEach(function (column) {
          var col = el("div", "col");
          col.appendChild(renderBlocks(column));
          wrap.appendChild(col);
        });
        frag.appendChild(wrap);
        return;
      }
      
      if (block.kind === "chips") {
        var strip = el("div", "chiprow");
        block.items.forEach(function (item, i) {
          var b = el("button", "chipbtn", item);
          b.type = "button";
          b.setAttribute("aria-pressed", i === block.active ? "true" : "false");
          b.addEventListener("click", function () {
            block.active = i;
            var sibs = strip.children;
            for (var k = 0; k < sibs.length; k++) sibs[k].setAttribute("aria-pressed", k === i ? "true" : "false");
          });
          strip.appendChild(b);
        });
        frag.appendChild(strip);
        return;
      }
      if (block.kind === "section") {
        frag.appendChild(el("h4", "section", block.label));
        return;
      }
      var cardNode = el("div", "card");
      block.rows.forEach(function (row) { cardNode.appendChild(renderRow(row)); });
      frag.appendChild(cardNode);
    });
    return frag;
  }

  function renderPane() {
    closeDrop();
    pane.innerHTML = "";
    pane.hidden = false;
    nomatch.hidden = true;
    pane.appendChild(renderBlocks(PAGES[state.tab] || []));
    // fade the pane in, the way Render() fades content on a tab switch
    pane.classList.add("fade");
    void pane.offsetWidth;
    window.requestAnimationFrame(function () { pane.classList.remove("fade"); });
  }

  /* ------------------------------- search ------------------------------- */

  function functionCount() {
    var rows = pane.querySelectorAll(".row");
    var n = 0;
    for (var i = 0; i < rows.length; i++) if (rows[i]._spec && FILTERABLE[rows[i]._spec.kind]) n++;
    return n;
  }

  function updateHint() {
    searchInput.placeholder = "explore " + functionCount() + " functions...";
  }

  function applySearch() {
    var q = state.search.trim().toLowerCase();
    var filtering = q.length > 0;
    clearBtn.hidden = !filtering;
    var queryPage = !!QUERY_TABS[state.tab];
    var anyMatch = !filtering;

    var rows = pane.querySelectorAll(".row");
    for (var i = 0; i < rows.length; i++) {
      var spec = rows[i]._spec;
      var show = true;
      if (filtering && queryPage && spec && FILTERABLE[spec.kind]) {
        show = spec.label.toLowerCase().indexOf(q) !== -1;
        if (show) anyMatch = true;
      }
      rows[i].hidden = !show;
    }
    // a card with every row filtered out collapses, like the auto-fit child
    var cards = pane.querySelectorAll(".card");
    for (var c = 0; c < cards.length; c++) {
      var visible = cards[c].querySelectorAll(".row:not([hidden])").length;
      cards[c].hidden = visible === 0;
    }

    var showNoMatch = filtering && queryPage && !anyMatch;
    nomatch.hidden = !showNoMatch;
    pane.hidden = showNoMatch;
    if (showNoMatch) closeDrop();
    updateHint();
  }

  /* ------------------------------ dropdown ------------------------------ */

  function closeDrop() {
    drop.hidden = true;
    drop.innerHTML = "";
    if (openCombo) openCombo.classList.remove("open");
    openCombo = null;
  }

  function openDrop(spec, rowEl) {
    if (openCombo === rowEl) { closeDrop(); return; }
    closeDrop();
    openCombo = rowEl;
    rowEl.classList.add("open");
    drop.innerHTML = "";
    spec.options.forEach(function (opt, i) {
      var d = el("div", "di", "<span>" + opt + "</span>");
      d.setAttribute("aria-selected", i === spec.value ? "true" : "false");
      d.addEventListener("click", function () {
        spec.value = i;
        var value = rowEl.querySelector(".rvalue");
        if (value) value.textContent = opt;
        if (spec.onChange) spec.onChange(i);
        closeDrop();
      });
      drop.appendChild(d);
    });
    drop.hidden = false;
    positionDrop();
  }

  // Anchored to the row every frame (ComboRow() sets the popup from the row
  // rect), so scrolling the pane moves the menu with its row.
  function positionDrop() {
    if (!openCombo || !openCombo._spec) return;
    var shellRect = stageInner.getBoundingClientRect();
    var r = openCombo.getBoundingClientRect();
    var paneRect = content.getBoundingClientRect();
    if (r.bottom < paneRect.top - 1 || r.top > paneRect.bottom + 1) { closeDrop(); return; }

    var s = state.scale || 1;
    var x = (r.left - shellRect.left) / s;
    var w = r.width / s;
    var itemH = 28;
    var h = openCombo._spec.options.length * itemH + 10 + 2;
    var y = (r.bottom - shellRect.top) / s + 4;
    if (y + h > 512 && (r.top - shellRect.top) / s - h - 4 > 0) y = (r.top - shellRect.top) / s - h - 4;

    drop.style.left = Math.max(8, Math.min(x, 1156 - 12 - w)) + "px";
    drop.style.top = Math.max(8, y) + "px";
    drop.style.width = w + "px";
  }

  /* ------------------------------- panel -------------------------------- */

  function renderPanelRows() {
    panelRows.innerHTML = "";
    var themeB = seg("Theme", ["Dark", "Light"], state.dark ? 0 : 1);
    themeB.onChange = function (v) { state.dark = v === 0; applyTheme(); };

    var animPct = Math.round(state.anim * 100);
    var animS = slider("Animation", animPct, 50, 200, "%.0f%%");
    animS.onChange = function (v) { state.anim = Math.round(v) / 100; applyAnim(); };

    panelRows.appendChild(renderRow(themeB));
    panelRows.appendChild(renderRow(animS));

    var accentRow = el("div", "row accent",
      '<span class="rlabel">Accent color</span><span class="swatches"></span>');
    var swatches = accentRow.querySelector(".swatches");
    ACCENTS.forEach(function (a, i) {
      var b = el("button", "sw");
      b.type = "button";
      b.title = a.name;
      b.setAttribute("aria-pressed", i === state.accent ? "true" : "false");
      b.addEventListener("click", function (ev) {
        ev.stopPropagation();
        state.accent = i;
        applyAccent();
      });
      swatches.appendChild(b);
    });
    panelRows.appendChild(accentRow);

    var bindRow = el("div", "row bindrow", '<span class="rlabel">Menu bind</span><span class="bind">' + BINDS[state.bind] + "</span>");
    bindRow.addEventListener("click", function () {
      state.bind = (state.bind + 1) % BINDS.length;
      bindRow.querySelector(".bind").textContent = BINDS[state.bind];
    });
    panelRows.appendChild(bindRow);
    applyAccent();
  }

  function dragPanel() {
    var dragging = false;
    var start = null;
    panelHead.addEventListener("pointerdown", function (ev) {
      ev.preventDefault();
      dragging = true;
      var r = panel.getBoundingClientRect();
      var stageRect = stageInner.getBoundingClientRect();
      start = {
        x: (r.left - stageRect.left) / state.scale,
        y: (r.top - stageRect.top) / state.scale,
        px: ev.clientX,
        py: ev.clientY
      };
      panelHead.style.cursor = "grabbing";
    });
    window.addEventListener("pointermove", function (ev) {
      if (!dragging) return;
      var nx = start.x + (ev.clientX - start.px) / state.scale;
      var ny = start.y + (ev.clientY - start.py) / state.scale;
      panel.style.left = Math.max(4, Math.min(nx, 1156 - 262)) + "px";
      panel.style.top = Math.max(4, Math.min(ny, 460)) + "px";
    });
    window.addEventListener("pointerup", function () {
      dragging = false;
      panelHead.style.cursor = "grab";
    });
  }

  /* -------------------------------- header ------------------------------ */

  function flashSave() {
    saveBtn.dataset.flash = "1";
    saveIcon.innerHTML = ic("check");
    saveLabel.textContent = "Saved";
    if (saveTimer) window.clearTimeout(saveTimer);
    saveTimer = window.setTimeout(function () {
      saveBtn.dataset.flash = "0";
      saveIcon.innerHTML = ic("save");
      saveLabel.textContent = "Save";
    }, 1600);
  }

  function setCollapsed(collapsed) {
    state.collapsed = collapsed;
    shell.hidden = collapsed;
    panel.hidden = collapsed || !state.panel;
    helpCard.hidden = true;
    pill.hidden = !collapsed;
    if (collapsed) closeDrop();
  }

  function updateFps() {
    var fps = 55 + Math.round(Math.random() * 7);
    fpsText.textContent = fps + " FPS";
    pillFps.textContent = fps + " FPS";
  }

  /* ------------------------------ interactions -------------------------- */

  function wire() {
    document.getElementById("searchIcon").innerHTML = ic("search");
    document.getElementById("nomatchIcon").innerHTML = ic("search");
    document.getElementById("clearBtn").innerHTML = ic("times");
    saveIcon.innerHTML = ic("save");
    gearBtn.innerHTML = ic("cog");
    minBtn.innerHTML = ic("minus");

    searchInput.addEventListener("input", function () {
      state.search = searchInput.value;
      applySearch();
    });

    clearBtn.addEventListener("click", function () {
      searchInput.value = "";
      state.search = "";
      applySearch();
      searchInput.focus();
    });

    saveBtn.addEventListener("click", flashSave);

    gearBtn.addEventListener("click", function () {
      state.panel = !state.panel;
      panel.hidden = !state.panel;
      gearBtn.setAttribute("aria-pressed", state.panel ? "true" : "false");
      closeDrop();
    });
    gearBtn.setAttribute("aria-pressed", state.panel ? "true" : "false");

    minBtn.addEventListener("click", function () { setCollapsed(true); });
    pill.addEventListener("click", function () { setCollapsed(false); });

    helpBtn.addEventListener("click", function (ev) {
      ev.stopPropagation();
      helpCard.hidden = !helpCard.hidden;
      helpBtn.setAttribute("aria-expanded", helpCard.hidden ? "false" : "true");
    });

    content.addEventListener("scroll", positionDrop);

    document.addEventListener("pointerdown", function (ev) {
      if (helpCard.contains(ev.target) || helpBtn.contains(ev.target)) return;
      if (!helpCard.hidden) {
        helpCard.hidden = true;
        helpBtn.setAttribute("aria-expanded", "false");
      }
      if (drop.hidden) return;
      if (drop.contains(ev.target)) return;
      if (openCombo && openCombo.contains(ev.target)) return;
      closeDrop();
    }, true);

    document.addEventListener("keydown", function (ev) {
      if (ev.key === "Escape") {
        closeDrop();
        helpCard.hidden = true;
        helpBtn.setAttribute("aria-expanded", "false");
      }
    });

    // drag the window by the top bar, mirroring Render()'s header drag zone
    var topbar = document.getElementById("topbar");
    var origin = { x: 0, y: 0 };
    topbar.addEventListener("pointerdown", function (ev) {
      if (ev.target.closest(".save, .iconbtn, .search")) return;
      ev.preventDefault();
      var sx = ev.clientX, sy = ev.clientY;
      var box = shell.getBoundingClientRect();
      var innerRect = stageInner.getBoundingClientRect();
      origin.x = (box.left - innerRect.left) / state.scale;
      origin.y = (box.top - innerRect.top) / state.scale;
      var move = function (e) {
        var dx = (e.clientX - sx) / state.scale;
        var dy = (e.clientY - sy) / state.scale;
        // EqClampMenuPos(): keep at least ~120px of the shell in the panel
        var nx = Math.max(120 - 880, Math.min(1156 - 120, origin.x + dx));
        var ny = Math.max(40 - 520, Math.min(520 - 40, origin.y + dy));
        shell.style.left = nx + "px";
        shell.style.top = ny + "px";
      };
      var up = function () {
        window.removeEventListener("pointermove", move);
        window.removeEventListener("pointerup", up);
      };
      window.addEventListener("pointermove", move);
      window.addEventListener("pointerup", up);
    });

    // playground
    document.getElementById("pgWall").addEventListener("click", function () {
      state.wall = !state.wall;
      shell.dataset.wall = state.wall ? "on" : "off";
    });
    document.getElementById("pgTheme").addEventListener("click", function () {
      state.dark = !state.dark;
      applyTheme();
    });
    document.getElementById("pgAccent").addEventListener("click", function () {
      state.accent = (state.accent + 1) % ACCENTS.length;
      applyAccent();
    });
    document.getElementById("pgAnim").addEventListener("click", function () {
      state.anim = state.anim === 1 ? 2 : (state.anim === 2 ? 0.5 : 1);
      applyAnim();
      renderPanelRows();
    });
  }

  /* -------------------------------- start ------------------------------- */

  function start() {
    wire();
    renderNav();
    renderPanelRows();
    dragPanel();
    renderPane();
    applySearch();
    applyAccent();
    applyAnim();
    applyTheme();
    layout();
    updateFps();
    window.setInterval(updateFps, 1100);
    window.addEventListener("resize", function () { layout(); positionDrop(); });
  }

  start();
})();
