// Loading speed (SvR 2007; 2009's src/loading_hooks.cpp did the same).
//
// The loader thread holds the D3D device while loading and calls sub_8242A088 often: once the time
// since the last presented frame reaches the budget at +188, it hands the device to the main thread
// for one loading-screen frame and waits for it. Both are in hundredths of a second (timebase ticks
// / frequency * 100, the constant at 0x8203FB18), so a budget of 1 is 10 ms. On the Xbox 360 the
// DVD was the bottleneck anyway; here the data is there at once, and a small budget leaves the
// loader waiting for frames.
//
// The hook raises the budget the loader is compared against (never lowers it): the loading screen
// still gets a frame every 1/60 s, and the loader gets most of each frame.
//
// That budget was rarely the limit in SvR 2007 (it is 150 ms for most loads). The time went to two
// waits:
// - The render thread (sub_82445240) holds the game's device lock (renderer +160, taken by
//   sub_82429C90, released by sub_82429CE0) from just before it presents until it has drawn the
//   next frame, and the frame limiter slept inside Present, so inside that lock. The loader needs
//   the lock for each texture it builds, so it got one turn per frame (seconds of a match load).
//   The render thread now sleeps right after it releases the lock (SvrRenderLockReleasedHook).
// - The loader thread's loop (sub_821CBE18) sleeps 16 ms before every pass, and after a pass with
//   work it also waits for the main thread's next frame, so at 60 fps each pass took two frames.
//   While it has work it now sleeps 1 ms (SvrLoaderSleepHook): one pass per frame.
//
// Both only apply outside gameplay (SvrInGameplay, src/fps_hooks.cpp: menus and loading screens).
// The game streams during entrances and matches too, and there a loader holding the device made
// the render thread late for its frame: 55 fps in entrances on the Steam Deck. The game's own
// budget is kept there as well.

#include <rex/cvar.h>
#include <rex/logging.h>
#include <rex/ppc.h>

#include <algorithm>
#include <atomic>
#include <cstring>

REXCVAR_DEFINE_INT32(svr_load_slice_ms, 12, "Game",
                     "Milliseconds the loader may work per loading-screen frame (0 keeps the game's "
                     "budget)")
    .range(0, 15);
REXCVAR_DEFINE_BOOL(svr_pace_outside_lock, true, "Game",
                    "Frame limiter sleeps after the game releases its device lock, so loading gets "
                    "the device while the render thread waits");
REXCVAR_DEFINE_BOOL(svr_fast_loader_poll, true, "Game",
                    "Loader thread checks for more work after 1 ms instead of 16 ms while it has "
                    "work");

bool SvrInGameplay();  // src/fps_hooks.cpp

// sub_8242A088 at 0x8242A0D0 (fcmpu cr6,f0,f13): f0 = time since the last frame, f13 = budget,
// both in hundredths of a second.
void SvrLoaderBudgetHook(PPCRegister& f13) {
  // Log each budget the game sets, the first few times.
  static std::atomic<uint32_t> logged{0};
  static std::atomic<uint32_t> last_bits{0xFFFFFFFFu};
  const float budget = float(f13.f64);
  uint32_t bits;
  std::memcpy(&bits, &budget, 4);
  if (last_bits.exchange(bits, std::memory_order_relaxed) != bits &&
      logged.fetch_add(1, std::memory_order_relaxed) < 40)
    REXLOG_INFO("[loading] game budget {} ms", budget * 10.0f);
  const int slice = REXCVAR_GET(svr_load_slice_ms);
  if (slice > 0 && !SvrInGameplay())
    f13.f64 = std::max(f13.f64, slice / 10.0);
}

#if defined(SVR_NATIVE_RENDERER)
void SvrPaceFrameOutsideLock();  // src/reblue/gpu/present.cpp
#endif

// sub_82445240 at 0x82445450, right after sub_82429CE0 released the device lock: the frame limiter.
void SvrRenderLockReleasedHook() {
#if defined(SVR_NATIVE_RENDERER)
  if (REXCVAR_GET(svr_pace_outside_lock) && !SvrInGameplay())
    SvrPaceFrameOutsideLock();
#endif
}

namespace {
// The loader thread's last pass found work (only that thread runs these two hooks).
thread_local bool t_loader_had_work = false;
}  // namespace

// sub_821CBE18 at 0x821CBE90, about to call Sleep(r3 = 16) at the top of a pass.
void SvrLoaderSleepHook(PPCRegister& r3) {
  if (t_loader_had_work && REXCVAR_GET(svr_fast_loader_poll) && !SvrInGameplay())
    r3.u64 = 1;
  t_loader_had_work = false;
}

// sub_821CBE18 at 0x821CBEA0: this pass found work (sub_8241CAF0 returned nonzero).
void SvrLoaderWorkHook() {
  t_loader_had_work = true;
}
