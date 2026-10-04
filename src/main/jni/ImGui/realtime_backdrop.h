#pragma once

#include "imgui.h"

#include <GLES3/gl3.h>

namespace backdrop
{

    inline constexpr int kMaxDim = 480;

    inline GLuint g_realtimeBackdrop = 0;

    inline GLuint g_fbo   = 0;
    inline GLuint g_tex   = 0;
    inline int    g_texW  = 0;
    inline int    g_texH  = 0;
    inline GLuint g_fboB  = 0;
    inline GLuint g_texB  = 0;
    inline int    g_texBW = 0;
    inline int    g_texBH = 0;
    inline bool   g_ok    = false;
    inline bool   g_tried = false;

    inline GLuint make_target(int w, int h, GLuint& fbo, GLuint& tex)
    {
        glGenTextures(1, &tex);
        if (!tex) return 0;
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        glGenFramebuffers(1, &fbo);
        if (!fbo) { glDeleteTextures(1, &tex); tex = 0; return 0; }
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
        const bool ok = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (!ok)
        {
            glDeleteFramebuffers(1, &fbo); fbo = 0;
            glDeleteTextures(1, &tex);    tex = 0;
            return 0;
        }
        return tex;
    }

    inline void destroy()
    {
        if (g_fbo)  { glDeleteFramebuffers(1, &g_fbo);  g_fbo = 0; }
        if (g_tex)  { glDeleteTextures(1, &g_tex);      g_tex = 0; }
        if (g_fboB) { glDeleteFramebuffers(1, &g_fboB); g_fboB = 0; }
        if (g_texB) { glDeleteTextures(1, &g_texB);     g_texB = 0; }
        g_texW = g_texH = 0;
        g_texBW = g_texBH = 0;
        g_realtimeBackdrop = 0;
        g_ok = false;
    }

    inline GLuint update(int screenW, int screenH)
    {
        if (screenW <= 0 || screenH <= 0)
            return 0;
        if (g_tried && !g_ok)
            return 0;

        while (glGetError() != GL_NO_ERROR) { }

        const int longest = (screenW > screenH) ? screenW : screenH;
        const float k = (longest > kMaxDim) ? (float)kMaxDim / (float)longest : 1.0f;
        int tw = (int)(screenW * k + 0.5f); if (tw < 16) tw = 16;
        int th = (int)(screenH * k + 0.5f); if (th < 16) th = 16;

        if (!g_tex || tw != g_texW || th != g_texH || !g_texB)
        {
            destroy();

            if (!make_target(tw, th, g_fbo, g_tex)) { destroy(); g_tried = true; return 0; }

            // Second stage: a very small target. Sampling it with GL_LINEAR and
            // letting the panel stretch it gives a wide, cheap, genuinely frosted
            // blur - the same trick iOS uses for its material. Two GL blits per
            // frame, no shader needed.
            int bw = tw / 8; if (bw < 4) bw = 4;
            int bh = th / 8; if (bh < 4) bh = 4;
            if (!make_target(bw, bh, g_fboB, g_texB)) { destroy(); g_tried = true; return 0; }

            g_texW = tw;   g_texH = th;
            g_texBW = bw; g_texBH = bh;
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_fbo);
        glBlitFramebuffer(0, 0, screenW, screenH,
                          0, 0, tw, th,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, g_fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_fboB);
        glBlitFramebuffer(0, 0, tw, th,
                          0, 0, g_texBW, g_texBH,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (glGetError() != GL_NO_ERROR)
        {
            destroy();
            g_tried = true;
            return 0;
        }

        g_ok = true;
        g_realtimeBackdrop = g_texB;
        return g_texB;
    }
}
