// Keyboard and mouse pictures for the game's button prompts: see key_glyphs.h.

#include "key_glyphs.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iterator>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <rex/cvar.h>
#include <rex/filesystem.h>
#include <rex/logging.h>

// stb_truetype draws the key names (its own private copy here, as ImGui keeps one of its own).
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
#include <imstb_truetype.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

// The keyboard's controller emulation (rexglue-sdk src/input/mnk/mnk_input_driver.cpp).
REXCVAR_DECLARE(bool, mnk_mouse);
REXCVAR_DECLARE(std::string, keybind_a);
REXCVAR_DECLARE(std::string, keybind_b);
REXCVAR_DECLARE(std::string, keybind_x);
REXCVAR_DECLARE(std::string, keybind_y);
REXCVAR_DECLARE(std::string, keybind_left_trigger);
REXCVAR_DECLARE(std::string, keybind_right_trigger);
REXCVAR_DECLARE(std::string, keybind_left_shoulder);
REXCVAR_DECLARE(std::string, keybind_right_shoulder);
REXCVAR_DECLARE(std::string, keybind_lstick_up);
REXCVAR_DECLARE(std::string, keybind_lstick_down);
REXCVAR_DECLARE(std::string, keybind_lstick_left);
REXCVAR_DECLARE(std::string, keybind_lstick_right);
REXCVAR_DECLARE(std::string, keybind_lstick_press);
REXCVAR_DECLARE(std::string, keybind_rstick_up);
REXCVAR_DECLARE(std::string, keybind_rstick_down);
REXCVAR_DECLARE(std::string, keybind_rstick_left);
REXCVAR_DECLARE(std::string, keybind_rstick_right);
REXCVAR_DECLARE(std::string, keybind_rstick_press);
REXCVAR_DECLARE(std::string, keybind_dpad_up);
REXCVAR_DECLARE(std::string, keybind_dpad_down);
REXCVAR_DECLARE(std::string, keybind_dpad_left);
REXCVAR_DECLARE(std::string, keybind_dpad_right);
REXCVAR_DECLARE(std::string, keybind_back);
REXCVAR_DECLARE(std::string, keybind_start);
REXCVAR_DECLARE(std::string, keybind_guide);

