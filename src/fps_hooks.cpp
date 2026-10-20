// Frame-rate hooks for WWE SmackDown vs. Raw 2007.
//
// The game's statically linked Xbox 360 D3D Swap reads the device's present interval
// (D3DPRESENT_INTERVAL_*) and turns it into the number of vblanks each frame stays on screen:
// ONE (1) -> every vblank (60 fps), TWO (2) -> every second vblank (30 fps).
//
// SvrPresentIntervalHook runs right after that load (2008: device+13572 at 0x8223A56C; not hooked
// in 2007 yet).

#include "frame_stats.h"

#include <rex/cvar.h>
#include <rex/logging.h>
#include <rex/ppc.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>

REXCVAR_DEFINE_BOOL(svr_60fps, false, "Game",
                    "Keep the game's frame-rate mode at 60 (50 PAL) instead of dropping to 30 (25) "
                    "in matches and cutscenes; game timing follows the mode");

// The game keeps a frame-rate mode in a global struct at 0x8295D1A4: +8 target fps (60/50/30/25),
// +12 mode lock and per-frame time-step floats at +28/+48/+52, all set together by SetFrameRate
// (sub_824297D8), "drop to 30" (sub_82429A30) and "back to 60" (sub_82429AA8). Same design as
// 2008 (struct 0x82E6E458; functions 824707A8 / 82470A18 / 82470AC0).
// Overriding only the present interval made the game draw at 60 but still step its logic as if
// at 30 (entrances at double speed); keeping the mode at 60 changes both together.

void SvrPresentIntervalHook(PPCRegister& r10) {
  // Log each change of the interval D3D Swap uses (follows the frame-rate mode).
  static std::atomic<uint32_t> last_logged{0xFFFFFFFFu};
  uint32_t requested = r10.u32;
  if (last_logged.exchange(requested, std::memory_order_relaxed) != requested) {
    REXLOG_INFO("[fps] game present interval: {} ({})", requested,
                requested == 2 ? "30 fps" : requested <= 1 ? "60 fps" : "other");
  }
}

// SetFrameRate(fps), r3 = requested rate (sub_824297D8 entry).
void SvrSetFrameRateHook(PPCRegister& r3) {
  uint32_t requested = r3.u32;
  if (REXCVAR_GET(svr_60fps) && (requested == 30 || requested == 25)) {
    r3.u64 = requested == 30 ? 60 : 50;
    REXLOG_INFO("[fps] game set frame rate {}; using {}", requested, r3.u32);
  } else {
    REXLOG_INFO("[fps] game set frame rate {}", requested);
  }
}

// Gameplay (entrances, matches): the game asks for its 30 fps mode when one starts (sub_82429A30,
// from sub_824BB848) and for 60 again when it ends (sub_82429AA8, from sub_824BD440), whether or
// not the drop is skipped. src/loading_hooks.cpp keeps its loading speed-ups to the rest.
namespace {
std::atomic<bool> g_gameplay{false};
std::atomic<uint32_t> g_gameplay_switches{0};
void SetGameplay(bool on) {
  if (g_gameplay.exchange(on, std::memory_order_relaxed) != on &&
      g_gameplay_switches.fetch_add(1, std::memory_order_relaxed) < 40)
    REXLOG_INFO("[fps] {} gameplay", on ? "entering" : "leaving");
}
}  // namespace

bool SvrInGameplay() {
  return g_gameplay.load(std::memory_order_relaxed);
}

// "Drop to 30 (25) fps mode" (sub_82429A30 entry); returning true skips the function.
bool SvrSkip30ModeHook() {
  bool skip = REXCVAR_GET(svr_60fps);
  static std::atomic<uint32_t> calls{0};
  uint32_t n = calls.fetch_add(1, std::memory_order_relaxed);
  if (n < 20 || n % 100 == 0) {
    REXLOG_INFO("[fps] game switched to 30 fps mode{}", skip ? "; kept at 60" : "");
  }
  SetGameplay(true);
  return skip;
}

// "Back to 60 (50) fps mode" (sub_82429AA8 entry).
void SvrBack60ModeHook() {
  SetGameplay(false);
}

// Cutscene captions (SvR 2008 v1.0.8's fix). The caption display's update (sub_82227200, the
// same code as 2008's sub_82261650; called through its vtable at 0x82024148 with the cutscene's
// time in 60ths of a second) turns each caption's show and hide times into 60ths with 60 / target
// fps, which is right only at 30 fps: the scene data's times are 30 fps frames, so in the 60 fps
// mode every caption came at half its time and left early. Handed 30 (25) at those two loads of
// the target fps, it works the times out as at 30 fps. (2008 also had clip-by-clip voiced captions
// with two more such loads; 2007 has no such code.)
static void SvrCaptionFps(PPCRegister& fps) {
  if (fps.u32 >= 50)
    fps.u64 = fps.u32 / 2;
}
void SvrCaptionFpsShowHook(PPCRegister& r30) { SvrCaptionFps(r30); }
void SvrCaptionFpsHideHook(PPCRegister& r31) { SvrCaptionFps(r31); }

// Frame counter: SvrFrameSwapHook runs in the game's swap function (sub_821EAB50) right before it
// calls VdSwap at 0x821EACDC, once per presented frame.
namespace {
using Clock = std::chrono::steady_clock;
constexpr size_t kFrameHistory = 512;  // > 1 s of frames at any realistic rate
std::mutex frame_mutex;
Clock::time_point frame_times[kFrameHistory];
uint64_t frame_count = 0;
}  // namespace

void SvrTextEntryFrame();  // text_entry.cpp

void SvrFrameSwapHook() {
  {
    std::lock_guard lock(frame_mutex);
    frame_times[frame_count % kFrameHistory] = Clock::now();
    ++frame_count;
  }
  SvrTextEntryFrame();
}

SvrFrameStats GetSvrFrameStats() {
  std::lock_guard lock(frame_mutex);
  SvrFrameStats stats;
  stats.frames = frame_count;
  if (frame_count < 2) {
    return stats;
  }
  auto now = Clock::now();
  auto newest = frame_times[(frame_count - 1) % kFrameHistory];
  // Count frames presented within the last second.
  uint64_t available = std::min<uint64_t>(frame_count, kFrameHistory);
  uint64_t in_window = 0;
  Clock::time_point oldest = newest;
  for (uint64_t i = 1; i <= available; ++i) {
    auto t = frame_times[(frame_count - i) % kFrameHistory];
    if (now - t > std::chrono::seconds(1)) {
      break;
    }
    oldest = t;
    ++in_window;
  }
  stats.fps = double(in_window);
  if (in_window >= 2) {
    stats.frame_time_ms =
        std::chrono::duration<double, std::milli>(newest - oldest).count() / double(in_window - 1);
  }
  return stats;
}
