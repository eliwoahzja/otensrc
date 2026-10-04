// Compile-shape regression check for the ethnir menu's public API.
//
// This file reproduces the translation-unit shape used by the tab bodies in
// runtime_preview_menu.h: `using namespace ImGui;` together with
// `using namespace ethnir;` in the same scope. Under that combination an
// unqualified NextColumn()/EndColumns() is ambiguous, because ImGui declares
// its own legacy column API in namespace ImGui. That is not a hypothetical --
// it produced 79 "call to 'NextColumn' is ambiguous" NDK errors on device.
//
// The menu's column helpers therefore carry an Eq* prefix. This TU must keep
// compiling; if anyone renames them back to BeginColumns/NextColumn/EndColumns,
// the build below fails and the menu no longer builds on device.
//
// It is compiled, not run.

#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/ethnir_menu.h"

// The exact combination that broke the NDK build.
using namespace ImGui;
using namespace ethnir;

// The direct guard: with both namespaces imported, an unqualified column call
// must resolve to exactly one function. Today that is ImGui::NextColumn.
// If a colliding ethnir::NextColumn is ever reintroduced, this line becomes
// "call to 'NextColumn' is ambiguous" and the build stops -- which is the
// device failure we are preventing. Never called; this TU is compile-only.
static void ProbeUnqualifiedColumnNames()
{
    NextColumn();
    EndColumns();
    Render();          // ImGui::Render() must stay the only candidate
}

static void TabBody()
{
    EqBeginColumns();
    SectionLabel("COLUMN A");
    BeginGroupCard("eth_shape_a");
    static bool flag = false;
    RowToggle(nullptr, "Flag", &flag);
    EndGroupCard();
    EqNextColumn();
    SectionLabel("COLUMN B");
    BeginGroupCard("eth_shape_b");
    EqEndColumns();
}

// Also pin the rest of the exported surface so a rename cannot silently break
// the tab bodies either.
static void OtherSurface()
{
    static float value = 1.0f;
    static int    choice = 0;
    static float  rgba[4] = { 0.0f, 212.0f, 255.0f, 255.0f };
    static int    bind = 0;
    static const char* options[] = { "A", "B" };
    static const char* binds[] = { "F1", "None" };

    RowSlider(nullptr, "Speed", &value, 0.0f, 2.0f, "%.2fx");
    ComboRow(nullptr, "Mode", &choice, options, 2);
    ColorRow(nullptr, "Colour", rgba);
    KeybindRow("Menu bind", &bind, binds, 2);
    SegmentedRow("Theme", &choice, options, 2);
    EqPassFilter("Speed");
    char clean[64];
    EqStripId("Speed##id", clean, (int)sizeof(clean));
}

// Referencing them explicitly keeps the compiler from warning about the
// unused statics while still requiring the declarations to exist.
int main()
{
    (void)&TabBody;
    (void)&OtherSurface;
    (void)&ProbeUnqualifiedColumnNames;
    return 0;
}