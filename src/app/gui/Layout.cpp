// Layout.cpp -- see Layout.h.

#include "app/gui/Layout.h"

#include "app/gui/Theme.h"
#include "app/gui/Ui.h"

#include <algorithm>
#include <cmath>

namespace gui {

namespace {

// The window the sizes in this GUI were chosen against. Below it the auto
// scale shrinks so the viewport keeps its share of a small screen; above it
// it grows, up to the point where growing stops buying legibility.
constexpr float kRefW = 1600.0f;
constexpr float kRefH = 950.0f;
constexpr float kAutoMin = 0.80f;
constexpr float kAutoMax = 1.25f;

}  // namespace

void apply_style(float scale) {
    ImGuiStyle& style = ImGui::GetStyle();
    style = ImGuiStyle();
    ImGui::StyleColorsDark(&style);
    style.WindowRounding = 4.0f;
    style.ChildRounding = 4.0f;
    style.PopupRounding = 4.0f;
    style.FrameRounding = 3.0f;
    style.GrabRounding = 2.0f;
    style.TabRounding = 3.0f;
    style.ScrollbarRounding = 3.0f;
    style.ScrollbarSize = 10.0f;
    style.GrabMinSize = 10.0f;
    style.WindowPadding = ImVec2(10, 9);
    style.FramePadding = ImVec2(8, 4);
    style.ItemSpacing = ImVec2(8, 6);
    style.ItemInnerSpacing = ImVec2(6, 4);
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 1.0f;
    style.SeparatorTextBorderSize = 1.0f;
    style.SeparatorTextPadding = ImVec2(10, 3);

    using namespace theme;
    auto a = [](ImVec4 v, float alpha) { return ImVec4(v.x, v.y, v.z, alpha); };
    ImVec4* c = style.Colors;
    c[ImGuiCol_Text]                 = kText;
    c[ImGuiCol_TextDisabled]         = kTextDim;
    c[ImGuiCol_WindowBg]             = kBg;
    c[ImGuiCol_ChildBg]              = a(kSurface, 0.92f);
    c[ImGuiCol_PopupBg]              = ImVec4(0.067f, 0.090f, 0.118f, 0.98f);
    c[ImGuiCol_Border]               = kBorder;
    c[ImGuiCol_BorderShadow]         = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_FrameBg]              = kFrame;
    c[ImGuiCol_FrameBgHovered]       = ImVec4(0.075f, 0.106f, 0.137f, 1.0f);
    c[ImGuiCol_FrameBgActive]        = ImVec4(0.067f, 0.137f, 0.169f, 1.0f);
    c[ImGuiCol_TitleBg]              = kBg;
    c[ImGuiCol_TitleBgActive]        = kSurface;
    c[ImGuiCol_TitleBgCollapsed]     = kBg;
    c[ImGuiCol_MenuBarBg]            = ImVec4(0.035f, 0.047f, 0.063f, 1.0f);
    c[ImGuiCol_ScrollbarBg]          = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_ScrollbarGrab]        = ImVec4(0.161f, 0.212f, 0.263f, 1.0f);
    c[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.220f, 0.290f, 0.349f, 1.0f);
    c[ImGuiCol_ScrollbarGrabActive]  = kCyanDeep;
    c[ImGuiCol_CheckMark]            = kCyan;
    c[ImGuiCol_SliderGrab]           = kCyanDeep;
    c[ImGuiCol_SliderGrabActive]     = kCyan;
    c[ImGuiCol_Button]               = ImVec4(0.098f, 0.137f, 0.176f, 1.0f);
    c[ImGuiCol_ButtonHovered]        = ImVec4(0.067f, 0.231f, 0.282f, 1.0f);
    c[ImGuiCol_ButtonActive]         = ImVec4(0.000f, 0.365f, 0.443f, 1.0f);
    c[ImGuiCol_Header]               = ImVec4(0.094f, 0.133f, 0.169f, 1.0f);
    c[ImGuiCol_HeaderHovered]        = ImVec4(0.067f, 0.208f, 0.255f, 1.0f);
    c[ImGuiCol_HeaderActive]         = ImVec4(0.000f, 0.302f, 0.365f, 1.0f);
    c[ImGuiCol_Separator]            = kBorder;
    c[ImGuiCol_SeparatorHovered]     = a(kCyan, 0.6f);
    c[ImGuiCol_SeparatorActive]      = kCyan;
    c[ImGuiCol_ResizeGrip]           = a(kCyan, 0.12f);
    c[ImGuiCol_ResizeGripHovered]    = a(kCyan, 0.5f);
    c[ImGuiCol_ResizeGripActive]     = kCyan;
    c[ImGuiCol_InputTextCursor]      = kCyan;
    c[ImGuiCol_Tab]                  = kSurface;
    c[ImGuiCol_TabHovered]           = ImVec4(0.067f, 0.231f, 0.282f, 1.0f);
    c[ImGuiCol_TabSelected]          = kSurfaceHi;
    c[ImGuiCol_TabSelectedOverline]  = kCyan;
    c[ImGuiCol_TabDimmed]            = kBg;
    c[ImGuiCol_TabDimmedSelected]    = kSurface;
    c[ImGuiCol_PlotLines]            = kCyan;
    c[ImGuiCol_PlotLinesHovered]     = kTeal;
    c[ImGuiCol_PlotHistogram]        = kCyanDeep;   // also the ProgressBar fill
    c[ImGuiCol_PlotHistogramHovered] = kTeal;
    c[ImGuiCol_TableHeaderBg]        = kSurfaceHi;
    c[ImGuiCol_TableBorderStrong]    = kBorder;
    c[ImGuiCol_TableBorderLight]     = a(kBorder, 0.6f);
    c[ImGuiCol_TableRowBg]           = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_TableRowBgAlt]        = a(kText, 0.022f);
    c[ImGuiCol_TextLink]             = kCyan;
    c[ImGuiCol_TextSelectedBg]       = a(kCyan, 0.28f);
    c[ImGuiCol_DragDropTarget]       = kCyan;
    c[ImGuiCol_NavCursor]            = kCyan;
    c[ImGuiCol_NavWindowingHighlight] = a(kText, 0.7f);
    c[ImGuiCol_NavWindowingDimBg]    = ImVec4(0.0f, 0.0f, 0.0f, 0.45f);
    c[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.0f, 0.02f, 0.03f, 0.65f);
    style.ScaleAllSizes(scale);
    style.FontScaleMain = scale;
}

