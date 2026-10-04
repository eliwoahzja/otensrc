/* ============================================================================
   Equinox glass menu — site preview behaviour.

   Mirrors src/main/jni/ImGui/equinox_menu.h: the same 780x480 shell, six tabs,
   search filter, damped pill, combo popups that flip above the row, and the
   traffic lights (collapse / jump to VISUALS / swap the backdrop).
   ========================================================================== */
(function () {
  "use strict";

  /* ------------------------------- icons -------------------------------- */

  var ICONS = {
    eye: '<path d="M1.3 8S3.8 3.2 8 3.2 14.7 8 14.7 8 12.2 12.8 8 12.8 1.3 8 1.3 8z"/><circle cx="8" cy="8" r="2.5"/>',
    crosshair: '<circle cx="8" cy="8" r="5.1"/><path d="M8 0.6v3M8 12.4v3M0.6 8h3M12.4 8h3"/>',
    bolt: '<path d="M9.4 1 3.4 9h3.3l-1.1 6 6-8H8.3z"/>',
    list: '<path d="M5.4 3.4h9.2M5.4 8h9.2M5.4 12.6h9.2"/><path d="M1.9 3.4h.01M1.9 8h.01M1.9 12.6h.01" stroke-linecap="round" stroke-width="2"/>',
    sliders: '<path d="M2 4.2h12M2 11.8h12"/><circle cx="5.4" cy="4.2" r="1.7"/><circle cx="10.6" cy="11.8" r="1.7"/>',
    cog: '<circle cx="8" cy="8" r="3"/><path d="M8 1.1v2M8 12.9v2M1.1 8h2M12.9 8h2M3.1 3.1l1.4 1.4M11.5 11.5l1.4 1.4M12.9 3.1l-1.4 1.4M4.5 11.5 3.1 12.9"/>',
    search: '<circle cx="7" cy="7" r="4.6"/><path d="M10.4 10.4 14 14"/>',
    times: '<path d="M2 2l10 10M12 2 2 12"/>',
    check: '<path d="M2 8.4 6 12.4 14 3.6"/>',
    gauge: '<path d="M1.6 11.3a6.4 6.4 0 1 1 12.8 0"/><path d="M8 11.3 11.2 6"/>',
    shield: '<path d="M8 1.4l5.4 2v4.1c0 3.2-2.2 5.8-5.4 7.1-3.2-1.3-5.4-3.9-5.4-7.1V3.4z"/>',
    clock: '<circle cx="8" cy="8" r="6"/><path d="M8 4.4V8l2.6 1.6"/>',
    tag: '<path d="M2 7.2V2.6h4.6L14 10l-4.4 4.4z"/><circle cx="4.9" cy="4.9" r="1"/>'
  };

  function icon(name, cls) {
    return '<span class="' + (cls || "ricon") + '" aria-hidden="true"><svg viewBox="0 0 16 16" fill="none" ' +
      'stroke="currentColor" stroke-width="1.4" stroke-linecap="round" stroke-linejoin="round">' +
      (ICONS[name] || ICONS.eye) + "</svg></span>";
  }

  function decorateIcons(root) {
    var nodes = root.querySelectorAll("[data-icon]");
    for (var i = 0; i < nodes.length; i++) {
      var el = nodes[i];
      el.innerHTML = '<svg viewBox="0 0 16 16" fill="none" stroke="currentColor" stroke-width="1.5" ' +
        'stroke-linecap="round" stroke-linejoin="round">' + (ICONS[el.dataset.icon] || ICONS.eye) + "</svg>";
    }
  }

  /* -------------------------------- data -------------------------------- */

  var TABS = [
    { icon: "eye", label: "VISUALS" },
    { icon: "crosshair", label: "AIMBOT" },
    { icon: "bolt", label: "MEMORY" },
    { icon: "list", label: "SKINS" },
    { icon: "sliders", label: "MISC" },
    { icon: "cog", label: "SETTINGS" }
  ];

  var QUICK = [
    { tab: 1, icon: "crosshair", title: "Aimbot" },
    { tab: 0, icon: "eye", title: "Visuals" },
    { tab: 3, icon: "list", title: "Skins" },
    { tab: 5, icon: "cog", title: "Settings" }
  ];

  function toggle(label, on, iconName) { return { type: "toggle", label: label, on: !!on, icon: iconName }; }
  function slider(label, value, min, max, fmt, iconName) {
    return { type: "slider", label: label, value: value, min: min, max: max, fmt: fmt, icon: iconName };
  }
  function combo(label, options, value, iconName) {
    return { type: "combo", label: label, options: options, value: value, icon: iconName };
  }

  /* Tab content copied from runtime_preview_menu.h / Main.cpp where the overlay
     defines it; the custom cards (misc, settings) use their real section titles
     with representative rows. */
  var CONTENT = [
    [ /* 0 VISUALS */
      { label: "ESP", card: [
        toggle("ESP Line", true, "eye"), toggle("ESP Box", true, "eye"),
        toggle("ESP Skeleton", true, "eye"), toggle("ESP Health", true, "eye"),
        toggle("ESP Name", true, "eye"), toggle("ESP Distance", true, "eye"),
        toggle("ESP Count", false, "eye"), toggle("360 Alert", false, "eye"),
        toggle("Show AimLine", false, "eye"),
        toggle("Yellow Wallhack", true, "eye"), toggle("Red Wallhack", false, "eye")
      ] },
      { label: "ESP OPTIONS", card: [
        combo("Box Type", ["Fill", "Outline", "Corner", "3D"], 0, "sliders"),
        combo("Line Position", ["Top", "Mid", "Bottom"], 0, "sliders"),
        combo("Health Position", ["Top", "Side"], 0, "sliders"),
        combo("ESP Style", ["None", "3D Sphere", "Player Signal"], 0, "sliders"),
        { type: "color", label: "Player ESP Color", color: "#00d4ff", icon: "eye" },
        { type: "color", label: "Bot ESP Color", color: "#00d278", icon: "eye" }
      ] }
    ],
    [ /* 1 AIMBOT */
      { label: "AIMBOTS", card: [
        toggle("Aimbot 360", true, "crosshair"),
        toggle("Bullet Track", true, "crosshair"),
        slider("Aim Assist Size", 0, 0, 100, "%.0f", "crosshair")
      ] },
      { label: "COMBAT OPTIONS", card: [
        combo("Location", ["Head", "Chest", "Body"], 0, "crosshair"),
        combo("Trigger", ["None", "Shooting", "Scoping"], 0, "crosshair"),
        combo("Target By", ["Distance", "FOV"], 0, "crosshair"),
        slider("FOV Size", 45, 0, 100, "%.0f", "crosshair")
      ] }
    ],
    [ /* 2 MEMORY */
      { label: "MEMORY HACKS", card: [
        toggle("Unlock Blueprint", true, "bolt"), toggle("Hitbox", true, "bolt"),
        toggle("No Recoil", true, "bolt"), toggle("No Spread", true, "bolt"),
        toggle("No Shake", false, "bolt"), toggle("No Overheat", false, "bolt"),
        toggle("No Parachute", false, "bolt"), toggle("Anti Flashbang", false, "bolt"),
        toggle("Firerate", true, "bolt"), toggle("Fast Dive", false, "bolt"),
        toggle("Fast Reload", true, "bolt"), toggle("Fast Scope", false, "bolt"),
        toggle("Quick Switch", false, "bolt"), toggle("Weapon Kinetic", false, "bolt")
      ] },
      { label: "MOVEMENT SETTINGS", card: [
        slider("Snowboard Speed", 0, 0, 100, "%.1f", "sliders"),
        slider("Slide Distance", 6, 0, 30, "%.1f", "sliders"),
        slider("SpeedHack", 1, 0.5, 2, "%.1fx", "sliders"),
        slider("High Jump", 1, 0.5, 5, "%.2fx", "sliders")
      ] }
    ],
    [ /* 3 SKINS */
      { type: "chiptabs", items: ["Character", "Watch", "Deadbox", "Plane", "Weapon", "Camo"], active: 4 },
      { label: "WEAPON SKINS", card: [
        { type: "kv", label: "AK117", value: "Default" },
        { type: "kv", label: "QQ9", value: "Default" },
        { type: "kv", label: "DL Q33", value: "Default" },
        { type: "kv", label: "Kilo Bolt-Action", value: "Default" },
        { type: "kv", label: "PDW-57", value: "Default" }
      ] },
      { label: "CAMO MODIFIER", card: [
        toggle("Default / OFF", true),
        toggle("Diamond Camo", false),
        toggle("Red Sprite Camo", false),
        { type: "note", text: "Only applies to [M] Mythic and [L] Legendary weapon skins." }
      ] }
    ],
    [ /* 4 MISC */
      { label: "CHANGELOG", card: [
        { type: "chiptabs", items: ["FEATURES", "FIXES", "UPDATES"], active: 0 },
        { type: "date", text: "March 10, 2026" },
        { type: "bullet", icon: "shield", text: "<b>New glass menu shell</b> — animated tabs, damped accent pill and a live search filter." },
        { type: "bullet", icon: "bolt", text: "<b>ESP styles</b> — 3D sphere and player signal added to the ESP Style combo." },
        { type: "bullet", icon: "clock", text: "<b>Faster injection</b> — the loader now reuses the cached lib when it is unchanged." }
      ] },
      { label: "INFO", card: [
        { type: "kv", label: "Version", value: "v1.0.87" },
        { type: "kv", label: "Loader", value: "Online" },
        { type: "kv", label: "Build", value: "2026" }
      ] },
      { label: "PRICELIST", card: [
        { type: "kv", label: "1 Day", value: "\u20b150 / $1" },
        { type: "kv", label: "1 Week", value: "\u20b1250 / $5" },
        { type: "kv", label: "1 Month", value: "\u20b1600 / $12" },
        { type: "kv", label: "Lifetime", value: "\u20b11200 / $24" }
      ] }
    ],
    [ /* 5 SETTINGS */
      { type: "cardgrid", cards: [
        { label: "LICENSE INFO", card: [
          { type: "kv", label: "Licensed to", value: "White Crowns" },
          { type: "kv", label: "Plan", value: "Lifetime" },
          { type: "kv", label: "Expires", value: "Never" }
        ] },
        { label: "LOGO SETTINGS", card: [
          slider("Logo Size", 1, 0.1, 2, "%.2fx", "tag"),
          slider("Logo Opacity", 1, 0, 1, "%.2f", "eye"),
          toggle("Show Logo", true, "tag")
        ] },
        { label: "CONFIG MANAGEMENT", card: [
          { type: "buttons", items: ["SAVE", "LOAD", "RESET"] }
        ] },
        { label: "ENHANCEMENT", card: [
          toggle("Streamer Mode", false, "shield"),
          toggle("Low Latency", true, "bolt"),
          slider("UI Scale", 1, 0.8, 1.4, "%.2fx", "sliders")
        ] }
      ] }
    ]
  ];

  /* ------------------------------ elements ------------------------------ */

  var stage = document.getElementById("stage");
  var shell = document.getElementById("shell");
  var scene = document.getElementById("scene");
  var tabsEl = document.getElementById("tabs");
  var pillEl = document.getElementById("tabPill");
  var inner = document.getElementById("contentInner");
  var contentEl = document.getElementById("content");
  var noMatchEl = document.getElementById("noMatch");
  var dropEl = document.getElementById("drop");
  var searchEl = document.getElementById("searchInput");
  var clearEl = document.getElementById("searchClear");
  var fpsText = document.getElementById("fpsText");
  var fpsIcon = document.querySelector(".fps-icon");
  var omni = document.getElementById("omni");
  var omniFps = document.getElementById("omniFps");

  var state = { tab: 0, search: "", scale: 1, ox: 0, oy: 0, collapsed: false };
  var openCombo = null;

  /* ------------------------------- layout ------------------------------- */

  function layout() {
    var s = Math.min(1.7, Math.max(0.62, (window.innerWidth - 80) / 780));
    state.scale = s;
    stage.style.width = 780 * s + "px";
    stage.style.height = 480 * s + "px";
    shell.style.transform = "scale(" + s + ")";
    place();
  }

  function place() {
    shell.style.left = state.ox * state.scale + "px";
    shell.style.top = state.oy * state.scale + "px";
  }

  /* ------------------------------- sidebar ------------------------------ */

  function renderTabs() {
    tabsEl.innerHTML = "";
    TABS.forEach(function (tab, i) {
      var b = document.createElement("button");
      b.type = "button";
      b.className = "tab" + (i === state.tab ? " active" : "");
      b.innerHTML = '<svg viewBox="0 0 16 16" fill="none" stroke="currentColor" stroke-width="1.4" ' +
        'stroke-linecap="round" stroke-linejoin="round">' + ICONS[tab.icon] + "</svg><span>" + tab.label + "</span>";
      b.addEventListener("click", function () { setTab(i); });
      tabsEl.appendChild(b);
    });
    pillEl.style.top = 89 + 36 * state.tab + "px";
  }

  function setTab(i) {
    if (state.tab === i) return;
    state.tab = i;
    var kids = tabsEl.children;
    for (var k = 0; k < kids.length; k++) kids[k].classList.toggle("active", k === i);
    pillEl.style.top = 89 + 36 * i + "px";
    closeDrop();
    renderContent();
    applySearch();
    contentEl.scrollTop = 0;
  }

  /* ------------------------------- content ------------------------------ */

  function cardEl(block, wide) {
    var wrap = document.createElement("div");
    var card = document.createElement("div");
    card.className = "card";
    if (block.label) {
      var h = document.createElement("h4");
      h.className = "card-title";
      h.textContent = block.label;
      wrap.appendChild(h);
    }
    wrap.appendChild(card);
    wrap.className = "block" + (wide ? " wide" : "");
    return { wrap: wrap, card: card };
  }

  function renderContent() {
    inner.innerHTML = "";
    inner.hidden = false;
    noMatchEl.hidden = true;
    closeDrop();
    var blocks = CONTENT[state.tab];

    blocks.forEach(function (block) {
      if (block.type === "chiptabs") {
        var strip = document.createElement("div");
        strip.className = "chiprow";
        block.items.forEach(function (item, i) {
          var b = document.createElement("button");
          b.type = "button";
          b.className = "chipbtn" + (i === block.active ? " active" : "");
          b.textContent = item;
          b.addEventListener("click", function () {
            block.active = i;
            var sibs = strip.children;
            for (var s = 0; s < sibs.length; s++) sibs[s].classList.toggle("active", s === i);
          });
          strip.appendChild(b);
        });
        inner.appendChild(strip);
        return;
      }

      if (block.type === "cardgrid") {
        var grid = document.createElement("div");
        grid.className = "cardgrid";
        block.cards.forEach(function (sub) {
          var parts = cardEl(sub, true);
          sub.card.forEach(function (row) { parts.card.appendChild(renderRow(row)); });
          grid.appendChild(parts.wrap);
        });
        inner.appendChild(grid);
        return;
      }

      var made = cardEl(block);
      block.card.forEach(function (row) { made.card.appendChild(renderRow(row)); });
      inner.appendChild(made.wrap);
    });

    // fade the pane in, the way Render() fades content on a tab switch
    inner.classList.add("fade");
    void inner.offsetWidth;   // commit the faded state so the transition runs
    window.requestAnimationFrame(function () { inner.classList.remove("fade"); });
  }

  function fmtValue(fmt, v) {
    if (fmt === "%.0f") return String(Math.round(v));
    if (fmt === "%.1f") return v.toFixed(1);
    if (fmt === "%.2f") return v.toFixed(2);
    if (fmt === "%.1fx") return v.toFixed(1) + "x";
    if (fmt === "%.2fx") return v.toFixed(2) + "x";
    return String(v);
  }

  function renderRow(row) {
    var el;
    if (row.type === "toggle") {
      el = document.createElement("div");
      el.className = "row toggle" + (row.on ? " on" : "");
      el.innerHTML = (row.icon ? icon(row.icon) : "") +
        '<span class="rlabel">' + row.label + "</span>" +
        '<span class="switch"><span class="knob"></span></span>';
      el.addEventListener("click", function () {
        row.on = !row.on;
        el.classList.toggle("on", row.on);
      });
    } else if (row.type === "slider") {
      el = document.createElement("div");
      el.className = "row slider";
      el.innerHTML = '<div class="slabel">' + (row.icon ? icon(row.icon) : "") +
        "<span>" + row.label + "</span></div>" +
        '<span class="spill"></span>' +
        '<span class="strack"><span class="sfill"></span><span class="sknob"></span></span>';
      var track = el.querySelector(".strack");
      var fill = el.querySelector(".sfill");
      var knob = el.querySelector(".sknob");
      var spill = el.querySelector(".spill");
      var paint = function () {
        var t = (row.value - row.min) / (row.max - row.min);
        t = Math.max(0, Math.min(1, t));
        spill.textContent = fmtValue(row.fmt, row.value);
        fill.style.width = t * 100 + "%";
        knob.style.left = t * track.clientWidth + "px";
      };
      var setFromX = function (clientX) {
        var r = track.getBoundingClientRect();
        var t = Math.max(0, Math.min(1, (clientX - r.left) / (r.width || 1)));
        row.value = row.min + t * (row.max - row.min);
        paint();
      };
      el.addEventListener("pointerdown", function (ev) {
        ev.preventDefault();
        el.classList.add("active");
        setFromX(ev.clientX);
        var move = function (e) { setFromX(e.clientX); };
        var up = function () {
          el.classList.remove("active");
          window.removeEventListener("pointermove", move);
          window.removeEventListener("pointerup", up);
        };
        window.addEventListener("pointermove", move);
        window.addEventListener("pointerup", up);
      });
      requestAnimationFrame(paint);
      el._paint = paint;
    } else if (row.type === "combo") {
      el = document.createElement("div");
      el.className = "row combo";
      el.innerHTML = (row.icon ? icon(row.icon) : "") +
        '<span class="rlabel">' + row.label + "</span>" +
        '<span class="cvalue">' + row.options[row.value] + "</span>" +
        '<span class="chev"><svg viewBox="0 0 8 8" fill="none" stroke="currentColor" stroke-width="1.4" ' +
        'stroke-linecap="round"><path d="M1 2.6 4 5.4 7 2.6"/></svg></span>';
      el.addEventListener("click", function () { openDrop(row, el); });
    } else if (row.type === "color") {
      el = document.createElement("div");
      el.className = "row color";
      el.innerHTML = (row.icon ? icon(row.icon) : "") +
        '<span class="rlabel">' + row.label + "</span>" +
        '<span class="chex"></span><span class="chip"></span>';
      var PALETTE = ["#00d4ff", "#00d278", "#ff3c50", "#ffc832", "#bf38ff", "#ffffff"];
      var paintColor = function () {
        el.querySelector(".chip").style.background = row.color;
        el.querySelector(".chex").textContent = row.color.toUpperCase();
      };
      el.addEventListener("click", function () {
        var i = (PALETTE.indexOf(row.color) + 1) % PALETTE.length;
        row.color = PALETTE[i];
        paintColor();
      });
      paintColor();
    } else if (row.type === "kv") {
      el = document.createElement("div");
      el.className = "row kv";
      el.innerHTML = "<span>" + row.label + "</span><b>" + row.value + "</b>";
      el._filterable = false;
    } else if (row.type === "bullet") {
      el = document.createElement("div");
      el.className = "row bullet";
      el.innerHTML = icon(row.icon) + "<span>" + row.text + "</span>";
      el._filterable = false;
    } else if (row.type === "buttons") {
      el = document.createElement("div");
      el.className = "row btnrow";
      row.items.forEach(function (label) {
        var b = document.createElement("button");
        b.type = "button";
        b.className = "minibtn";
        b.textContent = label;
        b.addEventListener("click", function () {
          b.textContent = label === "RESET" ? "RESET" : "OK";
          window.setTimeout(function () { b.textContent = label; }, 900);
        });
        el.appendChild(b);
      });
      el._filterable = false;
    } else if (row.type === "note") {
      el = document.createElement("div");
      el.className = "row note";
      el.textContent = row.text;
      el._filterable = false;
    } else { /* date */
      el = document.createElement("div");
      el.className = "row kv";
      el.innerHTML = "<span>" + row.text + "</span>";
      el._filterable = false;
    }
    el._row = row;
    if (el._filterable === undefined) el._filterable = (row.type === "toggle" || row.type === "slider" || row.type === "combo");
    return el;
  }

  /* ------------------------------- search ------------------------------- */

  function applySearch() {
    var q = state.search.trim().toLowerCase();
    var filtering = q.length > 0;
    clearEl.hidden = !filtering;
    var queryTabs = state.tab >= 0 && state.tab <= 3;   // RowToggle/RowSlider/ComboRow tabs
    // Mirrors FrameSearchHit / LastSearchHit: a tab only counts as "has hits"
    // when one of the EqPassFilter rows matched, so the placeholder replaces the
    // whole pane - unfiltered custom rows and all.
    var anyMatch = !filtering;

    var blocks = inner.querySelectorAll(".block");
    for (var b = 0; b < blocks.length; b++) {
      var rows = blocks[b].querySelectorAll(".row");
      var shownHere = 0;
      for (var i = 0; i < rows.length; i++) {
        var row = rows[i]._row;
        var show = true;
        if (filtering && queryTabs && rows[i]._filterable) {
          show = row && row.label ? row.label.toLowerCase().indexOf(q) !== -1 : true;
          if (show) anyMatch = true;
        }
        rows[i].hidden = !show;
        if (show) shownHere++;
      }
      blocks[b].hidden = shownHere === 0;
    }

    var showNoMatch = filtering && queryTabs && !anyMatch;
    noMatchEl.hidden = !showNoMatch;
    inner.hidden = showNoMatch;
    if (showNoMatch) closeDrop();

    // sliders re-paint after being re-shown (their width was 0 while hidden)
    var sliders = inner.querySelectorAll(".row.slider");
    for (var s = 0; s < sliders.length; s++) {
      if (sliders[s]._paint) sliders[s]._paint();
    }
  }

  /* ------------------------------- dropdown ------------------------------ */

  function closeDrop() {
    dropEl.hidden = true;
    dropEl.innerHTML = "";
    if (openCombo) openCombo.classList.remove("open");
    openCombo = null;
  }

  function openDrop(row, rowEl) {
    if (openCombo === rowEl) { closeDrop(); return; }
    closeDrop();
    openCombo = rowEl;
    rowEl.classList.add("open");
    dropEl.innerHTML = "";

    row.options.forEach(function (opt, i) {
      var d = document.createElement("div");
      d.className = "di" + (i === row.value ? " sel" : "");
      d.innerHTML = "<span>" + opt + "</span>" +
        (i === row.value ? '<span class="check"><svg viewBox="0 0 16 16" fill="none" stroke="currentColor" ' +
          'stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round">' + ICONS.check + "</svg></span>" : "");
      d.addEventListener("click", function () {
        row.value = i;
        var value = rowEl.querySelector(".cvalue");
        if (value) value.textContent = opt;
        closeDrop();
      });
      dropEl.appendChild(d);
    });
    dropEl.hidden = false;
    positionDrop();
  }

  /* Anchored to the row every frame, the way ComboRow() sets the popup position
     from the row rect, so scrolling the pane moves the panel with its row
     instead of dismissing it. */
  function positionDrop() {
    if (!openCombo || !openCombo._row) return;
    var pane = contentEl.getBoundingClientRect();
    var r = openCombo.getBoundingClientRect();
    if (r.bottom < pane.top - 1 || r.top > pane.bottom + 1) { closeDrop(); return; }

    var box = shell.getBoundingClientRect();
    var s = box.width / 780;
    var x = (r.left - box.left) / s;
    var w = r.width / s;
    var itemH = 26, pad = 6;
    var h = openCombo._row.options.length * itemH + pad * 2 + 2;
    var y = (r.bottom - box.top) / s + 4;
    if (y + h > 466) y = (r.top - box.top) / s - h - 4;   // flip above near the bottom

    dropEl.style.left = Math.max(212, Math.min(x, 766 - w)) + "px";
    dropEl.style.top = y + "px";
    dropEl.style.width = w + "px";
  }

  /* ------------------------------- header -------------------------------- */

  function renderHeader() {
    var quick = document.getElementById("quick");
    quick.innerHTML = "";
    QUICK.forEach(function (q) {
      var b = document.createElement("button");
      b.type = "button";
      b.className = "qbtn";
      b.title = q.title;
      b.innerHTML = '<svg viewBox="0 0 16 16" fill="none" stroke="currentColor" stroke-width="1.4" ' +
        'stroke-linecap="round" stroke-linejoin="round">' + ICONS[q.icon] + "</svg>";
      b.addEventListener("click", function () { setTab(q.tab); });
      quick.appendChild(b);
    });

    var traffic = document.getElementById("traffic");
    traffic.innerHTML = "";
    [{ t: "Collapse" }, { t: "Jump to VISUALS" }, { t: "Swap backdrop" }].forEach(function (spec, i) {
      var b = document.createElement("button");
      b.type = "button";
      b.className = "tdot";
      b.dataset.light = i;
      b.title = spec.t;
      b.innerHTML = "<i></i>";
      b.addEventListener("click", function () { trafficLight(i); });
      traffic.appendChild(b);
    });
  }

  function trafficLight(i) {
    if (i === 0) {
      state.collapsed = true;
      shell.hidden = true;
      omni.hidden = false;
      closeDrop();
    } else if (i === 1) {
      setTab(0);
    } else {
      scene.classList.toggle("light");
    }
  }

  function updateFps() {
    var fps = 55 + Math.round(Math.random() * 7);
    fpsText.textContent = "FPS " + fps;
    omniFps.textContent = String(fps);
    fpsIcon.classList.toggle("warn", fps < 55 && fps >= 30);
    fpsIcon.classList.toggle("bad", fps < 30);
  }

  /* ------------------------------ interactions --------------------------- */

  searchEl.addEventListener("input", function () {
    state.search = searchEl.value;
    applySearch();
  });

  clearEl.addEventListener("click", function () {
    searchEl.value = "";
    state.search = "";
    searchEl.focus();
    applySearch();
  });

  omni.addEventListener("click", function () {
    state.collapsed = false;
    omni.hidden = true;
    shell.hidden = false;
  });

  contentEl.addEventListener("scroll", positionDrop);

  document.addEventListener("pointerdown", function (ev) {
    if (dropEl.hidden) return;
    if (dropEl.contains(ev.target)) return;
    if (openCombo && openCombo.contains(ev.target)) return;
    closeDrop();
  }, true);

  document.addEventListener("keydown", function (ev) {
    if (ev.key === "Escape") closeDrop();
  });

  // drag the window by the header strip, the way Render() drags on h0..h1
  var head = document.getElementById("head");
  head.addEventListener("pointerdown", function (ev) {
    if (ev.target.closest(".search, .qbtn, .tdot")) return;
    ev.preventDefault();
    var startX = ev.clientX, startY = ev.clientY;
    var ox = state.ox, oy = state.oy;
    var move = function (e) {
      var dx = (e.clientX - startX) / state.scale;
      var dy = (e.clientY - startY) / state.scale;
      // EqClampMenuPos(): keep at least ~120px of the 780px shell in the panel
      state.ox = Math.max(120 - 780, Math.min(660, ox + dx));
      state.oy = Math.max(48, Math.min(432, oy + dy));
      place();
    };
    var up = function () {
      window.removeEventListener("pointermove", move);
      window.removeEventListener("pointerup", up);
    };
    window.addEventListener("pointermove", move);
    window.addEventListener("pointerup", up);
  });

  window.addEventListener("resize", layout);

  /* -------------------------------- start -------------------------------- */

  decorateIcons(document);
  renderTabs();
  renderHeader();
  renderContent();
  layout();
  updateFps();
  window.setInterval(updateFps, 1100);
})();
