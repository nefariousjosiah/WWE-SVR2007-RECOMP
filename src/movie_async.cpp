// Arena screen videos (Bink) decoded on their own thread.
//
// The render thread (sub_82445240) runs the main thread's render commands, and one of them updates
// a playing movie (sub_824474F8: r3 = the command, +16 the movie, +24 its state: 0 opened, 1 next
// frame, 2 next frame and notify, 3 close). For a new frame it checks that the next of the
// movie's two frame buffers is free (sub_821C9390), has Bink decode into it (BinkDoFrame,
// sub_8270FF88), shows it (movie +264 = Bink's buffer index, +152), flushes it unless Bink is
// behind (sub_8270FB68, sub_821C8FC8) and moves on (BinkNextFrame, sub_82710528). The decode took
// about 2 ms of every frame in entrances on a desktop CPU and twice that on the Steam Deck, on the
// thread that also has to draw the frame: the Undertaker's entrance dropped frames there.
//
// Here state 1 shows the frame decoded at the previous update and starts the next one on a
// worker thread: the screens run one video frame behind, and the render thread doesn't wait.
// Bink writes the buffer that isn't on screen (the two alternate) and reads the one that is, as
// its reference. Every other command for a movie, and BinkClose (sub_827106E8) from anywhere,
// first waits for that movie's decode. The game's per-thread allocator selection around the
// decode (sub_82414380 / sub_82414BB0) stays on the render thread: unknown threads share the
// table's default slot, and Bink allocates when a movie opens, not per frame.

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <unordered_map>

#include <rex/cvar.h>
#include <rex/hook.h>
#include <rex/logging.h>
#include <rex/system/function_dispatcher.h>
#include <rex/system/kernel_state.h>
#include <rex/system/xthread.h>
#include <rex/types.h>

REXCVAR_DEFINE_BOOL(svr_async_movies, true, "Game",
                    "Decode the arena screens' videos on their own thread (off: on the render "
                    "thread, as the console did)");

REX_EXTERN(sub_821C9390);  // check the next frame buffer is free
REX_EXTERN(sub_8270FB68);  // Bink: behind schedule?
REX_EXTERN(sub_821C8FC8);  // flush the frame buffers
REX_EXTERN(sub_82710528);  // BinkNextFrame
REX_EXTERN(sub_82414380);  // the thread's allocator
REX_EXTERN(sub_82414BB0);  // select the thread's allocator
REX_EXTERN(__imp__sub_824474F8);
REX_EXTERN(__imp__sub_827106E8);

