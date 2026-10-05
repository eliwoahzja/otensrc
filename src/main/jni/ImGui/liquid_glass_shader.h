#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include <GLES3/gl3.h>
#include <android/log.h>

namespace liquid
{
    inline const char* kVert =
        "#version 300 es\n"
        "uniform vec4 u_rect;\n"
        "uniform vec2 u_screen;\n"
        "out vec2 v_local;\n"
        "void main()\n"
        "{\n"
        "    vec2 c = vec2(float((gl_VertexID & 1) == 0), float((gl_VertexID & 2) == 0));\n"
        "    vec2 px = u_rect.xy + c * u_rect.zw;\n"
        "    vec2 ndc = vec2(px.x / u_screen.x * 2.0 - 1.0,\n"
        "                    1.0 - px.y / u_screen.y * 2.0);\n"
        "    gl_Position = vec4(ndc, 0.0, 1.0);\n"
        "    v_local = c;\n"
        "}\n";

    inline const char* kFrag =
        "#version 300 es\n"
        "precision highp float;\n"
        "uniform sampler2D u_backdrop;\n"
        "uniform vec4  u_rect;\n"
        "uniform vec2  u_screen;\n"
        "uniform vec4  u_uv;\n"
        "uniform float u_radius;\n"
        "uniform float u_refract;\n"
        "uniform float u_dispersion;\n"
        "uniform float u_specular;\n"
        "uniform float u_edge;\n"
        "uniform float u_tint;\n"
        "in  vec2 v_local;\n"
        "out vec4 fragColor;\n"
        "\n"
        "float sdRoundBox(vec2 p, vec2 b, float r)\n"
        "{\n"
        "    vec2 q = abs(p) - b + r;\n"
        "    return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - r;\n"
        "}\n"
        "\n"
        "vec2 toScreen(vec2 local)\n"
        "{\n"
        "    return u_rect.xy + local * u_rect.zw;\n"
        "}\n"
        "\n"
        "void main()\n"
        "{\n"
        "    vec2 hs = u_rect.zw * 0.5;\n"
        "    vec2 p = v_local * u_rect.zw - hs;\n"
        "\n"
        "    float d = sdRoundBox(p, hs, u_radius);\n"
        "\n"
        "    // Discard fully outside so the quad can be oversized for the halo.\n"
        "    if (d > 1.0) discard;\n"
        "\n"
        "    // Surface normal from the SDF gradient: this is the lens.\n"
        "    float e = 1.25;\n"
        "    vec2 grad = vec2(\n"
        "        sdRoundBox(p + vec2(e, 0.0), hs, u_radius) - sdRoundBox(p - vec2(e, 0.0), hs, u_radius),\n"
        "        sdRoundBox(p + vec2(0.0, e), hs, u_radius) - sdRoundBox(p - vec2(0.0, e), hs, u_radius));\n"
        "    grad = (abs(grad.x) + abs(grad.y) > 1e-5) ? normalize(grad) : vec2(0.0);\n"
        "\n"
        "    // Strongest refraction right at the rim, flat through the middle.\n"
        "    float rim = 1.0 - smoothstep(0.0, u_edge, -d);\n"
        "    vec2 off = grad * u_refract * rim;\n"
        "\n"
        "    vec2 uvBase = mix(u_uv.xy, u_uv.zw, v_local);\n"
        "    float disp = u_dispersion * rim;\n"
        "\n"
        "    // Per-channel offset: the prism fringe at the edges.\n"
        "    vec3 col;\n"
        "    col.r = texture(u_backdrop, uvBase + off * (1.0 + disp)).r;\n"
        "    col.g = texture(u_backdrop, uvBase + off).g;\n"
        "    col.b = texture(u_backdrop, uvBase + off * (1.0 - disp)).b;\n"
        "\n"
        "    // Neutral interior: darken and slightly cool the backdrop.\n"
        "    col = mix(col, col * vec3(0.93, 0.94, 1.0), 0.6);\n"
        "    col *= (1.0 - u_tint);\n"
        "\n"
        "    // Fresnel-ish specular: bright where the normal turns away.\n"
        "    float fres = pow(clamp(rim, 0.0, 1.0), 2.5);\n"
        "    col += fres * u_specular;\n"
        "\n"
        "    // Thin bright lip right on the boundary.\n"
        "    float lip = 1.0 - smoothstep(0.0, 2.0, abs(d));\n"
        "    col += lip * 0.16;\n"
        "\n"
        "    // Feather the outer boundary over ~1px to avoid a hard cut.\n"
        "    float alpha = 1.0 - smoothstep(-1.0, 0.0, d);\n"
        "    fragColor = vec4(col, alpha);\n"
        "}\n";

