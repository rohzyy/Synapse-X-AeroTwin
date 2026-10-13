// Theme.cpp -- see Theme.h.

#include "app/gui/Theme.h"

#include "app/gui/Layout.h"
#include "app/gui/Ui.h"

#include <algorithm>
#include <array>
#include <cfloat>
#include <cmath>
#include <cstring>

namespace gui::theme {

namespace {

constexpr float kPi = 3.14159265f;

ImU32 col(const ImVec4& c, float alpha = 1.0f) {
    return ImGui::ColorConvertFloat4ToU32(ImVec4(c.x, c.y, c.z, c.w * alpha));
}

ImVec4 mix(const ImVec4& a, const ImVec4& b, float t) {
    return ImVec4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
                  a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);
}

// A soft disc that fades to nothing at `r`. Three rings rather than one so
// the falloff reads as light and not as a cone.
void glow(ImDrawList* dl, ImVec2 c, float r, const ImVec4& tint, float alpha,
          int seg = 40) {
    if (r <= 0.5f || alpha <= 0.0f) return;
    constexpr int kRings = 3;
    const float ring_r[kRings] = {0.30f, 0.62f, 1.0f};
    const float ring_a[kRings] = {0.62f, 0.22f, 0.0f};
    const ImVec2 uv = ImGui::GetFontTexUvWhitePixel();
    dl->PrimReserve(seg * 3 + (kRings - 1) * seg * 6, 1 + kRings * seg);
    const unsigned base = dl->_VtxCurrentIdx;
    dl->PrimWriteVtx(c, uv, col(tint, alpha));
    for (int k = 0; k < kRings; k++) {
        const ImU32 ck = col(tint, alpha * ring_a[k]);
        for (int i = 0; i < seg; i++) {
            const float a = 2.0f * kPi * (float)i / (float)seg;
            dl->PrimWriteVtx(ImVec2(c.x + std::cos(a) * r * ring_r[k],
                                    c.y + std::sin(a) * r * ring_r[k]), uv, ck);
        }
    }
    auto ring = [&](int k, int i) { return (ImDrawIdx)(base + 1 + k * seg + (i % seg)); };
    for (int i = 0; i < seg; i++) {
        dl->PrimWriteIdx((ImDrawIdx)base);
        dl->PrimWriteIdx(ring(0, i));
        dl->PrimWriteIdx(ring(0, i + 1));
    }
    for (int k = 0; k + 1 < kRings; k++) {
        for (int i = 0; i < seg; i++) {
            dl->PrimWriteIdx(ring(k, i));
            dl->PrimWriteIdx(ring(k + 1, i));
            dl->PrimWriteIdx(ring(k + 1, i + 1));
            dl->PrimWriteIdx(ring(k, i));
            dl->PrimWriteIdx(ring(k + 1, i + 1));
            dl->PrimWriteIdx(ring(k, i + 1));
        }
    }
}

// One anisotropic Gaussian, approximated by three nested ellipses whose alpha
// accumulates toward the centre.
void splat(ImDrawList* dl, ImVec2 c, float rx, float ry, float rot,
           const ImVec4& tint, float alpha) {
    const float sc[3] = {1.0f, 0.62f, 0.30f};
    const float al[3] = {0.18f, 0.32f, 0.75f};
    for (int k = 0; k < 3; k++)
        dl->AddEllipseFilled(c, ImVec2(std::max(0.6f, rx * sc[k]), std::max(0.5f, ry * sc[k])),
                             col(tint, alpha * al[k]), rot, 12);
}

// Deterministic per-index noise in [0, 1).
float hash01(unsigned i) {
    i = (i ^ 61u) ^ (i >> 16);
    i *= 9u;
    i ^= i >> 4;
    i *= 0x27d4eb2du;
    i ^= i >> 15;
    return (float)(i & 0xffffffu) / 16777216.0f;
}

float hover_anim(ImGuiID id, bool hot) {
    float* v = ImGui::GetStateStorage()->GetFloatRef(id, 0.0f);
    const float dt = std::min(ImGui::GetIO().DeltaTime, 0.1f);
    *v += ((hot ? 1.0f : 0.0f) - *v) * std::min(1.0f, dt * 16.0f);
    return *v;
}

void draw_icon(ImDrawList* dl, Icon icon, ImVec2 c, float s, ImU32 ink) {
    const float t = std::max(1.3f, s * 0.075f);
    switch (icon) {
    case Icon::Folder: {
        const ImVec2 a(c.x - s * 0.42f, c.y - s * 0.26f), b(c.x + s * 0.42f, c.y + s * 0.32f);
        dl->AddRect(a, b, ink, s * 0.04f, 0, t);
        dl->AddLine(ImVec2(a.x, a.y), ImVec2(a.x + s * 0.06f, a.y - s * 0.12f), ink, t);
        dl->AddLine(ImVec2(a.x + s * 0.06f, a.y - s * 0.12f),
                    ImVec2(a.x + s * 0.30f, a.y - s * 0.12f), ink, t);
        dl->AddLine(ImVec2(a.x + s * 0.30f, a.y - s * 0.12f), ImVec2(a.x + s * 0.38f, a.y), ink, t);
        dl->AddLine(ImVec2(a.x, a.y + s * 0.14f), ImVec2(b.x, a.y + s * 0.14f), ink, t);
        break;
    }
    case Icon::Capture: {
        const ImVec2 a(c.x - s * 0.44f, c.y - s * 0.24f), b(c.x + s * 0.44f, c.y + s * 0.34f);
        dl->AddRect(a, b, ink, s * 0.05f, 0, t);
        dl->AddLine(ImVec2(c.x - s * 0.16f, a.y), ImVec2(c.x - s * 0.10f, a.y - s * 0.12f), ink, t);
        dl->AddLine(ImVec2(c.x - s * 0.10f, a.y - s * 0.12f),
                    ImVec2(c.x + s * 0.10f, a.y - s * 0.12f), ink, t);
        dl->AddLine(ImVec2(c.x + s * 0.10f, a.y - s * 0.12f), ImVec2(c.x + s * 0.16f, a.y), ink, t);
        dl->AddCircle(ImVec2(c.x, c.y + s * 0.05f), s * 0.17f, ink, 0, t);
        break;
    }
    case Icon::Cube: {
        const float r = s * 0.42f;
        ImVec2 v[6];
        for (int i = 0; i < 6; i++) {
            const float a = kPi / 3.0f * (float)i - kPi / 2.0f;
            v[i] = ImVec2(c.x + std::cos(a) * r, c.y + std::sin(a) * r);
        }
        dl->AddPolyline(v, 6, ink, ImDrawFlags_Closed, t);
        dl->AddLine(c, v[0], ink, t);
        dl->AddLine(c, v[2], ink, t);
        dl->AddLine(c, v[4], ink, t);
        break;
    }
    case Icon::Mesh: {
        const ImVec2 a(c.x, c.y - s * 0.40f), b(c.x - s * 0.44f, c.y + s * 0.34f),
                     d(c.x + s * 0.44f, c.y + s * 0.34f);
        const ImVec2 ab((a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f),
                     bd((b.x + d.x) * 0.5f, (b.y + d.y) * 0.5f),
                     da((d.x + a.x) * 0.5f, (d.y + a.y) * 0.5f);
        dl->AddTriangle(a, b, d, ink, t);
        dl->AddTriangle(ab, bd, da, ink, t);
        break;
    }
    case Icon::Stack: {
        for (int i = 0; i < 3; i++) {
            const float y = c.y - s * 0.26f + (float)i * s * 0.24f;
            const float in = (float)(2 - i) * s * 0.05f;
            dl->AddRect(ImVec2(c.x - s * 0.42f + in, y - s * 0.08f),
                        ImVec2(c.x + s * 0.42f - in, y + s * 0.08f), ink, s * 0.03f, 0, t);
        }
        break;
    }
    case Icon::Resume: {
        dl->AddTriangleFilled(ImVec2(c.x - s * 0.22f, c.y - s * 0.32f),
                              ImVec2(c.x - s * 0.22f, c.y + s * 0.32f),
                              ImVec2(c.x + s * 0.34f, c.y), ink);
        break;
    }
    }
}

}  // namespace

// ===========================================================================
// Backdrop and logo
// ===========================================================================

void draw_backdrop() {
    const ImGuiViewport* vp = ImGui::GetMainViewport();
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    const ImVec2 p0 = vp->Pos;
    const ImVec2 p1(vp->Pos.x + vp->Size.x, vp->Pos.y + vp->Size.y);
    const float w = vp->Size.x, h = vp->Size.y;

    dl->AddRectFilledMultiColor(p0, p1, col(ImVec4(0.051f, 0.071f, 0.094f, 1.0f)),
                                col(ImVec4(0.051f, 0.071f, 0.094f, 1.0f)),
                                col(kBg), col(kBg));
    glow(dl, ImVec2(p0.x + w * 0.92f, p0.y), w * 0.42f, kCyan, 0.045f, 64);

    const float step = std::max(24.0f, px(48.0f));
    const ImU32 line = col(kText, 0.022f);
    for (float x = p0.x + step; x < p1.x; x += step)
        dl->AddLine(ImVec2(x, p0.y), ImVec2(x, p1.y), line);
    for (float y = p0.y + step; y < p1.y; y += step)
        dl->AddLine(ImVec2(p0.x, y), ImVec2(p1.x, y), line);
    (void)h;
}

void draw_logo(ImDrawList* dl, ImVec2 c, float s) {
    const float h = s * 0.5f, arm = s * 0.22f, t = std::max(1.2f, s * 0.045f);
    const ImU32 frame = col(kTextDim, 0.85f);
    const ImVec2 a(c.x - h, c.y - h), b(c.x + h, c.y + h);
    dl->AddLine(a, ImVec2(a.x + arm, a.y), frame, t);
    dl->AddLine(a, ImVec2(a.x, a.y + arm), frame, t);
    dl->AddLine(ImVec2(b.x, a.y), ImVec2(b.x - arm, a.y), frame, t);
    dl->AddLine(ImVec2(b.x, a.y), ImVec2(b.x, a.y + arm), frame, t);
    dl->AddLine(ImVec2(a.x, b.y), ImVec2(a.x + arm, b.y), frame, t);
    dl->AddLine(ImVec2(a.x, b.y), ImVec2(a.x, b.y - arm), frame, t);
    dl->AddLine(b, ImVec2(b.x - arm, b.y), frame, t);
    dl->AddLine(b, ImVec2(b.x, b.y - arm), frame, t);
    splat(dl, c, s * 0.40f, s * 0.085f, kPi * 0.25f, kTeal, 1.0f);
    splat(dl, c, s * 0.40f, s * 0.085f, -kPi * 0.25f, kCyan, 1.0f);
    dl->AddCircleFilled(c, s * 0.055f, col(kText));
}

// ===========================================================================
// Hero
// ===========================================================================

void draw_hero(ImVec2 p0, ImVec2 p1, const char* name, const char* tagline,
               const char* chip) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float w = p1.x - p0.x, h = p1.y - p0.y;
    if (w <= 0.0f || h <= 0.0f) return;
    const float R = px(6.0f);
    const float t = (float)ImGui::GetTime();