namespace svr::keys {
namespace {

// --- What each button is on the keyboard -----------------------------------------------------

bool Four(Pad pad) { return pad == Pad::LStick || pad == Pad::RStick || pad == Pad::DPad; }

// A button's bind; for a stick or the D-pad its four, in the order up, left, down, right.
std::array<std::string, 4> Binds(Pad pad) {
  switch (pad) {
    case Pad::A: return {REXCVAR_GET(keybind_a)};
    case Pad::B: return {REXCVAR_GET(keybind_b)};
    case Pad::X: return {REXCVAR_GET(keybind_x)};
    case Pad::Y: return {REXCVAR_GET(keybind_y)};
    case Pad::LB: return {REXCVAR_GET(keybind_left_shoulder)};
    case Pad::RB: return {REXCVAR_GET(keybind_right_shoulder)};
    case Pad::LT: return {REXCVAR_GET(keybind_left_trigger)};
    case Pad::RT: return {REXCVAR_GET(keybind_right_trigger)};
    case Pad::LSPress: return {REXCVAR_GET(keybind_lstick_press)};
    case Pad::RSPress: return {REXCVAR_GET(keybind_rstick_press)};
    case Pad::LStick:
      return {REXCVAR_GET(keybind_lstick_up), REXCVAR_GET(keybind_lstick_left),
              REXCVAR_GET(keybind_lstick_down), REXCVAR_GET(keybind_lstick_right)};
    case Pad::RStick:
      return {REXCVAR_GET(keybind_rstick_up), REXCVAR_GET(keybind_rstick_left),
              REXCVAR_GET(keybind_rstick_down), REXCVAR_GET(keybind_rstick_right)};
    case Pad::Start: return {REXCVAR_GET(keybind_start)};
    case Pad::Back: return {REXCVAR_GET(keybind_back)};
    case Pad::Guide: return {REXCVAR_GET(keybind_guide)};
    case Pad::DPad:
      return {REXCVAR_GET(keybind_dpad_up), REXCVAR_GET(keybind_dpad_left),
              REXCVAR_GET(keybind_dpad_down), REXCVAR_GET(keybind_dpad_right)};
  }
  return {};
}

// One key as drawn: its name ("W", "SPACE", ";"), or an arrow, or a mouse button.
struct Key {
  std::string text;
  int arrow = -1;  // 0 up, 1 left, 2 down, 3 right
  int mouse = -1;  // 0 left button, 1 right, 2 middle, 3 the mouse moving
  bool Valid() const { return !text.empty() || arrow >= 0 || mouse >= 0; }
};

std::string_view Trim(std::string_view s) {
  while (!s.empty() && s.front() == ' ')
    s.remove_prefix(1);
  while (!s.empty() && s.back() == ' ')
    s.remove_suffix(1);
  return s;
}

// The key of a bind: its first choice ("Semicolon,Space": the semicolon), with its modifiers
// ("Shift+Up": SHIFT+UP). The names are those of the bindings (rexglue-sdk src/ui/keybinds.cpp).
Key KeyOf(std::string_view bind) {
  std::string_view token = Trim(bind.substr(0, bind.find(',')));
  std::string mods;
  for (size_t plus; (plus = token.find('+')) != std::string_view::npos && plus > 0;) {
    const std::string_view head = token.substr(0, plus);
    if (head == "Shift")
      mods += "SHIFT+";
    else if (head == "Ctrl" || head == "Control")
      mods += "CTRL+";
    else if (head == "Alt")
      mods += "ALT+";
    else
      break;
    token.remove_prefix(plus + 1);
  }
  Key key;
  if (token.empty())
    return key;
  static constexpr std::string_view kArrows[] = {"Up", "Left", "Down", "Right"};
  static constexpr std::string_view kArrowWords[] = {"UP", "LEFT", "DOWN", "RIGHT"};
  for (int i = 0; i < 4; ++i)
    if (token == kArrows[i]) {
      if (mods.empty())
        key.arrow = i;
      else
        key.text = mods + std::string(kArrowWords[i]);
      return key;
    }
  static constexpr std::string_view kMouse[] = {"LMB", "RMB", "MMB"};
  for (int i = 0; i < 3; ++i)
    if (token == kMouse[i]) {
      key.mouse = i;
      return key;
    }
  static constexpr std::pair<std::string_view, std::string_view> kNames[] = {
      {"Semicolon", ";"},     {"Quote", "'"},        {"Comma", ","},         {"Period", "."},
      {"Slash", "/"},         {"Backslash", "\\"},   {"Minus", "-"},         {"Plus", "="},
      {"Backtick", "`"},      {"LBracket", "["},     {"RBracket", "]"},      {"Space", "SPACE"},
      {"Return", "ENTER"},    {"NumpadEnter", "ENTER"}, {"Escape", "ESC"},   {"Tab", "TAB"},
      {"Backspace", "BKSP"},  {"Delete", "DEL"},     {"Insert", "INS"},      {"Home", "HOME"},
      {"End", "END"},         {"PageUp", "PG UP"},   {"PageDown", "PG DN"},  {"Shift", "SHIFT"},
      {"Control", "CTRL"},    {"Ctrl", "CTRL"},      {"Alt", "ALT"},         {"CapsLock", "CAPS"},
      {"NumLock", "NUM"},     {"ScrollLock", "SCRL"}, {"PrintScreen", "PRT SC"},
      {"Pause", "PAUSE"},     {"NumpadPlus", "NUM +"}, {"NumpadMinus", "NUM -"},
      {"NumpadStar", "NUM *"}, {"NumpadSlash", "NUM /"},
  };
  std::string name;
  for (const auto& [from, to] : kNames)
    if (token == from)
      name = to;
  if (name.empty() && token.starts_with("Numpad") && token.size() == 7)
    name = "NUM " + std::string(token.substr(6));
  if (name.empty())
    for (char c : token)
      name += char(std::toupper(static_cast<unsigned char>(c)));
  key.text = mods + name;
  return key;
}

// --- The font ----------------------------------------------------------------------------------

struct Font {
  std::vector<unsigned char> data;
  stbtt_fontinfo info{};
  float cap = 0.7f;  // the height of a capital, in ems
  bool ready = false;
};

const Font& TheFont() {
  static Font font;
  static std::once_flag once;
  std::call_once(once, [] {
    const auto path = rex::filesystem::GetExecutableFolder() / "fonts" / "Roboto-Medium.ttf";
    std::ifstream file(path, std::ios::binary);
    font.data.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    if (font.data.empty() ||
        !stbtt_InitFont(&font.info, font.data.data(),
                        stbtt_GetFontOffsetForIndex(font.data.data(), 0))) {
      REXLOG_WARN("Button prompts: {} couldn't be read; keyboard keys stay controller buttons",
                  path.string());
      return;
    }
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    const float em = stbtt_ScaleForMappingEmToPixels(&font.info, 1.0f);
    if (stbtt_GetCodepointBox(&font.info, 'H', &x0, &y0, &x1, &y1))
      font.cap = float(y1) * em;
    font.ready = true;
  });
  return font;
}

// --- Drawing -----------------------------------------------------------------------------------

struct Colour {
  float r, g, b;
};
constexpr Colour kInk{30, 30, 30};          // outlines and names
constexpr Colour kSide{142, 142, 142};      // a key's side, below its face
constexpr Colour kFaceTop{255, 255, 255};   // a key's face, top to bottom
constexpr Colour kFaceBottom{212, 212, 212};
constexpr Colour kLit{47, 123, 246};        // the mouse button meant

Colour Mix(Colour a, Colour b, float t) {
  return {a.r + (b.r - a.r) * t, a.g + (b.g - a.g) * t, a.b + (b.b - a.b) * t};
}

// Where the drawing goes: the texture and the place it may cover.
struct Target {
  uint8_t* rgba;
  uint32_t width;
  int x0, y0, x1, y1;
};

// Lays `colour` over a pixel, covering `cover` of it (straight alpha, over what's there).
void Over(const Target& t, int x, int y, Colour colour, float cover) {
  if (cover <= 0.0f || x < t.x0 || y < t.y0 || x >= t.x1 || y >= t.y1)
    return;
  cover = std::min(cover, 1.0f);
  uint8_t* p = t.rgba + (size_t(y) * t.width + size_t(x)) * 4;
  const float below = p[3] / 255.0f * (1.0f - cover), alpha = cover + below;
  const float c[3] = {colour.r, colour.g, colour.b};
  for (int i = 0; i < 3; ++i)
    p[i] = uint8_t(std::lround((c[i] * cover + p[i] * below) / alpha));
  p[3] = uint8_t(std::lround(alpha * 255.0f));
}

// Fills what `inside(x, y)` holds of the box x0..x1, y0..y1, 4 x 4 samples a pixel, coloured by
// `colour(y)` (0 at the box's top, 1 at its bottom).
template <typename Inside, typename Shade>
void Fill(const Target& t, float x0, float y0, float x1, float y1, Inside inside, Shade colour) {
  for (int y = int(std::floor(y0)); y < int(std::ceil(y1)); ++y)
    for (int x = int(std::floor(x0)); x < int(std::ceil(x1)); ++x) {
      int hits = 0;
      for (int j = 0; j < 4; ++j)
        for (int i = 0; i < 4; ++i)
          hits += inside(x + (i + 0.5f) / 4.0f, y + (j + 0.5f) / 4.0f) ? 1 : 0;
      if (hits)
        Over(t, x, y, colour(std::clamp((y + 0.5f - y0) / std::max(y1 - y0, 1.0f), 0.0f, 1.0f)),
             hits / 16.0f);
    }
}

auto RoundRect(float x0, float y0, float x1, float y1, float r) {
  r = std::clamp(r, 0.0f, std::min(x1 - x0, y1 - y0) / 2);
  return [=](float x, float y) {
    if (x < x0 || x > x1 || y < y0 || y > y1)
      return false;
    const float dx = std::max({x0 + r - x, x - (x1 - r), 0.0f});
    const float dy = std::max({y0 + r - y, y - (y1 - r), 0.0f});
    return dx * dx + dy * dy <= r * r;
  };
}

auto Solid(Colour c) {
  return [c](float) { return c; };
}

// A triangle pointing `dir` (0 up, 1 left, 2 down, 3 right) in the square of side s around cx, cy.
void Arrow(const Target& t, float cx, float cy, float s, int dir, Colour c) {
  static constexpr float kTip[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
  const float tx = kTip[dir][0], ty = kTip[dir][1], half = s / 2;
  const float ax = cx + tx * half, ay = cy + ty * half;          // the point
  const float bx = cx - tx * half * 0.6f - ty * half, by = cy - ty * half * 0.6f + tx * half;
  const float qx = cx - tx * half * 0.6f + ty * half, qy = cy - ty * half * 0.6f - tx * half;
  const auto side = [](float px, float py, float x0, float y0, float x1, float y1) {
    return (x1 - x0) * (py - y0) - (y1 - y0) * (px - x0);
  };
  Fill(t, cx - half, cy - half, cx + half, cy + half,
       [=](float x, float y) {
         const float d0 = side(x, y, ax, ay, bx, by), d1 = side(x, y, bx, by, qx, qy),
                     d2 = side(x, y, qx, qy, ax, ay);
         return (d0 >= 0 && d1 >= 0 && d2 >= 0) || (d0 <= 0 && d1 <= 0 && d2 <= 0);
       },
       Solid(c));
}

// The font's scale for capitals px pixels tall.
float Scale(const Font& font, float px) {
  return px / std::max(font.cap, 0.01f) * stbtt_ScaleForMappingEmToPixels(&font.info, 1);
}

float TextWidth(const Font& font, std::string_view text, float px) {
  const float scale = Scale(font, px);
  float w = 0;
  for (char ch : text) {
    int advance = 0, bearing = 0;
    stbtt_GetCodepointHMetrics(&font.info, static_cast<unsigned char>(ch), &advance, &bearing);
    w += advance * scale;
  }
  return w;
}

// Writes `text` with capitals `px` pixels tall, centred on cx, cy.
void Text(const Target& t, std::string_view text, float cx, float cy, float px, Colour c) {
  const Font& font = TheFont();
  if (!font.ready || text.empty())
    return;
  const float scale = Scale(font, px);
  float pen = cx - TextWidth(font, text, px) / 2;
  const float baseline = cy + px / 2;
  std::vector<unsigned char> glyph;
  for (char ch : text) {
    const int code = static_cast<unsigned char>(ch);
    int advance = 0, bearing = 0;
    stbtt_GetCodepointHMetrics(&font.info, code, &advance, &bearing);
    const float shift_x = pen - std::floor(pen), shift_y = baseline - std::floor(baseline);
    int x0 = 0, y0 = 0, x1 = 0, y1 = 0;
    stbtt_GetCodepointBitmapBoxSubpixel(&font.info, code, scale, scale, shift_x, shift_y, &x0, &y0,
                                        &x1, &y1);
    const int w = x1 - x0, h = y1 - y0;
    if (w > 0 && h > 0) {
      glyph.assign(size_t(w) * h, 0);
      stbtt_MakeCodepointBitmapSubpixel(&font.info, glyph.data(), w, h, w, scale, scale, shift_x,
                                        shift_y, code);
      const int ox = int(std::floor(pen)) + x0, oy = int(std::floor(baseline)) + y0;
      for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
          Over(t, ox + x, oy + y, c, glyph[size_t(y) * w + x] / 255.0f);
    }
    pen += advance * scale;
  }
}

// A key cap filling x, y, w, h: a dark outline, a grey side below a white face, the key's name or
// arrow on the face.
void KeyCap(const Target& t, float x, float y, float w, float h, const Key& key) {
  const float line = std::max(1.0f, h * 0.06f), side = std::max(1.0f, h * 0.1f), r = h * 0.2f;
  Fill(t, x, y, x + w, y + h, RoundRect(x, y, x + w, y + h, r), Solid(kInk));
  Fill(t, x, y, x + w, y + h, RoundRect(x + line, y + line, x + w - line, y + h - line, r - line),
       Solid(kSide));
  const float fx0 = x + line, fy0 = y + line, fx1 = x + w - line, fy1 = y + h - line - side;
  Fill(t, fx0, fy0, fx1, fy1, RoundRect(fx0, fy0, fx1, fy1, r - line),
       [](float v) { return Mix(kFaceTop, kFaceBottom, v); });
  const float cx = (fx0 + fx1) / 2, cy = (fy0 + fy1) / 2, fh = fy1 - fy0, fw = fx1 - fx0;
  if (key.arrow >= 0) {
    Arrow(t, cx, cy, fh * 0.52f, key.arrow, kInk);
    return;
  }
  float px = fh * 0.56f;
  if (const float tw = TextWidth(TheFont(), key.text, px); tw > fw * 0.84f)
    px *= fw * 0.84f / tw;
  Text(t, key.text, cx, cy, px, kInk);
}

// The width a key cap of height h wants for its name.
float KeyWidth(const Key& key, float h) {
  if (key.arrow >= 0 || key.text.size() <= 1)
    return h;
  const float face = h * (1.0f - 0.22f);
  return std::max(h, TextWidth(TheFont(), key.text, face * 0.56f) / 0.84f + h * 0.16f);
}

// A mouse filling (as tall as fits) x, y, w, h: the button meant lit; `which` 3: the mouse moving
// (the right stick), arrows at its sides.
void Mouse(const Target& t, float x, float y, float w, float h, int which) {
  const float bh = std::min(h, w / (which == 3 ? 1.25f : 0.68f)), bw = bh * 0.68f;
  const float bx = x + (w - bw) / 2, by = y + (h - bh) / 2;
  const float line = std::max(1.0f, bh * 0.05f), r = bw * 0.48f;
  Fill(t, bx, by, bx + bw, by + bh, RoundRect(bx, by, bx + bw, by + bh, r), Solid(kInk));
  const auto body = RoundRect(bx + line, by + line, bx + bw - line, by + bh - line, r - line);
  Fill(t, bx, by, bx + bw, by + bh, body, [](float v) { return Mix(kFaceTop, kFaceBottom, v); });
  const float split = by + bh * 0.42f, mid = bx + bw / 2;
  if (which == 0 || which == 1)
    Fill(t, bx, by, bx + bw, split,
         [=](float px, float py) {
           return body(px, py) && py < split && (which == 0 ? px < mid : px > mid);
         },
         Solid(kLit));
  const auto all = [](float, float) { return true; };
  Fill(t, mid - line / 2, by, mid + line / 2, split, all, Solid(kInk));
  Fill(t, bx, split - line / 2, bx + bw, split + line / 2,
       [=](float px, float py) { return body(px, py); }, Solid(kInk));
  const float ww = bw * 0.16f, wy0 = by + bh * 0.12f, wy1 = split - bh * 0.07f;
  Fill(t, mid - ww / 2, wy0, mid + ww / 2, wy1,
       RoundRect(mid - ww / 2, wy0, mid + ww / 2, wy1, ww / 2),
       Solid(which == 2 ? kLit : kInk));
  if (which == 3) {
    const float s = bh * 0.2f, cy = by + bh * 0.62f;
    Arrow(t, bx - s * 0.75f, cy, s, 1, kInk);
    Arrow(t, bx + bw + s * 0.75f, cy, s, 3, kInk);
  }
}

// One key, as large as it fits in x, y, w, h, in the middle (`fill`: the whole place).
void OneKey(const Target& t, float x, float y, float w, float h, bool fill, const Key& key) {
  if (key.mouse >= 0) {
    Mouse(t, x, y, w, h, key.mouse);
    return;
  }
  float kh = h, kw = KeyWidth(key, kh);
  if (!fill && kw > w) {
    if (key.arrow >= 0 || key.text.size() <= 1)
      kh = kw = w;  // square keys stay square
    else
      kw = w;
  }
  if (fill)
    kw = w, kh = h;
  KeyCap(t, x + (w - kw) / 2, y + (h - kh) / 2, kw, kh, key);
}

// A stick's or the D-pad's four keys (up, left, down, right) in a W A S D block; in a place too
// small for that, one key with the four names in a row (or an arrow, all arrows).
void FourKeys(const Target& t, float x, float y, float w, float h, bool fill,
              const std::array<Key, 4>& keys) {
  float kx = std::min(w / 3, h / 2), ky = kx;
  if (fill)
    kx = w / 3, ky = h / 2;
  if (std::min(kx, ky) >= 16.0f) {
    const float left = x + (w - 3 * kx) / 2, top = y + (h - 2 * ky) / 2, gap = kx * 0.06f;
    const float at[4][2] = {{1, 0}, {0, 1}, {1, 1}, {2, 1}};
    for (int i = 0; i < 4; ++i)
      if (keys[i].Valid())
        KeyCap(t, left + at[i][0] * kx + gap / 2, top + at[i][1] * ky + gap / 2, kx - gap,
               ky - gap, keys[i]);
    return;
  }
  Key one;
  bool letters = true;
  for (const Key& key : keys)
    letters = letters && key.text.size() == 1;
  if (letters)
    for (int i : {0, 1, 2, 3})
      one.text += keys[i].text;
  else if (keys[0].arrow >= 0)
    one.arrow = 0;
  else
    one = keys[0];
  OneKey(t, x, y, w, h, fill, one);
}

}  // namespace

uint64_t BindingsSignature() {
  uint64_t h = 0xCBF29CE484222325ull;
  const auto add = [&](std::string_view s) {
    for (char c : s)
      h = (h ^ static_cast<unsigned char>(c)) * 0x100000001B3ull;
    h = (h ^ 0xFF) * 0x100000001B3ull;
  };
  for (int p = 0; p <= int(Pad::DPad); ++p)
    for (const std::string& bind : Binds(Pad(p)))
      add(bind);
  add(REXCVAR_GET(mnk_mouse) ? "mouse" : "keys");
  return h;
}

bool Bound(Pad pad) {
  if (!TheFont().ready)
    return false;
  if (pad == Pad::RStick && REXCVAR_GET(mnk_mouse))
    return true;
  const auto binds = Binds(pad);
  if (Four(pad))
    return std::ranges::any_of(binds, [](const std::string& b) { return KeyOf(b).Valid(); });
  return KeyOf(binds[0]).Valid();
}

void Draw(Pad pad, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool fill, uint8_t* rgba,
          uint32_t width, uint32_t height) {
  if (!rgba || !w || !h || !TheFont().ready)
    return;
  const Target t{rgba, width, int(x), int(y), int(std::min(x + w, width)),
                 int(std::min(y + h, height))};
  if (pad == Pad::RStick && REXCVAR_GET(mnk_mouse)) {
    Mouse(t, float(x), float(y), float(w), float(h), 3);
    return;
  }
  const auto binds = Binds(pad);
  if (Four(pad)) {
    std::array<Key, 4> keys;
    for (int i = 0; i < 4; ++i)
      keys[i] = KeyOf(binds[i]);
    FourKeys(t, float(x), float(y), float(w), float(h), fill, keys);
    return;
  }
  OneKey(t, float(x), float(y), float(w), float(h), fill, KeyOf(binds[0]));
}

}  // namespace svr::keys
