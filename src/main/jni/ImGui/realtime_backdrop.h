#pragma once
// Real-time game-frame backdrop for the liquid-glass shell.
//
// Why an FBO instead of glCopyTexSubImage2D() straight off the default
// framebuffer: this runs inside the eglSwapBuffers hook, so the framebuffer we
// would read is the very same one ImGui is about to draw into. Sampling and
// rendering the same render target in one frame is undefined in GL and shows up
// as garbage or a driver crash on some devices. So we blit into a separate
// FBO-backed texture first and only ever sample that.
//
// The blit target is downscaled: the glass panel is heavily blurred and tinted,
// so full resolution buys nothing and costs a lot of fill rate.
//
// If any step fails we flip g_backdrop_ok to false and the shell falls back to
// the pre-baked wallpaper, so a driver that hates this path degrades instead of
// crashing.

#include "imgui.h"

#include <GLES3/gl3.h>   // not reachable from Main.cpp's include chain

namespace backdrop
{
    // Longest edge of the downscaled capture, in pixels.
    inline constexpr int kMaxDim = 480;

    // Capture texture handed to the shell each frame; 0 means "unavailable,
    // fall back to the pre-baked wallpaper". Inline so every translation unit
    // shares one object and a static/extern mismatch is impossible.
    inline GLuint g_realtimeBackdrop = 0;

    inline GLuint g_fbo   = 0;
    inline GLuint g_tex   = 0;
    inline int    g_texW  = 0;
    inline int    g_texH  = 0;
    inline bool   g_ok    = false;   // true once the FBO path is known good
    inline bool   g_tried = false;   // one-shot: stop retrying after a failure

    inline void destroy()
    {
        if (g_fbo) { glDeleteFramebuffers(1, &g_fbo); g_fbo = 0; }
        if (g_tex) { glDeleteTextures(1, &g_tex);     g_tex = 0; }
        g_texW = g_texH = 0;
        g_realtimeBackdrop = 0;   // never hand out a deleted texture id
        g_ok = false;
    }

    // Returns the capture texture, or 0 when unavailable (caller falls back).
    inline GLuint update(int screenW, int screenH)
    {
        if (screenW <= 0 || screenH <= 0)
            return 0;
        if (g_tried && !g_ok)
            return 0;

        // Drain any error the game left behind. Otherwise glGetError() at the
        // bottom can report someone else's failure and permanently disable our
        // path (g_tried is one-shot).
        while (glGetError() != GL_NO_ERROR) { }

        // Downscale factor: fit the screen inside kMaxDim on its longest edge.
        const int longest = (screenW > screenH) ? screenW : screenH;
        const float k = (longest > kMaxDim) ? (float)kMaxDim / (float)longest : 1.0f;
        int tw = (int)(screenW * k + 0.5f); if (tw < 16) tw = 16;
        int th = (int)(screenH * k + 0.5f); if (th < 16) th = 16;

        // ---- (re)allocate on first use or on rotation/resize ----
        if (!g_tex || tw != g_texW || th != g_texH)
        {
            destroy();

            glGenTextures(1, &g_tex);
            if (!g_tex) { g_tried = true; return 0; }
            glBindTexture(GL_TEXTURE_2D, g_tex);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

            glGenFramebuffers(1, &g_fbo);
            if (!g_fbo) { destroy(); g_tried = true; return 0; }
            glBindFramebuffer(GL_FRAMEBUFFER, g_fbo);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_tex, 0);

            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                destroy();
                g_tried = true;
                return 0;
            }
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            g_texW = tw;
            g_texH = th;
        }

        // ---- blit the freshly rendered game frame into our own FBO ----
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_fbo);
        glBlitFramebuffer(0, 0, screenW, screenH,
                          0, 0, tw, th,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);

        // Leave the game's own framebuffer bound for ImGui.
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (glGetError() != GL_NO_ERROR)
        {
            destroy();
            g_tried = true;
            return 0;
        }

        g_ok = true;
        g_realtimeBackdrop = g_tex;
        return g_tex;
    }
}