    dl->AddRectFilledMultiColor(p0, p1, col(kSurface), col(ImVec4(0.039f, 0.055f, 0.071f, 1.0f)),
                                col(kInset), col(kSurface));
    dl->PushClipRect(p0, p1, true);

    // The right half is a viewport: a floor grid in perspective under a
    // rotating cloud of anisotropic splats.
    const float vx0 = p0.x + w * 0.46f;
    const ImVec2 sc(p0.x + w * 0.74f, p0.y + h * 0.47f);
    const float horizon = p0.y + h * 0.62f;
    const ImU32 grid = col(kCyan, 0.07f);
    for (int i = -12; i <= 12; i++) {
        const float x = sc.x + (float)i * px(26.0f);
        dl->AddLine(ImVec2(sc.x + (float)i * px(5.0f), horizon),
                    ImVec2(sc.x + (x - sc.x) * 4.0f, p1.y + px(40.0f)), grid);
    }
    for (int j = 0; j < 7; j++) {
        const float f = (float)j / 6.0f;
        const float y = horizon + (p1.y - horizon) * f * f;
        dl->AddLine(ImVec2(vx0, y), ImVec2(p1.x, y), col(kCyan, 0.03f + 0.05f * f));
    }
    glow(dl, sc, std::min(h * 0.75f, w * 0.30f), kCyan, 0.10f, 48);