    inline GLuint compile(GLenum type, const char* src, const char* tag)
    {
        GLuint s = glCreateShader(type);
        if (!s) return 0;
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        GLint ok = 0;
        glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok)
        {
            GLint len = 0;
            glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
            if (len > 1)
            {
                char buf[1536] = { 0 };
                glGetShaderInfoLog(s, (len < 1535) ? len : 1535, nullptr, buf);
                __android_log_print(ANDROID_LOG_ERROR, "liquid_glass", "%s: %s", tag, buf);
            }
            glDeleteShader(s);
            return 0;
        }
        return s;
    }

    inline GLuint program()
    {
        static GLuint prog = 0;
        static bool tried = false;
        if (tried) return prog;
        tried = true;

        while (glGetError() != GL_NO_ERROR) { }

        GLuint vs = compile(GL_VERTEX_SHADER, kVert, "vert");
        if (!vs) return 0;
        GLuint fs = compile(GL_FRAGMENT_SHADER, kFrag, "frag");
        if (!fs) { glDeleteShader(vs); return 0; }

        prog = glCreateProgram();
        glAttachShader(prog, vs);
        glAttachShader(prog, fs);
        glLinkProgram(prog);
        glDeleteShader(vs);
        glDeleteShader(fs);

        GLint ok = 0;
        glGetProgramiv(prog, GL_LINK_STATUS, &ok);
        if (!ok)
        {
            GLint len = 0;
            glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
            if (len > 1)
            {
                char buf[1536] = { 0 };
                glGetProgramInfoLog(prog, (len < 1535) ? len : 1535, nullptr, buf);
                __android_log_print(ANDROID_LOG_ERROR, "liquid_glass", "link: %s", buf);
            }
            glDeleteProgram(prog);
            prog = 0;
        }
        else
        {
            __android_log_print(ANDROID_LOG_INFO, "liquid_glass", "program ready");
        }
        return prog;
    }

    struct Params
    {
        ImVec2 rect_min;
        ImVec2 rect_size;
        ImVec4 uv;          // backdrop uv window
        float  radius   = 14.f;
        float  refract  = 0.030f;
        float  disperse = 0.22f;
        float  specular = 0.30f;
        float  edge     = 18.f;
        float  tint     = 0.18f;
    };

    inline bool draw(ImDrawList* dl, GLuint tex, const Params& p)
    {
        GLuint prog = program();
        if (!prog || tex == 0 || p.rect_size.x < 2.f || p.rect_size.y < 2.f)
            return false;

        glUseProgram(prog);

        GLint uRect   = glGetUniformLocation(prog, "u_rect");
        GLint uScreen = glGetUniformLocation(prog, "u_screen");
        GLint uUV     = glGetUniformLocation(prog, "u_uv");
        GLint uRadius = glGetUniformLocation(prog, "u_radius");
        GLint uRefr   = glGetUniformLocation(prog, "u_refract");
        GLint uDisp   = glGetUniformLocation(prog, "u_dispersion");
        GLint uSpec   = glGetUniformLocation(prog, "u_specular");
        GLint uEdge   = glGetUniformLocation(prog, "u_edge");
        GLint uTint   = glGetUniformLocation(prog, "u_tint");
        GLint uTex    = glGetUniformLocation(prog, "u_backdrop");

        const ImVec2 disp = ImGui::GetIO().DisplaySize;
        glUniform4f(uRect, p.rect_min.x, p.rect_min.y, p.rect_size.x, p.rect_size.y);
        glUniform2f(uScreen, (disp.x > 0.f) ? disp.x : 1.f, (disp.y > 0.f) ? disp.y : 1.f);
        glUniform4f(uUV, p.uv.x, p.uv.y, p.uv.z, p.uv.w);
        glUniform1f(uRadius, p.radius);
        glUniform1f(uRefr, p.refract);
        glUniform1f(uDisp, p.disperse);
        glUniform1f(uSpec, p.specular);
        glUniform1f(uEdge, p.edge);
        glUniform1f(uTint, p.tint);
        glUniform1i(uTex, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
        return true;
    }
}
