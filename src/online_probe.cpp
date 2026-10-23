// Dev aid for netplay: logs the game's own Xbox LIVE menu state machine, so a log shows where an
// instance went the sign-in way (XamShowSigninUI, "not the current active gamer profile") and
// what the game saw when it decided.
//
// The machine (sub_821B5960, one call per frame while the online menu runs; its state is the
// word at +48, 0..60) starts a connection request on the online manager (the object at global
// 0x82964F4C) in state 1 and, in state 2, goes on to state 57 if [[manager + 60] + 32] == 1
// (sub_824598A8) and to state 3 (sign-in UI) if not. Each state change is logged once with those
// fields: "[online] state 2 -> 3 (manager 7FA31200: busy 0, link 7FA31300, link+32 0, mode 0)".

#include <cstdint>

#include <rex/hook.h>
#include <rex/logging.h>
#include <rex/types.h>

namespace {

constexpr uint32_t kManagerGlobal = 0x82964F4C;

uint32_t Load(uint8_t *base, uint32_t address) {
  return address ? uint32_t(*reinterpret_cast<rex::be<uint32_t> *>(base + address)) : 0;
}

} // namespace

// The online menu's state machine; r3 = the machine, state at +48.
REX_EXTERN(__imp__sub_821B5960);
REX_HOOK_RAW(sub_821B5960) {
  static uint32_t last_machine = 0;
  static uint32_t last_state = ~0u;
  const uint32_t machine = ctx.r3.u32;
  const uint32_t before = Load(base, machine + 48);
  __imp__sub_821B5960(ctx, base);
  const uint32_t after = Load(base, machine + 48);
  if (machine == last_machine && after == last_state && before == after)
    return;
  last_machine = machine;
  last_state = after;
  const uint32_t manager = Load(base, kManagerGlobal);
  const uint32_t link = Load(base, manager ? manager + 60 : 0);
  REXLOG_INFO("[online] state {} -> {} (manager {:08X}: busy {}, link {:08X}, link+32 {}, mode {})",
              before, after, manager, Load(base, manager ? manager + 192 : 0), link,
              Load(base, link ? link + 32 : 0), Load(base, manager ? manager + 176 : 0));
}

// The "find a match" search task (sub_8240A438, called each frame; state at +60, result code at
// +12, 1 = still fine, 2/6/7 = failed, results buffer pointer at +864). State 2 asks for the
// buffer size (expects 122), 3 sends XSessionSearchEx and expects 997 (pending), 4 waits for it,
// 5 is done. Logged on each state change: whether it failed, and what the buffer holds
// (XSESSION_SEARCHRESULT_HEADER: count, pointer to the results).
REX_EXTERN(__imp__sub_8240A438);
REX_HOOK_RAW(sub_8240A438) {
  static uint32_t last_task = 0;
  static uint32_t last_state = ~0u;
  static uint32_t last_status = ~0u;
  const uint32_t task = ctx.r3.u32;
  const uint32_t before = Load(base, task + 60);
  __imp__sub_8240A438(ctx, base);
  const uint32_t returned = ctx.r3.u32;
  const uint32_t after = Load(base, task + 60);
  const uint32_t status = Load(base, task + 12);
  if (task == last_task && after == last_state && status == last_status)
    return;
  last_task = task;
  last_state = after;
  last_status = status;
  const uint32_t buffer = Load(base, task + 864);
  REXLOG_INFO("[online] search task {:08X}: state {} -> {} (returned {}, status {}); results "
              "buffer {:08X}: {} result(s), array at {:08X}",
              task, before, after, returned, status, buffer, Load(base, buffer),
              Load(base, buffer ? buffer + 4 : 0));
}