    {
        constexpr int kSplats = 900;
        const float rad = std::min(h * 0.30f, w * 0.17f);
        const float yaw = t * 0.25f, tilt = 0.55f;
        const float cy = std::cos(yaw), sy = std::sin(yaw);
        const float ct = std::cos(tilt), st = std::sin(tilt);
        struct S { ImVec2 p; float z, rot, u; };
        std::array<S, kSplats> pts;
        auto knot = [](float u, float out[3]) {
            const float r = 0.62f + 0.30f * std::cos(3.0f * u);
            out[0] = r * std::cos(2.0f * u);
            out[1] = 0.32f * std::sin(3.0f * u);
            out[2] = r * std::sin(2.0f * u);
        };
        auto project = [&](const float v[3], float* x, float* y, float* z) {
            const float x1 = v[0] * cy + v[2] * sy, z1 = -v[0] * sy + v[2] * cy;
            *x = x1;
            *y = v[1] * ct - z1 * st;
            *z = v[1] * st + z1 * ct;
        };
        for (int i = 0; i < kSplats; i++) {
            const float u = 2.0f * kPi * (float)i / (float)kSplats;
            float a[3], b[3];
            knot(u, a);
            knot(u + 0.01f, b);
            // Scatter across the tube so the knot reads as a surface of splats.
            const float jr = 0.09f * std::sqrt(hash01((unsigned)i * 3u + 1u));
            const float th = 2.0f * kPi * hash01((unsigned)i * 7u + 2u);
            const float ph = std::acos(2.0f * hash01((unsigned)i * 11u + 3u) - 1.0f);
            const float off[3] = {jr * std::sin(ph) * std::cos(th), jr * std::cos(ph),
                                  jr * std::sin(ph) * std::sin(th)};
            for (int c = 0; c < 3; c++) { a[c] += off[c]; b[c] += off[c]; }
            float ax, ay, az, bx, by, bz;
            project(a, &ax, &ay, &az);
            project(b, &bx, &by, &bz);
            pts[i] = {ImVec2(sc.x + ax * rad * 1.5f, sc.y + ay * rad * 1.5f), az,
                      std::atan2(by - ay, bx - ax), (float)i / (float)kSplats};
        }
        std::sort(pts.begin(), pts.end(), [](const S& a, const S& b) { return a.z < b.z; });
        for (const S& q : pts) {
            const float depth = std::clamp((q.z + 1.0f) * 0.5f, 0.0f, 1.0f);
            const ImVec4 c = mix(kTeal, kCyan, 0.5f + 0.5f * std::sin(q.u * 2.0f * kPi * 2.0f + t * 0.6f));
            splat(dl, q.p, px(3.5f + 4.0f * depth), px(1.3f + 1.4f * depth), q.rot,
                  mix(c, kText, 0.25f * depth), 0.18f + 0.50f * depth);
        }
    }

