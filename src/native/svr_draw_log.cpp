// Native renderer dev aid: everything one frame draws, in order.
//
//   SVR_DRAW_LOG=7000   (frame index, counted in presents; logs/game.log gets a [drawlog] line per
//                        draw, clear and resolve of that frame)
//   F9 in game          logs the next frame the same way (svr_drawlog_key)
//   SVR_DRAW_LOG_ON_RESOLVE=384x360   every frame is kept in memory and written out only if it
//                        resolves into a texture of that size (a one-off capture, e.g. the
//                        Create-a-Superstar photo: press Retry to take it again)
//
// Per draw: the game's render target / depth surface and the host target they map to, viewport,
// scissor, shaders, and every bound texture (the game's D3D texture and its host counterpart).

#include "svr_draw_log.h"

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "core/logging.h"
#include "core/memory_helpers.h"
#include "gpu/d3d.h"
#include "gpu/device.h"
#include "gpu/resources.h"
#include "gpu/shaders/shader_constants.h"
#include "gpu/vertex_declaration.h"
#include "svr_resources.h"

namespace bd::gpu {
void SvrDumpPlane(u32 guest_va, const char *path);  // gpu/native_texture_mirror.cpp

namespace {

u64 TargetFrame() {
  static const u64 frame = [] {
    const char *env = std::getenv("SVR_DRAW_LOG");
    return env ? std::strtoull(env, nullptr, 0) : 0ull;
  }();
  return frame;
}

// SetTexture (sub_821D5058) keeps the bound D3D texture per slot at device + (slot + 3072) * 4.
constexpr u32 kBoundTextureTable = 3072u * 4u;

// SVR_DRAW_LOG_ON_RESOLVE=<w>x<h>: 0x0 when unset.
struct ResolveTrigger {
  u32 width = 0, height = 0;
};
const ResolveTrigger &Trigger() {
  static const ResolveTrigger trigger = [] {
    ResolveTrigger t;
    if (const char *env = std::getenv("SVR_DRAW_LOG_ON_RESOLVE"))
      std::sscanf(env, "%ux%u", &t.width, &t.height);
    return t;
  }();
  return trigger;
}
bool BufferMode() { return Trigger().width != 0; }

// Buffer mode: this frame's lines, written out if the frame resolves into the trigger size.
std::vector<std::string> g_lines;
u64 g_lines_frame = ~0ull;
bool g_flushing = false;

void NewFrameIfNeeded() {
  const u64 frame = SvrFrameIndex();
  if (frame != g_lines_frame) {
    g_lines_frame = frame;
    g_lines.clear();
    g_flushing = false;
  }
}

void Emit(std::string line) {
  if (BufferMode() && !g_flushing) {
    NewFrameIfNeeded();
    if (g_lines.size() < 20000)
      g_lines.push_back(std::move(line));
    return;
  }
  BD_INFO("{}", line);
}
#define DL_INFO(...) Emit(fmt::format(__VA_ARGS__))

std::string Host(const GuestTexture *t) {
  if (!t)
    return "-";
  return fmt::format("{}x{} f{}", t->width, t->height, u32(t->format));
}

u64 ShaderHash(const GuestShader *s) {
  return s && s->shaderCacheEntry ? s->shaderCacheEntry->hash : 0ull;
}

}  // namespace

std::atomic<u64> g_key_frame{0};  // the frame F9 asked for

bool SvrDrawLogActive() {
  if (BufferMode()) {
    NewFrameIfNeeded();
    return true;
  }
  const u64 frame = SvrFrameIndex();
  const u64 target = TargetFrame();
  return (target && frame == target) || frame == g_key_frame.load(std::memory_order_relaxed);
}

void SvrDrawLogNextFrame() {
  const u64 next = SvrFrameIndex() + 1;
  g_key_frame.store(next, std::memory_order_relaxed);
  BD_INFO("[drawlog] logging frame {} (F9)", next);
}

void SvrDrawLogDraw(u32 device_guest, const char *name, u32 primitive, bool indexed, u32 count,
                    u32 start, i32 base) {
  if (!SvrDrawLogActive())
    return;
  static u32 s_index = 0;
  const auto *dev = bd::mem::at<const D3DDevice>(device_guest);
  if (!dev)
    return;
  auto &s = state();
  std::string textures;
  for (u32 slot = 0; slot < kTextureSlots; ++slot) {
    const u32 va = bd::mem::load<u32>(device_guest + kBoundTextureTable + slot * 4u);
    if (!va && !s.textures[slot])
      continue;
    textures += fmt::format(" t{}=0x{:08X}[{}]", slot, va, Host(s.textures[slot]));
    // The fetch constant (D3DTexture+0x1C, 6 dwords): mip filter, min/max level, LOD bias.
    if (va && slot < 8) {
      // SVR_DRAW_LOG_PLANES=1: single-channel textures (video planes) also written as images.
      static const bool dump_planes = std::getenv("SVR_DRAW_LOG_PLANES") != nullptr;
      if (const auto *t = bd::mem::try_at<const D3DTexture>(va);
          t && dump_planes && (u32(t->Format.dword[0]) & 0x3F) == 2)
        SvrDumpPlane(va, fmt::format("logs/plane_{}_t{}_{:08X}", s_index, slot, va).c_str());
      if (const auto *t = bd::mem::try_at<const D3DTexture>(va))
        textures += fmt::format("{{fc {:08X} {:08X} {:08X} {:08X} {:08X} {:08X}}}",
                                u32(t->Format.dword[0]), u32(t->Format.dword[1]),
                                u32(t->Format.dword[2]), u32(t->Format.dword[3]),
                                u32(t->Format.dword[4]), u32(t->Format.dword[5]));
    }
  }
  // Register shadows: the 0x2100 block at +0x284C, the 0x2200 block at +0x28B4 (one dword each).
  const auto reg = [&](u32 r) -> u32 {
    const u32 off = r >= 0x2200 ? 0x28B4u + (r - 0x2200u) * 4u : 0x284Cu + (r - 0x2100u) * 4u;
    return bd::mem::load<u32>(device_guest + off);
  };
  const auto regf = [&](u32 r) {
    const u32 v = reg(r);
    float f;
    std::memcpy(&f, &v, 4);
    return f;
  };
  if (const auto *decl = s.vertex_declaration) {
    std::string elements;
    for (u32 i = 0; i < decl->inputElementCount; ++i) {
      const auto &e = decl->inputElements[i];
      if (e.slotIndex == 15)
        continue;
      elements += fmt::format(" {}{}:s{}+{}:f{}", e.semanticName ? e.semanticName : "?",
                              e.semanticIndex, e.slotIndex, e.alignedByteOffset, u32(e.format));
    }
    DL_INFO("[drawlog]   decl{}", elements);
  }
  DL_INFO("[drawlog]   blendctl 0x{:08X} colorctl 0x{:08X} alpharef {:.4g} | host blend {} {}/{} "
          "alphatest {} threshold {:.4g} zfunc {} zwrite {}",
          reg(0x2201), reg(0x2202), regf(0x210E), s.pipelineState.alphaBlendEnable ? "on" : "off",
          u32(s.pipelineState.srcBlend), u32(s.pipelineState.destBlend),
          (s.pipelineState.specConstants & kSpecConstantAlphaTest) ? "on" : "off",
          Video::AlphaThreshold(), u32(s.pipelineState.zFunc),
          s.pipelineState.zWriteEnable ? "on" : "off");
  DL_INFO("[drawlog]   vte 0x{:X} clip 0x{:X} mode 0x{:X} depthctl 0x{:08X} colormask 0x{:X} "
          "ps-c13 {:.4g},{:.4g},{:.4g},{:.4g} ps-c14 {:.4g},{:.4g},{:.4g},{:.4g}",
          reg(0x2206), reg(0x2204), reg(0x2205), reg(0x2200), reg(0x2104),
          float(dev->psFloatConstants[13][0]), float(dev->psFloatConstants[13][1]),
          float(dev->psFloatConstants[13][2]), float(dev->psFloatConstants[13][3]),
          float(dev->psFloatConstants[14][0]), float(dev->psFloatConstants[14][1]),
          float(dev->psFloatConstants[14][2]), float(dev->psFloatConstants[14][3]));
  DL_INFO("[drawlog] #{} {} prim {} {} count {} start {} base {} | rt 0x{:08X}[{}] ds 0x{:08X}[{}] "
          "| vp {:.0f},{:.0f} {:.0f}x{:.0f} z {:.2f}-{:.2f} sc {},{}-{},{} | vs {:016X} ps {:016X} "
          "|{}",
          s_index++, name, primitive, indexed ? "indexed" : "", count, start, base,
          u32(dev->renderTargetShadow[0]), Host(s.render_target), u32(dev->depthStencilShadow),
          Host(s.depth_stencil), float(dev->viewport.X), float(dev->viewport.Y),
          float(dev->viewport.Width), float(dev->viewport.Height), float(dev->viewport.MinZ),
          float(dev->viewport.MaxZ), i32(dev->scissorRect.left), i32(dev->scissorRect.top),
          i32(dev->scissorRect.right), i32(dev->scissorRect.bottom), ShaderHash(s.vertex_shader),
          ShaderHash(s.pixel_shader), textures);
}

void SvrDrawLogResolve(u32 device_guest, u32 flags, u32 source_rect_va, u32 dest_va,
                       u32 dest_point_va) {
  if (!SvrDrawLogActive())
    return;
  const auto *dev = bd::mem::at<const D3DDevice>(device_guest);
  std::string rect = "full";
  if (const auto *r = bd::mem::try_at<const D3DRect>(source_rect_va))
    rect = fmt::format("{},{}-{},{}", i32(r->left), i32(r->top), i32(r->right), i32(r->bottom));
  std::string point = "0,0";
  if (const auto *p = bd::mem::try_at<const be_i32>(dest_point_va))
    point = fmt::format("{},{}", i32(p[0]), i32(p[1]));
  u32 width = 0, height = 0;
  if (const auto *t = bd::mem::try_at<const D3DTexture>(dest_va)) {
    const u32 size = u32(t->Format.dword[2]);  // 2D: width - 1 [12:0], height - 1 [25:13]
    width = (size & 0x1FFF) + 1;
    height = ((size >> 13) & 0x1FFF) + 1;
  }
  DL_INFO("[drawlog] resolve flags 0x{:X} from rt 0x{:08X} rect {} -> 0x{:08X} ({}x{}) at {}",
          flags, dev ? u32(dev->renderTargetShadow[0]) : 0u, rect, dest_va, width, height, point);
  if (BufferMode() && !g_flushing && width == Trigger().width && height == Trigger().height) {
    BD_INFO("[drawlog] frame {}: resolve into {}x{}, its draws so far:", SvrFrameIndex(), width,
            height);
    for (const auto &line : g_lines)
      BD_INFO("{}", line);
    g_lines.clear();
    g_flushing = true;  // the rest of this frame is written out directly
  }
}

void SvrDrawLogResolveDone(u32 dest_va, GuestTexture *dst) {
  static const bool dump = std::getenv("SVR_DRAW_LOG_PLANES") != nullptr;
  if (!dump || !dst || BufferMode() || !SvrDrawLogActive())
    return;
  std::vector<u8> pixels;
  u32 w = 0, h = 0, row = 0;
  if (!Video::ReadBackTexture(dst, pixels, w, h, row))
    return;
  const std::string path = fmt::format("logs/resolve_{}_{:08X}.ppm", SvrFrameIndex(), dest_va);
  if (FILE *f = std::fopen(path.c_str(), "wb")) {
    std::fprintf(f, "P6\n%u %u\n255\n", w, h);
    for (u32 y = 0; y < h; ++y)
      for (u32 x = 0; x < w; ++x)
        std::fwrite(pixels.data() + size_t(y) * row + size_t(x) * 4, 1, 3, f);
    std::fclose(f);
    BD_INFO("[drawlog]   resolve 0x{:08X} read back: {}x{} -> {}", dest_va, w, h, path);
  }
  // The single-channel textures still bound (a video's planes), as the GPU has them.
  for (u32 slot = 0; slot < 8; ++slot) {
    GuestTexture *t = state().textures[slot];
    std::vector<u8> plane;
    u32 pw = 0, ph = 0, prow = 0;
    if (!t || t->format != plume::RenderFormat::R8_UNORM ||
        !Video::ReadBackTexture(t, plane, pw, ph, prow))
      continue;
    const std::string ppath = fmt::format("logs/gpu_{}_t{}.pgm", SvrFrameIndex(), slot);
    if (FILE *f = std::fopen(ppath.c_str(), "wb")) {
      std::fprintf(f, "P5\n%u %u\n255\n", pw, ph);
      for (u32 y = 0; y < ph; ++y)
        std::fwrite(plane.data() + size_t(y) * prow, 1, pw, f);
      std::fclose(f);
      BD_INFO("[drawlog]   bound t{} {} ({}x{}, descriptor {}) read back -> {}", slot,
              static_cast<const void *>(t), pw, ph, t->descriptorIndex, ppath);
    }
  }
}

void SvrDrawLogVertices(u32 data_va, u32 count, u32 stride) {
  if (!SvrDrawLogActive() || !data_va || !stride)
    return;
  for (u32 v = 0; v < count && v < 4; ++v) {
    std::string line;
    for (u32 k = 0; k < stride / 4 && k < 8; ++k)
      line += fmt::format(" {:08X}", bd::mem::load<u32>(data_va + v * stride + k * 4));
    DL_INFO("[drawlog]   v{}:{}", v, line);
  }
}

void SvrDrawLogClear(u32 device_guest, u32 count, u32 rects_va, u32 flags, u32 color) {
  if (!SvrDrawLogActive())
    return;
  const auto *dev = bd::mem::at<const D3DDevice>(device_guest);
  std::string rects;
  for (u32 i = 0; i < count && i < 8; ++i) {
    if (const auto *r = bd::mem::try_at<const D3DRect>(rects_va + i * 16u))
      rects += fmt::format(" {},{}-{},{}", i32(r->left), i32(r->top), i32(r->right),
                           i32(r->bottom));
  }
  DL_INFO("[drawlog] clear flags 0x{:X} color 0x{:08X} rt 0x{:08X} rects {}{}", flags, color,
          dev ? u32(dev->renderTargetShadow[0]) : 0u, count, rects);
}

}  // namespace bd::gpu
