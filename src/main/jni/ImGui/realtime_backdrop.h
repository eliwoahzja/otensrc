#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <GLES3/gl3.h>
#include <android/log.h>

namespace backdrop
{
    inline GLuint g_realtimeBackdrop = 0;

    inline constexpr int kMaxDim = 720;

    inline GLuint g_fbo = 0, g_fboB = 0, g_texA = 0, g_texB = 0;
    inline int g_w = 0, g_h = 0;
    inline bool g_ok = false, g_tried = false;

    inline const char* kBlurVert =
        "#version 300 es\n"
        "uniform vec4 u_rect;\n"
        "uniform vec2 u_screen;\n"
        "out vec2 v_uv;\n"
        "void main()\n"
        "{\n"
        "    vec2 c = vec2(float((gl_VertexID & 1) == 0), float((gl_VertexID & 2) == 0));\n"
        "    v_uv = c;\n"
        "    vec2 px = u_rect.xy + c * u_rect.zw;\n"
        "    gl_Position = vec4(px.x / u_screen.x * 2.0 - 1.0,\n"
        "                        1.0 - px.y / u_screen.y * 2.0, 0.0, 1.0);\n"
        "}\n";

    inline const char* kBlurFrag =
        "#version 300 es\n"
        "precision highp float;\n"
        "uniform sampler2D u_src;\n"
        "uniform vec2 u_dir;\n"
        "in vec2 v_uv;\n"
        "out vec4 fragColor;\n"
        "const float W0 = 0.2270270270;\n"
        "const float W1 = 0.1945945946;\n"
        "const float W2 = 0.1216216216;\n"
        "const float W3 = 0.0540540541;\n"
        "const float W4 = 0.0162162162;\n"
        "void main()\n"
        "{\n"
        "    vec2 c = v_uv;\n"
        "    vec2 o = u_dir;\n"
        "    vec3 s = texture(u_src, c).rgb * W0;\n"
        "    s += texture(u_src, c + o).rgb * W1;\n"
        "    s += texture(u_src, c - o).rgb * W1;\n"
        "    s += texture(u_src, c + o * 2.0).rgb * W2;\n"
        "    s += texture(u_src, c - o * 2.0).rgb * W2;\n"
        "    s += texture(u_src, c + o * 3.0).rgb * W3;\n"
        "    s += texture(u_src, c - o * 3.0).rgb * W3;\n"
        "    s += texture(u_src, c + o * 4.0).rgb * W4;\n"
        "    s += texture(u_src, c - o * 4.0).rgb * W4;\n"
        "    fragColor = vec4(s, 1.0);\n"
        "}\n";

    inline GLuint compile(GLenum t, const char* src)
    {
        GLuint s = glCreateShader(t);
        if (!s) return 0;
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        GLint ok = 0;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            GLint n = 0;
            glGetShaderiv(s, GL_INFO_LOG_LENGTH, &n);
            if (n > 1)
            {
                char b[1024] = { 0 };
                glGetShaderInfoLog(s, (n < 1023) ? n : 1023, nullptr, b);
                __android_log_print(ANDROID_LOG_ERROR, "backdrop", "%s", b);
            }
            glDeleteShader(s);
            return 0;
        }
        return s;
    }

    inline GLuint blurProgram()
    {
        static GLuint p = 0;
        static bool tried = false;
        if (tried) return p;
        tried = true;
        GLuint vs = compile(GL_VERTEX_SHADER, kBlurVert);
        if (!vs) return 0;
        GLuint fs = compile(GL_FRAGMENT_SHADER, kBlurFrag);
        if (!fs) { glDeleteShader(vs); return 0; }
        p = glCreateProgram();
        glAttachShader(p, vs);
        glAttachShader(p, fs);
        glLinkProgram(p);
        glDeleteShader(vs);
        glDeleteShader(fs);
        GLint ok = 0;
        glGetProgramiv(p, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            __android_log_print(ANDROID_LOG_ERROR, "backdrop", "blur link failed");
            glDeleteProgram(p);
            p = 0;
        }
        return p;
    }

    inline GLuint makeTarget(int w, int h, GLuint& fbo, GLuint& tex)
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
        const bool okc = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        if (!okc)
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
        if (g_fboB) { glDeleteFramebuffers(1, &g_fboB); g_fboB = 0; }
        if (g_texA) { glDeleteTextures(1, &g_texA);     g_texA = 0; }
        if (g_texB) { glDeleteTextures(1, &g_texB);     g_texB = 0; }
        g_w = g_h = 0;
        g_realtimeBackdrop = 0;
        g_ok = false;
    }

    inline void blurPass(GLuint prog, GLuint src, GLuint dstFbo,
                         int w, int h, float dx, float dy)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, dstFbo);
        glViewport(0, 0, w, h);
        glUseProgram(prog);
        glUniform4f(glGetUniformLocation(prog, "u_rect"), 0.f, 0.f, (float)w, (float)h);
        glUniform2f(glGetUniformLocation(prog, "u_screen"), (float)w, (float)h);
        glUniform2f(glGetUniformLocation(prog, "u_dir"), dx, dy);
        glUniform1i(glGetUniformLocation(prog, "u_src"), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, src);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
    }

    inline GLuint update(int screenW, int screenH)
    {
        if (screenW <= 0 || screenH <= 0) return 0;
        if (g_tried && !g_ok) return 0;

        while (glGetError() != GL_NO_ERROR) { }

        const GLuint prog = blurProgram();
        if (!prog) { g_tried = true; return 0; }

        const int longest = (screenW > screenH) ? screenW : screenH;
        const float k = (longest > kMaxDim) ? (float)kMaxDim / (float)longest : 1.0f;
        int tw = (int)(screenW * k + 0.5f); if (tw < 16) tw = 16;
        int th = (int)(screenH * k + 0.5f); if (th < 16) th = 16;

        if (!g_texA || tw != g_w || th != g_h)
        {
            destroy();
            if (!makeTarget(tw, th, g_fbo, g_texA)) { destroy(); g_tried = true; return 0; }
            if (!makeTarget(tw, th, g_fboB, g_texB)) { destroy(); g_tried = true; return 0; }
            g_w = tw; g_h = th;
        }

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_fbo);
        glBlitFramebuffer(0, 0, screenW, screenH, 0, 0, tw, th,
                          GL_COLOR_BUFFER_BIT, GL_LINEAR);

        blurPass(prog, g_texA, g_fboB, tw, th, 1.5f / (float)tw, 0.f);
        blurPass(prog, g_texB, g_fbo,  tw, th, 0.f, 1.5f / (float)th);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        if (glGetError() != GL_NO_ERROR) { destroy(); g_tried = true; return 0; }

        g_ok = true;
        g_realtimeBackdrop = g_texA;
        return g_texA;
    }
}