    // Axis gizmo and crop marks: this half is a viewport.
    {
        const ImVec2 g(vx0 + px(26.0f), p1.y - px(24.0f));
        const float L = px(16.0f), th = px(1.4f);
        dl->AddLine(g, ImVec2(g.x + L, g.y), col(kCyan, 0.9f), th);
        dl->AddLine(g, ImVec2(g.x, g.y - L), col(kTeal, 0.9f), th);
        dl->AddLine(g, ImVec2(g.x - L * 0.6f, g.y + L * 0.45f), col(kTextDim, 0.9f), th);
    }
    viewport_frame(ImVec2(vx0, p0.y + px(14.0f)), ImVec2(p1.x - px(14.0f), p1.y - px(14.0f)));

    dl->AddRectFilledMultiColor(p0, ImVec2(p0.x + w * 0.56f, p1.y),
        col(kSurface, 0.96f), col(kSurface, 0.0f), col(kSurface, 0.0f), col(kSurface, 0.96f));
    dl->PopClipRect();
    dl->AddRect(p0, p1, col(kBorder), R, 0, px(1.0f));
    dl->AddLine(ImVec2(p0.x + R, p0.y), ImVec2(p0.x + w * 0.30f, p0.y), col(kCyan, 0.55f), px(1.0f));

