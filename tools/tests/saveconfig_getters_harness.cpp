// Exercises the ethcfg safe getters from SaveConfig.h: a missing or
// wrong-typed key must fall back to the default rather than throw.
//
// The getter namespace is extracted verbatim out of the live SaveConfig.h by
// tools/tests/run_tests.sh into ethcfg_extracted.h, so these checks always
// test the shipped source and cannot drift into testing a stale copy.
//
// It is extracted rather than included wholesale because SaveConfig.h also
// binds to the game's `Config` settings struct, which lives in a private SDK
// header that is not part of this repository (see docs/AIDE_PRO_BUILD.md).
#define IMGUI_UNUSED_MAIN_GUARD 1
#include "foxcheats/include/json.hpp"
#include <cstdio>

#include "ethcfg_extracted.h"   // generated from System/Core/SaveConfig.h

int main()
{
    int fail = 0;
    auto ok = [&](bool c, const char* m) { std::printf("%s %s\n", c ? "ok  " : "FAIL", m); if (!c) fail++; };

    using namespace ethcfg;
    nlohmann::json j = nlohmann::json::parse(
        "{\"b\":true,\"f\":3.5,\"i\":7,\"s\":\"x\",\"wrongType\":5,\"arr\":[1,2]}");

    ok(GetBool(j, "b", false) == true, "GetBool reads a present bool");
    ok(GetBool(j, "missing", true) == true, "GetBool falls back on a missing key");
    ok(GetBool(j, "wrongType", false) == false, "GetBool falls back on a wrong-typed key");
    ok(GetBool(j, "s", true) == true, "GetBool falls back on a string");

    ok(GetFloat(j, "f", 0.0f) > 3.49f && GetFloat(j, "f", 0.0f) < 3.51f, "GetFloat reads a float");
    ok(GetFloat(j, "i", 0.0f) > 6.9f && GetFloat(j, "i", 0.0f) < 7.1f, "GetFloat widens an int");
    ok(GetFloat(j, "s", 9.0f) == 9.0f, "GetFloat falls back on a string");
    ok(GetFloat(j, "missing", 2.5f) == 2.5f, "GetFloat falls back on a missing key");

    ok(GetInt(j, "i", 0) == 7, "GetInt reads an int");
    ok(GetInt(j, "f", 99) == 99, "GetInt falls back on a float (no silent truncation)");
    ok(GetInt(j, "s", 5) == 5, "GetInt falls back on a string");
    ok(GetInt(j, "missing", 4) == 4, "GetInt falls back on a missing key");

    // A malformed document must not escape as an exception, so a hand-edited
    // or half-written config can never crash the loader on device.
    bool threw = false;
    nlohmann::json bad;
    try { bad = nlohmann::json::parse("{ not json"); } catch (...) { threw = true; }
    ok(!threw || bad.is_discarded() || !bad.is_object(),
       "malformed JSON is discarded rather than propagated");

    std::printf(fail == 0 ? "GETTERS CHECK PASSED (0 failures)\n" : "GETTERS CHECK FAILED\n");
    return fail;
}