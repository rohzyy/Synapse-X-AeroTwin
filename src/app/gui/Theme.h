#pragma once

// The SynapseX look: the palette apply_style() builds ImGui's colours from,
// the window backdrop, the logo, the home screen's hero and action cards, and
// the viewport frame. The palette has no purple, violet, magenta or pink; keep
// it that way.

#include "imgui.h"

namespace gui::theme {

inline constexpr ImVec4 kBg        {0.043f, 0.059f, 0.078f, 1.00f};   // #0B0F14
inline constexpr ImVec4 kSurface   {0.082f, 0.110f, 0.141f, 1.00f};   // #151C24
inline constexpr ImVec4 kSurfaceHi {0.106f, 0.141f, 0.180f, 1.00f};   // #1B242E
inline constexpr ImVec4 kFrame     {0.055f, 0.075f, 0.098f, 1.00f};   // #0E1319
inline constexpr ImVec4 kInset     {0.031f, 0.043f, 0.059f, 1.00f};   // #080B0F
inline constexpr ImVec4 kBorder    {0.141f, 0.188f, 0.235f, 1.00f};   // #24303C
inline constexpr ImVec4 kText      {0.910f, 0.941f, 0.961f, 1.00f};   // #E8F0F5
inline constexpr ImVec4 kTextDim   {0.545f, 0.604f, 0.659f, 1.00f};   // #8B9AA8
inline constexpr ImVec4 kCyan      {0.000f, 0.851f, 1.000f, 1.00f};   // #00D9FF
inline constexpr ImVec4 kCyanDeep  {0.000f, 0.522f, 0.627f, 1.00f};   // #0085A0
inline constexpr ImVec4 kTeal      {0.000f, 0.749f, 0.651f, 1.00f};   // #00BFA6
inline constexpr ImVec4 kAmber     {1.000f, 0.710f, 0.278f, 1.00f};   // #FFB547

enum class Icon { Folder, Capture, Cube, Mesh, Stack, Resume };

// Paints the whole viewport behind every window; call once per frame.
void draw_backdrop();

// The mark: a bracketed X of two splats. `s` is its height in pixels.
void draw_logo(ImDrawList* dl, ImVec2 center, float s);

void draw_hero(ImVec2 p0, ImVec2 p1, const char* name, const char* tagline,
               const char* chip);

float card_height(const char* title, const char* body, float width);

// A clickable card. `id` must be unique in the current ID scope.
bool action_card(const char* id, Icon icon, const char* title,
                 const char* body, ImVec2 size, bool featured = false);

// A dashed panel with `text` in it, the home screen's drop target.
void drop_zone(const char* text, float width);

// Corner brackets over a viewport rectangle, as camera software frames one.
void viewport_frame(ImVec2 p0, ImVec2 p1);

// Screen headings: the current face, larger.
void push_title_font(float scale = 1.3f);
void pop_title_font();

}  // namespace gui::theme