    ImFont* font = ImGui::GetFont();
    const float fs = ImGui::GetFontSize();
    const float pad = std::max(px(28.0f), w * 0.035f);
    const float big = std::clamp(h * 0.22f, fs * 2.0f, fs * 3.2f);
    const float tag_w = std::max(w * 0.40f, px(240.0f));
    const float tag_h = font->CalcTextSizeA(fs * 1.02f, FLT_MAX, tag_w, tagline).y;
    const float chip_h = fs * 1.45f;
    const float block = chip_h + px(14.0f) + big + px(10.0f) + px(1.0f) + px(12.0f) + tag_h;
    float y = std::max(p0.y + pad * 0.6f, (p0.y + p1.y) * 0.5f - block * 0.5f);
    const float x = p0.x + pad;

    if (chip && *chip) {
        const ImVec2 ts = font->CalcTextSizeA(fs * 0.82f, FLT_MAX, 0.0f, chip);
        const ImVec2 c0(x, y), c1(x + ts.x + fs * 1.6f, y + chip_h);
        dl->AddRect(c0, c1, col(kBorder), px(3.0f), 0, px(1.0f));
        const float pulse = 0.5f + 0.5f * std::sin(t * 2.0f);
        dl->AddCircleFilled(ImVec2(c0.x + fs * 0.55f, (c0.y + c1.y) * 0.5f), fs * 0.14f,
                            col(kTeal, 0.55f + 0.45f * pulse));
        dl->AddText(font, fs * 0.82f, ImVec2(c0.x + fs * 0.95f, (c0.y + c1.y) * 0.5f - ts.y * 0.5f),
                    col(kTextDim), chip);
        y += chip_h + px(14.0f);
    }

    const float mark = big * 0.92f;
    draw_logo(dl, ImVec2(x + mark * 0.5f, y + big * 0.52f), mark);
    const float nx = x + mark + big * 0.35f;
    // The trailing X of the wordmark takes the accent.
    const size_t n = std::strlen(name);
    const bool split = n > 1 && name[n - 1] == 'X';
    dl->AddText(font, big, ImVec2(nx, y), col(kText), name, split ? name + n - 1 : nullptr);
    if (split) {
        const float wx = font->CalcTextSizeA(big, FLT_MAX, 0.0f, name, name + n - 1).x;
        dl->AddText(font, big, ImVec2(nx + wx, y), col(kCyan), name + n - 1);
    }
    y += big + px(10.0f);
    dl->AddLine(ImVec2(x, y), ImVec2(x + px(48.0f), y), col(kCyan), px(1.0f));
    dl->AddLine(ImVec2(x + px(48.0f), y), ImVec2(x + px(160.0f), y), col(kBorder), px(1.0f));
    y += px(1.0f) + px(12.0f);
    dl->AddText(font, fs * 1.02f, ImVec2(x, y), col(kTextDim), tagline, nullptr, tag_w);
}

// ===========================================================================
// Cards, drop zone, viewport frame
// ===========================================================================

namespace {

struct CardMetrics {
    float pad, tile, gap, title_fs, body_fs;
};

CardMetrics card_metrics() {
    const float fs = ImGui::GetFontSize();
    return {px(14.0f), px(34.0f), px(12.0f), fs * 1.04f, fs * 0.9f};
}

}  // namespace