namespace {

constexpr uint32_t kBinkDoFrame = 0x8270FF88;

uint32_t Load32(const uint8_t* base, uint32_t va) {
  const uint8_t* p = base + va;
  return uint32_t(p[0]) << 24 | uint32_t(p[1]) << 16 | uint32_t(p[2]) << 8 | p[3];
}
void Store32(uint8_t* base, uint32_t va, uint32_t v) {
  uint8_t* p = base + va;
  p[0] = uint8_t(v >> 24);
  p[1] = uint8_t(v >> 16);
  p[2] = uint8_t(v >> 8);
  p[3] = uint8_t(v);
}

enum class Decode { kRunning, kDone };

struct Decoder {
  std::mutex mutex;
  std::condition_variable cv;
  std::deque<uint32_t> queue;                  // Binks to decode, in order
  std::unordered_map<uint32_t, Decode> state;  // Bink -> its decode (absent: none)
  rex::system::object_ref<rex::system::XHostThread> thread;
};
Decoder& decoder() {
  static Decoder d;
  return d;
}

int WorkerMain() {
  Decoder& d = decoder();
  auto* thread_state = rex::system::XThread::GetCurrentThread()->thread_state();
  auto* dispatcher = REX_KERNEL_STATE()->function_dispatcher();
  for (;;) {
    uint32_t bink;
    {
      std::unique_lock lock(d.mutex);
      d.cv.wait(lock, [&] { return !d.queue.empty(); });
      bink = d.queue.front();
      d.queue.pop_front();
    }
    uint64_t args[] = {bink};
    dispatcher->Execute(thread_state, kBinkDoFrame, args, 1);
    {
      std::lock_guard lock(d.mutex);
      d.state[bink] = Decode::kDone;
    }
    d.cv.notify_all();
  }
}

bool StartWorker() {
  Decoder& d = decoder();
  if (d.thread)
    return true;
  d.thread = rex::system::object_ref<rex::system::XHostThread>(
      new rex::system::XHostThread(REX_KERNEL_STATE(), 256 * 1024, 0, [] { return WorkerMain(); }));
  d.thread->set_name("SvR movie decode");
  using rex::X_STATUS;
  if (d.thread->Create() != X_STATUS_SUCCESS) {
    REXLOG_WARN("[movies] decode thread not started: videos decode on the render thread");
    d.thread.reset();
    return false;
  }
  REXLOG_INFO("[movies] arena screen videos decode on their own thread");
  return true;
}

// Waits for the Bink's decode, if one is running; true if one was started (it needs its
// follow-up: shown, flushed, BinkNextFrame).
bool Wait(uint32_t bink) {
  Decoder& d = decoder();
  std::unique_lock lock(d.mutex);
  auto it = d.state.find(bink);
  if (it == d.state.end())
    return false;
  d.cv.wait(lock, [&] { return d.state[bink] == Decode::kDone; });
  d.state.erase(bink);
  return true;
}

void Call(PPCContext& ctx, uint8_t* base, void (*fn)(PPCContext&, uint8_t*), uint32_t r3) {
  ctx.r3.u64 = r3;
  fn(ctx, base);
}

// The rest of an update after its decode: show the frame, flush unless behind, next frame.
void FinishFrame(PPCContext& ctx, uint8_t* base, uint32_t movie, uint32_t bink) {
  Store32(base, movie + 264, Load32(base, movie + 152));
  Call(ctx, base, sub_8270FB68, bink);
  if (ctx.r3.u32 == 0)
    Call(ctx, base, sub_821C8FC8, movie);
  Call(ctx, base, sub_82710528, bink);
}

}  // namespace

// A movie's render command (r3).
REX_HOOK_RAW(sub_824474F8) {
  const uint32_t command = ctx.r3.u32;
  const uint32_t movie = command ? Load32(base, command + 16) : 0;
  const uint32_t movie_state = command ? Load32(base, command + 24) : 0;
  const uint32_t bink = movie ? Load32(base, movie + 252) : 0;
  if (!bink) {
    __imp__sub_824474F8(ctx, base);
    return;
  }
  const bool async = movie_state == 1 && REXCVAR_GET(svr_async_movies) && StartWorker();
  // The game's own sequence around the update: its allocator for this movie, then back.
  Call(ctx, base, sub_82414380, 0);
  const uint32_t allocator = ctx.r3.u32;
  Call(ctx, base, sub_82414BB0, Load32(base, movie + 268));
  if (Wait(bink))
    FinishFrame(ctx, base, movie, bink);
  if (async) {
    Call(ctx, base, sub_821C9390, movie);
    Decoder& d = decoder();
    {
      std::lock_guard lock(d.mutex);
      d.state[bink] = Decode::kRunning;
      d.queue.push_back(bink);
    }
    d.cv.notify_all();
    Call(ctx, base, sub_82414BB0, allocator);
    return;
  }
  Call(ctx, base, sub_82414BB0, allocator);
  ctx.r3.u64 = command;
  __imp__sub_824474F8(ctx, base);
}

// BinkClose(r3), from any thread: never while its decode runs.
REX_HOOK_RAW(sub_827106E8) {
  const uint32_t bink = ctx.r3.u32;
  if (bink)
    Wait(bink);
  ctx.r3.u64 = bink;
  __imp__sub_827106E8(ctx, base);
}
