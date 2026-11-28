// PlayStation button pictures: see pad_icons.h. The same file in every game of the series; each
// game's textures are in its own pad_icons_tables.h.

#include "pad_icons.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>
#include <cstring>
#include <map>
#include <mutex>
#include <set>
#include <span>
#include <string>

#include <rex/cvar.h>
#include <rex/input/input_system.h>
#include <rex/logging.h>

#include "key_glyphs.h"
#include "pad_icons_art.h"

// stb_image reads the built-in PNGs (its own copy here: updater.cpp keeps another).
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif
#include <stb_image.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

REXCVAR_DEFINE_INT32(svr_button_prompts, 0, "Game",
                     "Button pictures in the game's prompts: 0 = auto (those of the controller in "
                     "use: PlayStation buttons for a DualShock or DualSense, keys for the "
                     "keyboard, "
                     "Xbox otherwise), 1 = Xbox, 2 = PlayStation, 3 = keyboard")
    .range(0, 3);

namespace svr {
namespace {

// A place on a texture (x, y, width, height).
struct Cell {
  uint32_t x, y, w, h;
};

// The PlayStation buttons of res/pad_icons/buttons.png, one 128 x 128 square each in this order
// (tools/pad_icons/ps3_style.ps1, ButtonSheet); also the controller's buttons for the keyboard's
// keys (key_glyphs.h, the same order: Cross is A, Circle B, Square X, Triangle Y, L1 LB, and so
// on).
enum class Btn : uint8_t {
  Cross, Circle, Square, Triangle, L1, R1, L2, R2, L3, R3, LStick, RStick, Start, Select, PS, DPad,
  kCount
};
constexpr uint32_t kSheetCell = 128;

// An icon page of the fonts: its size and hashes (the FNV-1a 64 of its base level as stored on
// the disc and with its 16-bit words swapped: the renderer hands over the blocks in the order the
// GPU reads them, whichever way round the disc keeps them), its drawing (res/pad_icons/<png>.png,
// the page's size), the places cleared and given the drawing, and places the drawing is only laid
// over (the game's picture around it, an arrow say, stays). For the keyboard: the button of each
// place (`keys`, one for each of `cells`; none: the page keeps its Xbox buttons) and of each
// overlay (`overlay_keys`), drawn as keys there.
struct Page {
  uint32_t width, height;
  std::span<const uint64_t> hashes;
  const char* png;
  std::span<const Cell> cells, overlays;
  std::span<const Btn> keys{}, overlay_keys{};
};

// A picture of its own, replaced whole by the drawing of the same size (res/pad_icons/<png>.png).
// For the keyboard: the button it shows (`key`) drawn as its key over the whole picture; none
// (a controller's picture): the game's own stays.
struct Whole {
  uint32_t width, height;
  uint64_t hashes[2];
  const char* png;
  Btn key = Btn::kCount;
};

// A button on one of the game's other pictures (an in-match prompt, say): its place, cleared, and
// the PlayStation button drawn into it as large as it fits, in the middle; or, with `fill`,
// stretched to the whole place (a picture the game draws stretched, its buttons stored squeezed
// to match). (w == 0: no button.)
struct Stamp {
  uint16_t x, y, w, h;
  Btn button;
  bool fill = false;
};
// One such picture: its size, hashes (as for a Page), a name for the log, and its buttons.
struct Prompt {
  uint32_t width, height;
  uint64_t hashes[2];
  const char* name;
  Stamp stamps[6];
};

using enum Btn;
#include "pad_icons_tables.h"

// Which buttons the pictures show.
enum class Kind { kXbox, kPlayStation, kKeyboard };
constexpr const char* kKindNames[] = {"Xbox", "PlayStation", "keyboard"};

std::mutex g_mutex;
std::set<uint32_t> g_textures;   // guest textures found to be one of the pictures
Kind g_built = Kind::kXbox;      // what they were last built with
uint64_t g_built_bindings = 0;   // and the keyboard's bindings then

Kind WantedKind() {
  switch (REXCVAR_GET(svr_button_prompts)) {
    case 1:
      return Kind::kXbox;
    case 2:
      return Kind::kPlayStation;
    case 3:
      return Kind::kKeyboard;
    default:
      switch (rex::input::LastUsedPadFamily()) {
        case rex::input::PadFamily::kPlayStation:
          return Kind::kPlayStation;
        case rex::input::PadFamily::kKeyboard:
          return Kind::kKeyboard;
        default:
          return Kind::kXbox;
      }
  }
}

keys::Pad KeyOf(Btn b) { return keys::Pad(b); }

// Clears the place of a button and draws its key there (an unbound button keeps the game's).
void DrawKey(Btn b, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool fill,
             std::vector<uint8_t>& rgba, uint32_t width, uint32_t height, bool clear) {
  if (!keys::Bound(KeyOf(b)))
    return;
  if (clear)
    for (uint32_t yy = y; yy < y + h && yy < height; ++yy)
      for (uint32_t xx = x; xx < x + w && xx < width; ++xx)
        std::memset(rgba.data() + (size_t(yy) * width + xx) * 4, 0, 4);
  keys::Draw(KeyOf(b), x, y, w, h, fill, rgba.data(), width, height);
}

uint64_t Fnv1a64(const uint8_t* blocks, size_t row_bytes, size_t rows, size_t stride) {
  uint64_t h = 0xCBF29CE484222325ull;
  for (size_t r = 0; r < rows; ++r) {
    const uint8_t* p = blocks + r * stride;
    for (size_t i = 0; i < row_bytes; ++i)
      h = (h ^ p[i]) * 0x100000001B3ull;
  }
  return h;
}

// One DXT5 (BC3) block, or DXT3 (BC2: `dxt3`, 4-bit alpha), in the order the GPU reads it, into
// a 4x4 corner of `rgba` (row length `width` pixels).
void DecodeBlock(const uint8_t* s, uint8_t* rgba, uint32_t width, bool dxt3) {
  uint8_t alpha[8] = {s[0], s[1]};
  if (alpha[0] > alpha[1]) {
    for (int i = 1; i < 7; ++i)
      alpha[i + 1] = uint8_t(((7 - i) * alpha[0] + i * alpha[1]) / 7);
  } else {
    for (int i = 1; i < 5; ++i)
      alpha[i + 1] = uint8_t(((5 - i) * alpha[0] + i * alpha[1]) / 5);
    alpha[6] = 0;
    alpha[7] = 255;
  }
  uint64_t alpha_bits = 0;
  for (int i = 0; i < 6; ++i)
    alpha_bits |= uint64_t(s[2 + i]) << (8 * i);
  const uint16_t c0 = uint16_t(s[8] | s[9] << 8), c1 = uint16_t(s[10] | s[11] << 8);
  uint8_t color[4][3];
  const auto expand = [](uint16_t c, uint8_t* out) {
    out[0] = uint8_t(((c >> 11) & 31) * 255 / 31);
    out[1] = uint8_t(((c >> 5) & 63) * 255 / 63);
    out[2] = uint8_t((c & 31) * 255 / 31);
  };
  expand(c0, color[0]);
  expand(c1, color[1]);
  for (int k = 0; k < 3; ++k) {
    color[2][k] = uint8_t((2 * color[0][k] + color[1][k]) / 3);
    color[3][k] = uint8_t((color[0][k] + 2 * color[1][k]) / 3);
  }
  const uint32_t color_bits = uint32_t(s[12] | s[13] << 8 | s[14] << 16 | uint32_t(s[15]) << 24);
  for (int p = 0; p < 16; ++p) {
    uint8_t* out = rgba + (size_t(p / 4) * width + (p % 4)) * 4;
    const uint8_t* c = color[(color_bits >> (2 * p)) & 3];
    out[0] = c[0];
    out[1] = c[1];
    out[2] = c[2];
    out[3] = dxt3 ? uint8_t(((s[p / 2] >> (4 * (p % 2))) & 15) * 17)
                  : alpha[(alpha_bits >> (3 * p)) & 7];
  }
}

// The whole texture, decoded into `rgba`.
void Decode(const uint8_t* blocks, size_t stride, uint32_t width, uint32_t height, bool dxt3,
            std::vector<uint8_t>& rgba) {
  rgba.assign(size_t(width) * height * 4, 0);
  for (uint32_t by = 0; by < height / 4; ++by)
    for (uint32_t bx = 0; bx < width / 4; ++bx)
      DecodeBlock(blocks + by * stride + bx * 16,
                  rgba.data() + (size_t(by) * 4 * width + bx * 4) * 4, width, dxt3);
}

// A built-in drawing (RGBA8), decoded once; null if it isn't width x height.
const std::vector<uint8_t>* Art(const char* name, uint32_t width, uint32_t height) {
  static std::mutex mutex;
  static std::map<std::string, std::vector<uint8_t>> decoded;
  std::lock_guard lock(mutex);
  auto [it, added] = decoded.try_emplace(name);
  if (added) {
    for (const PadIconsPng& png : kPadIconsPngs) {
      if (std::strcmp(png.name, name) != 0)
        continue;
      int w = 0, h = 0, n = 0;
      if (uint8_t* px = stbi_load_from_memory(png.data, int(png.size), &w, &h, &n, 4)) {
        it->second.assign(px, px + size_t(w) * h * 4);
        stbi_image_free(px);
        if (uint32_t(w) != width || uint32_t(h) != height)
          it->second.clear();
      }
    }
    if (it->second.empty())
      REXLOG_WARN("Button prompts: the built-in picture {} couldn't be read", name);
  }
  return it->second.empty() ? nullptr : &it->second;
}

// The sheet of all the buttons, and the drawn part of each one's square (its shadow included).
struct Sheet {
  const std::vector<uint8_t>* rgba = nullptr;
  std::array<Cell, size_t(kCount)> drawn{};
};
const Sheet* ButtonSheet() {
  static std::once_flag once;
  static Sheet sheet;
  std::call_once(once, [] {
    constexpr uint32_t width = kSheetCell * uint32_t(kCount);
    sheet.rgba = Art("buttons", width, kSheetCell);
    if (!sheet.rgba)
      return;
    for (uint32_t i = 0; i < uint32_t(kCount); ++i) {
      uint32_t x0 = kSheetCell, y0 = kSheetCell, x1 = 0, y1 = 0;
      for (uint32_t y = 0; y < kSheetCell; ++y)
        for (uint32_t x = 0; x < kSheetCell; ++x)
          if ((*sheet.rgba)[(size_t(y) * width + i * kSheetCell + x) * 4 + 3] > 8) {
            x0 = std::min(x0, x), y0 = std::min(y0, y);
            x1 = std::max(x1, x + 1), y1 = std::max(y1, y + 1);
          }
      if (x1 > x0)
        sheet.drawn[i] = {i * kSheetCell + x0, y0, x1 - x0, y1 - y0};
    }
  });
  return sheet.rgba ? &sheet : nullptr;
}

// Draws a button as large as it fits in the place `s` of `rgba` (`width` pixels a row), in the
// middle, over what's there: each pixel the average of a grid of samples of the drawing, enough of
// them to cover the drawing's pixels it stands for.
void DrawButton(const Sheet& sheet, const Stamp& s, std::vector<uint8_t>& rgba, uint32_t width,
                uint32_t height) {
  const Cell& d = sheet.drawn[size_t(s.button)];
  if (!d.w || !s.w || !s.h)
    return;
  float scale_x = float(s.w) / float(d.w), scale_y = float(s.h) / float(d.h);
  if (!s.fill)
    scale_x = scale_y = std::min(scale_x, scale_y);
  const float x0 = s.x + (s.w - d.w * scale_x) / 2, y0 = s.y + (s.h - d.h * scale_y) / 2;
  const int n = std::clamp(int(std::ceil(2.0f / std::min(scale_x, scale_y))), 1, 12);
  const size_t sheet_width = kSheetCell * size_t(kCount);
  for (uint32_t y = s.y; y < uint32_t(s.y + s.h) && y < height; ++y)
    for (uint32_t x = s.x; x < uint32_t(s.x + s.w) && x < width; ++x) {
      float sum[4] = {};
      for (int j = 0; j < n; ++j)
        for (int i = 0; i < n; ++i) {
          const float sx = (x + (i + 0.5f) / n - x0) / scale_x;
          const float sy = (y + (j + 0.5f) / n - y0) / scale_y;
          if (sx < 0 || sy < 0 || sx >= d.w || sy >= d.h)
            continue;
          const size_t at = (d.y + size_t(sy)) * sheet_width + d.x + size_t(sx);
          const uint8_t* p = sheet.rgba->data() + at * 4;
          const float a = p[3];
          sum[0] += p[0] * a, sum[1] += p[1] * a, sum[2] += p[2] * a, sum[3] += a;
        }
      if (sum[3] <= 0)
        continue;
      const float k = sum[3] / float(n * n) / 255.0f;  // how much of this pixel the drawing covers
      uint8_t* out = rgba.data() + (size_t(y) * width + x) * 4;
      for (int c = 0; c < 3; ++c)
        out[c] = uint8_t(std::lround(sum[c] / sum[3] * k + out[c] * (1 - k)));
      out[3] = uint8_t(std::lround(255 * k + out[3] * (1 - k)));
    }
}

// Notes a texture found to be one of the pictures; which buttons are wanted. Logs the first few
// times each picture is found.
Kind Found(uint32_t guest_va, const char* what) {
  static std::map<std::string, int> logged;
  const Kind kind = WantedKind();
  const uint64_t bindings = kind == Kind::kKeyboard ? keys::BindingsSignature() : 0;
  std::lock_guard lock(g_mutex);
  if (g_textures.insert(guest_va).second && logged[what]++ < 3)
    REXLOG_INFO("Button prompts: {} at 0x{:08X} ({} buttons)", what, guest_va,
                kKindNames[int(kind)]);
  g_built = kind;
  g_built_bindings = bindings;
  return kind;
}

// The pictures of kWhole and kPrompts: whether the texture (its hash) is one of them and wanted
// with other buttons than the game's; if so, fills `rgba` with it as it should look. `decode`
// fills `rgba` with the game's picture.
bool Picture(uint32_t guest_va, uint32_t width, uint32_t height, uint64_t hash,
             const std::function<void(std::vector<uint8_t>&)>& decode,
             std::vector<uint8_t>& rgba) {
  const auto sized = [&](const auto& t) { return t.width == width && t.height == height; };
  for (const Whole& w : kWhole) {
    if (!w.png || !sized(w) || (hash != w.hashes[0] && hash != w.hashes[1]))
      continue;
    const Kind kind = Found(guest_va, w.png);
    if (kind == Kind::kKeyboard) {
      if (w.key == Btn::kCount || !keys::Bound(KeyOf(w.key)))
        return false;
      rgba.assign(size_t(width) * height * 4, 0);
      DrawKey(w.key, 0, 0, width, height, false, rgba, width, height, false);
      return true;
    }
    const std::vector<uint8_t>* art =
        kind == Kind::kPlayStation ? Art(w.png, width, height) : nullptr;
    if (!art)
      return false;
    rgba = *art;
    return true;
  }
  for (const Prompt& p : kPrompts) {
    if (!p.name || !sized(p) || (hash != p.hashes[0] && hash != p.hashes[1]))
      continue;
    const Kind kind = Found(guest_va, p.name);
    if (kind == Kind::kKeyboard) {
      // The game's picture, each Xbox button's place cleared and its key drawn there.
      decode(rgba);
      for (const Stamp& s : p.stamps)
        if (s.w)
          DrawKey(s.button, s.x, s.y, s.w, s.h, s.fill, rgba, width, height, true);
      return true;
    }
    const Sheet* sheet = kind == Kind::kPlayStation ? ButtonSheet() : nullptr;
    if (!sheet)
      return false;
    // The game's picture, each Xbox button's place cleared, then the PlayStation buttons drawn.
    decode(rgba);
    for (const Stamp& s : p.stamps)
      for (uint32_t y = s.y; y < uint32_t(s.y + s.h) && y < height; ++y)
        for (uint32_t x = s.x; x < uint32_t(s.x + s.w) && x < width; ++x)
          std::memset(rgba.data() + (size_t(y) * width + x) * 4, 0, 4);
    for (const Stamp& s : p.stamps)
      DrawButton(*sheet, s, rgba, width, height);
    return true;
  }
  return false;
}

}  // namespace

bool PadIconsPage(uint32_t guest_va, uint32_t width, uint32_t height, const uint8_t* blocks,
                  size_t stride, std::vector<uint8_t>& rgba, bool dxt3) {
  if (!blocks || width % 4 || height % 4)
    return false;
  // (Candidates by size: the hash decides.)
  const auto sized = [&](const auto& t) { return t.width == width && t.height == height; };
  const bool page = std::ranges::any_of(kPages, sized);
  if (!page && !std::ranges::any_of(kWhole, sized) && !std::ranges::any_of(kPrompts, sized))
    return false;
  const uint64_t hash = Fnv1a64(blocks, width / 4 * 16, height / 4, stride);

  for (const Page& p : kPages) {
    if (!sized(p) || std::ranges::find(p.hashes, hash) == p.hashes.end())
      continue;
    const Kind kind = Found(guest_va, "icon page");
    if (kind == Kind::kKeyboard) {
      if (p.keys.size() != p.cells.size())
        return false;
      // The game's page, each Xbox button's place cleared and its key drawn there; the overlays'
      // keys drawn over what's there.
      Decode(blocks, stride, width, height, dxt3, rgba);
      for (size_t i = 0; i < p.cells.size(); ++i) {
        const Cell& c = p.cells[i];
        DrawKey(p.keys[i], c.x, c.y, c.w, c.h, false, rgba, width, height, true);
      }
      for (size_t i = 0; i < p.overlays.size() && i < p.overlay_keys.size(); ++i) {
        const Cell& c = p.overlays[i];
        DrawKey(p.overlay_keys[i], c.x, c.y, c.w, c.h, false, rgba, width, height, false);
      }
      return true;
    }
    const std::vector<uint8_t>* art =
        kind == Kind::kPlayStation ? Art(p.png, width, height) : nullptr;
    if (!art)
      return false;
    // The game's page, then each Xbox button's place cleared and given the PlayStation drawing,
    // and the overlays laid over what's there.
    Decode(blocks, stride, width, height, dxt3, rgba);
    for (const Cell& c : p.cells)
      for (uint32_t y = c.y; y < c.y + c.h && y < height; ++y) {
        const size_t at = (size_t(y) * width + c.x) * 4;
        std::memcpy(rgba.data() + at, art->data() + at, size_t(std::min(c.w, width - c.x)) * 4);
      }
    for (const Cell& c : p.overlays)
      for (uint32_t y = c.y; y < c.y + c.h && y < height; ++y)
        for (uint32_t x = c.x; x < c.x + c.w && x < width; ++x) {
          uint8_t* d = rgba.data() + (size_t(y) * width + x) * 4;
          const uint8_t* s = art->data() + (size_t(y) * width + x) * 4;
          const uint32_t a = s[3];
          for (int i = 0; i < 3; ++i)
            d[i] = uint8_t((s[i] * a + d[i] * (255 - a)) / 255);
          d[3] = uint8_t(a + d[3] * (255 - a) / 255);
        }
    return true;
  }
  if (Picture(guest_va, width, height, hash,
              [&](std::vector<uint8_t>& out) { Decode(blocks, stride, width, height, dxt3, out); },
              rgba))
    return true;
  if (page) {
    // Other textures of a page's size, a few of them (should a page ever differ: another release
    // of the game).
    static std::atomic<int> logged{0};
    if (logged.fetch_add(1, std::memory_order_relaxed) < 6)
      REXLOG_INFO("Button prompts: a {}x{} DXT5 texture at 0x{:08X} isn't an icon page ({:016X})",
                  width, height, guest_va, hash);
  }
  return false;
}

bool PadIconsColour(uint32_t guest_va, uint32_t width, uint32_t height, uint32_t bytes_per_texel,
                    const uint8_t* texels, size_t stride,
                    const std::function<void(std::vector<uint8_t>&)>& decode,
                    std::vector<uint8_t>& rgba) {
  const auto sized = [&](const auto& t) { return t.width == width && t.height == height; };
  if (!texels || (!std::ranges::any_of(kWhole, sized) && !std::ranges::any_of(kPrompts, sized)))
    return false;
  const uint64_t hash = Fnv1a64(texels, size_t(width) * bytes_per_texel, height, stride);
  return Picture(guest_va, width, height, hash, decode, rgba);
}

std::vector<uint32_t> PadIconsStalePages() {
  const Kind kind = WantedKind();
  std::lock_guard lock(g_mutex);
  if (g_textures.empty())
    return {};
  // (The keyboard's bindings are checked only while its keys are shown: rebinding redraws them.)
  const uint64_t bindings = kind == Kind::kKeyboard ? keys::BindingsSignature() : 0;
  if (kind == g_built && bindings == g_built_bindings)
    return {};
  g_built = kind;
  g_built_bindings = bindings;
  REXLOG_INFO("Button prompts: now {} buttons", kKindNames[int(kind)]);
  std::vector<uint32_t> textures(g_textures.begin(), g_textures.end());
  g_textures.clear();  // found again as they're built
  return textures;
}

}  // namespace svr