float card_height(const char* title, const char* body, float width) {
    const CardMetrics m = card_metrics();
    ImFont* font = ImGui::GetFont();
    const float text_w = std::max(px(60.0f), width - m.pad * 2.0f - m.tile - m.gap - px(16.0f));
    const float th = font->CalcTextSizeA(m.title_fs, FLT_MAX, text_w, title).y;
    const float bh = body && *body
        ? font->CalcTextSizeA(m.body_fs, FLT_MAX, text_w, body).y + px(4.0f) : 0.0f;
    return m.pad * 2.0f + std::max(m.tile, th + bh);
}

bool action_card(const char* id, Icon icon, const char* title, const char* body,
                 ImVec2 size, bool featured) {
    const CardMetrics m = card_metrics();
    const ImVec2 p0 = ImGui::GetCursorScreenPos();
    const ImVec2 p1(p0.x + size.x, p0.y + size.y);
    const bool clicked = ui::InvisibleButtonRaw(id, size);
    const bool hot = ImGui::IsItemHovered();
    const bool held = ImGui::IsItemActive();
    const float k = hover_anim(ImGui::GetItemID(), hot || ImGui::IsItemFocused());
    if (hot) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);

    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float R = px(5.0f);
    const ImVec4 base = featured ? mix(kSurface, kCyan, 0.06f) : kSurface;
    dl->AddRectFilled(p0, p1, col(held ? kFrame : mix(base, kSurfaceHi, k)), R);
    dl->AddRect(p0, p1, col(mix(featured ? mix(kBorder, kCyan, 0.35f) : kBorder, kCyan, k * 0.7f)),
                R, 0, px(1.0f));
    const float bar = (featured ? 1.0f : k) * (size.y - R * 2.0f);
    if (bar > 1.0f) {
        const float cy = (p0.y + p1.y) * 0.5f;
        dl->AddRectFilled(ImVec2(p0.x, cy - bar * 0.5f), ImVec2(p0.x + px(2.0f), cy + bar * 0.5f),
                          col(kCyan, featured ? 1.0f : k));
    }

    const ImVec2 t0(p0.x + m.pad, p0.y + m.pad), t1(t0.x + m.tile, t0.y + m.tile);
    dl->AddRectFilled(t0, t1, col(kFrame), px(4.0f));
    dl->AddRect(t0, t1, col(mix(kBorder, kCyan, 0.25f + 0.6f * k)), px(4.0f), 0, px(1.0f));
    draw_icon(dl, icon, ImVec2((t0.x + t1.x) * 0.5f, (t0.y + t1.y) * 0.5f), m.tile * 0.5f,
              col(mix(kTeal, kCyan, k)));

    ImFont* font = ImGui::GetFont();
    const float tx = t1.x + m.gap;
    const float text_w = std::max(px(60.0f), p1.x - m.pad - px(16.0f) - tx);
    const float th = font->CalcTextSizeA(m.title_fs, FLT_MAX, text_w, title).y;
    float ty = p0.y + m.pad;
    if (!(body && *body)) ty = (p0.y + p1.y) * 0.5f - th * 0.5f;
    dl->AddText(font, m.title_fs, ImVec2(tx, ty), col(kText), title, nullptr, text_w);
    if (body && *body)
        dl->AddText(font, m.body_fs, ImVec2(tx, ty + th + px(4.0f)), col(kTextDim), body,
                    nullptr, text_w);

    const float ax = p1.x - m.pad + px(3.0f) * k, ay = (p0.y + p1.y) * 0.5f;
    const float a = px(4.0f);
    const ImU32 ac = col(mix(kTextDim, kCyan, k), 0.4f + 0.6f * k);
    dl->AddLine(ImVec2(ax - a, ay - a), ImVec2(ax, ay), ac, px(1.4f));
    dl->AddLine(ImVec2(ax, ay), ImVec2(ax - a, ay + a), ac, px(1.4f));
    return clicked;
}