void UiScale::update(ImVec2 display_size) {
    float f = _user;
    if (f <= 0.0f) {
        const float w = display_size.x / _dpi, h = display_size.y / _dpi;
        f = std::min(w / kRefW, h / kRefH);
        f = std::clamp(f, kAutoMin, kAutoMax);
        // Snapped, so that dragging a window edge steps the interface size a
        // few times rather than reflowing it on every pixel.
        f = std::round(f * 20.0f) / 20.0f;
    }
    const float s = _dpi * f;
    if (std::fabs(s - _applied) < 1e-3f) return;
    _applied = s;
    apply_style(s);
}

// ---------------------------------------------------------------------------
// Splitters
// ---------------------------------------------------------------------------

namespace {

float grab_thickness() { return std::max(4.0f, px(6.0f)); }

void draw_grab(ImVec2 p, ImVec2 size, bool hot) {
    const ImU32 col = ImGui::GetColorU32(hot ? ImGuiCol_SeparatorActive
                                            : ImGuiCol_Separator);
    const float t = 1.0f + px(0.5f);
    ImVec2 a, b;
    if (size.x < size.y) {
        const float cx = p.x + size.x * 0.5f;
        a = ImVec2(cx - t, p.y);
        b = ImVec2(cx + t, p.y + size.y);
    } else {
        const float cy = p.y + size.y * 0.5f;
        a = ImVec2(p.x, cy - t);
        b = ImVec2(p.x + size.x, cy + t);
    }
    ImGui::GetWindowDrawList()->AddRectFilled(a, b, col);
}

}  // namespace

float splitter_extent() {
    return grab_thickness() + ImGui::GetStyle().ItemSpacing.y;
}

bool splitter_v(const char* id, float* extent, float lo, float hi, float span) {
    hi = std::max(hi, lo);   // a window too narrow for the minimum: pin it
    const float t = grab_thickness();
    ImGui::SameLine(0.0f, 0.0f);
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ui::InvisibleButtonRaw(id, ImVec2(t, span));
    const bool hot = ImGui::IsItemActive() || ImGui::IsItemHovered();
    if (hot) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
    draw_grab(p, ImVec2(t, span), hot);
    ImGui::SameLine(0.0f, 0.0f);
    if (!ImGui::IsItemActive()) return false;
    const float was = *extent;
    *extent = std::clamp(*extent + ImGui::GetIO().MouseDelta.x, lo, hi);
    return *extent != was;
}

bool splitter_h(const char* id, float* extent, float lo, float hi, float span) {
    hi = std::max(hi, lo);
    const float t = grab_thickness();
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ui::InvisibleButtonRaw(id, ImVec2(span, t));
    const bool hot = ImGui::IsItemActive() || ImGui::IsItemHovered();
    if (hot) ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
    draw_grab(p, ImVec2(span, t), hot);
    if (!ImGui::IsItemActive()) return false;
    const float was = *extent;
    // The handle sits above the pane it sizes, so dragging up makes it taller.
    *extent = std::clamp(*extent - ImGui::GetIO().MouseDelta.y, lo, hi);
    return *extent != was;
}

// ---------------------------------------------------------------------------

std::string elide_middle(const std::string& s, float max_w) {
    if (max_w <= 0.0f || ImGui::CalcTextSize(s.c_str()).x <= max_w) return s;
    const char* kDots = "...";
    // Bytes kept from each end, grown together until the result stops fitting.
    size_t head = 0, tail = 0;
    std::string best = kDots;
    auto step_fwd = [&](size_t i) {
        i++;
        while (i < s.size() && (s[i] & 0xC0) == 0x80) i++;
        return i;
    };
    auto step_back = [&](size_t i) {
        i--;
        while (i > 0 && (s[i] & 0xC0) == 0x80) i--;
        return i;
    };
    while (true) {
        const size_t nh = step_fwd(head);
        const size_t nt = s.size() - step_back(s.size() - tail);
        if (nh + nt >= s.size()) return s;
        std::string cand = s.substr(0, nh) + kDots + s.substr(s.size() - nt);
        if (ImGui::CalcTextSize(cand.c_str()).x > max_w) return best;
        head = nh;
        tail = nt;
        best.swap(cand);
    }
}

}  // namespace gui