void drop_zone(const char* text, float width) {
    ImFont* font = ImGui::GetFont();
    const float fs = ImGui::GetFontSize() * 0.92f;
    const float pad = px(12.0f);
    const float icon = px(20.0f);
    const float text_w = std::max(px(60.0f), width - pad * 3.0f - icon);
    const float th = font->CalcTextSizeA(fs, FLT_MAX, text_w, text).y;
    const float h = std::max(icon, th) + pad * 2.0f;
    const ImVec2 p0 = ImGui::GetCursorScreenPos(), p1(p0.x + width, p0.y + h);
    ImGui::Dummy(ImVec2(width, h));

    ImDrawList* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(p0, p1, col(kInset, 0.6f), px(4.0f));
    const float dash = px(6.0f), gap = px(4.0f), th_l = px(1.0f);
    const ImU32 dc = col(kBorder);
    auto dashed = [&](ImVec2 a, ImVec2 b) {
        const float len = std::hypot(b.x - a.x, b.y - a.y);
        if (len <= 0.0f) return;
        const ImVec2 d((b.x - a.x) / len, (b.y - a.y) / len);
        for (float s = 0.0f; s < len; s += dash + gap) {
            const float e = std::min(len, s + dash);
            dl->AddLine(ImVec2(a.x + d.x * s, a.y + d.y * s),
                        ImVec2(a.x + d.x * e, a.y + d.y * e), dc, th_l);
        }
    };
    dashed(p0, ImVec2(p1.x, p0.y));
    dashed(ImVec2(p0.x, p1.y), p1);
    dashed(p0, ImVec2(p0.x, p1.y));
    dashed(ImVec2(p1.x, p0.y), p1);

    const ImVec2 ic(p0.x + pad + icon * 0.5f, (p0.y + p1.y) * 0.5f);
    const ImU32 ink = col(kCyan, 0.85f);
    const float t = px(1.4f);
    dl->AddLine(ImVec2(ic.x, ic.y - icon * 0.36f), ImVec2(ic.x, ic.y + icon * 0.14f), ink, t);
    dl->AddLine(ImVec2(ic.x - icon * 0.2f, ic.y - icon * 0.16f), ImVec2(ic.x, ic.y - icon * 0.36f), ink, t);
    dl->AddLine(ImVec2(ic.x + icon * 0.2f, ic.y - icon * 0.16f), ImVec2(ic.x, ic.y - icon * 0.36f), ink, t);
    dl->AddLine(ImVec2(ic.x - icon * 0.4f, ic.y + icon * 0.38f),
                ImVec2(ic.x + icon * 0.4f, ic.y + icon * 0.38f), ink, t);

    dl->AddText(font, fs, ImVec2(p0.x + pad * 2.0f + icon, (p0.y + p1.y) * 0.5f - th * 0.5f),
                col(kTextDim), text, nullptr, text_w);
}

void viewport_frame(ImVec2 p0, ImVec2 p1) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float in = px(6.0f), L = px(14.0f), t = px(1.2f);
    const ImU32 c = col(kCyan, 0.55f);
    const ImVec2 a(p0.x + in, p0.y + in), b(p1.x - in, p1.y - in);
    if (b.x - a.x < L * 3.0f || b.y - a.y < L * 3.0f) return;
    dl->AddLine(a, ImVec2(a.x + L, a.y), c, t);
    dl->AddLine(a, ImVec2(a.x, a.y + L), c, t);
    dl->AddLine(ImVec2(b.x, a.y), ImVec2(b.x - L, a.y), c, t);
    dl->AddLine(ImVec2(b.x, a.y), ImVec2(b.x, a.y + L), c, t);
    dl->AddLine(ImVec2(a.x, b.y), ImVec2(a.x + L, b.y), c, t);
    dl->AddLine(ImVec2(a.x, b.y), ImVec2(a.x, b.y - L), c, t);
    dl->AddLine(b, ImVec2(b.x - L, b.y), c, t);
    dl->AddLine(b, ImVec2(b.x, b.y - L), c, t);
}

void push_title_font(float scale) {
    ImGui::PushFont(nullptr, ImGui::GetStyle().FontSizeBase * scale);
}

void pop_title_font() { ImGui::PopFont(); }

}  // namespace gui::theme
