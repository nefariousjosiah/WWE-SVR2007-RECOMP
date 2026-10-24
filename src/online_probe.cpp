// Dev aid for netplay: logs the game's own Xbox LIVE menu state machine, so a log shows where an
// instance went the sign-in way (XamShowSigninUI, "not the current active gamer profile") and
// what the game saw when it decided.
//
// The machine (sub_821B5960, one call per frame while the online menu runs; its state is the
// word at +48, 0..60) starts a connection request on the online manager (the object at global
// 0x82964F4C) in state 1 and, in state 2, goes on to state 57 if [[manager + 60] + 32] == 1
// (sub_824598A8) and to state 3 (sign-in UI) if not. Each state change is logged once with those
// fields: "[online] state 2 -> 3 (manager 7FA31200: busy 0, link 7FA31300, link+32 0, mode 0)".
//
// The online code is DemonWare (bdNet, bdSession, nLobbyManager) under Yuke's gss match layer.
// Both keep their debug messages in the retail build but print them nowhere; the [dw] and [gss]
// hooks below put them in our log, so a stalled join says why in the game's own words.

#include <cstdint>
#include <cstdio>
#include <map>
#include <mutex>
#include <string>
#include <utility>

#include <rex/hook.h>
#include <rex/logging.h>
#include <rex/types.h>

namespace {

constexpr uint32_t kManagerGlobal = 0x82964F4C;

uint32_t Load(uint8_t *base, uint32_t address) {
  return address ? uint32_t(*reinterpret_cast<rex::be<uint32_t> *>(base + address)) : 0;
}

uint64_t Load64(uint8_t *base, uint32_t address) {
  return address ? uint64_t(*reinterpret_cast<rex::be<uint64_t> *>(base + address)) : 0;
}

// A guest C string, at most `max` characters.
std::string GuestString(uint8_t *base, uint32_t address, size_t max = 400) {
  std::string out;
  if (!address)
    return "(null)";
  for (size_t i = 0; i < max; ++i) {
    char c = char(base[address + i]);
    if (!c)
      break;
    out += (c == '\n' || c == '\r') ? ' ' : c;
  }
  return out;
}

// "bool __cdecl bdSession::handleJoinReply(const class bdReceivedMessage &)" -> "handleJoinReply".
std::string ShortFunction(const std::string &signature) {
  size_t paren = signature.find('(');
  std::string head = signature.substr(0, paren);
  size_t start = head.find_last_of(" :");
  return start == std::string::npos ? head : head.substr(start + 1);
}

// Log lines from one place: the first 20, then every 200th, so a message the game repeats each
// frame can't flood the log.
bool ShouldLog(uint32_t file, uint32_t line) {
  static std::mutex mutex;
  static std::map<std::pair<uint32_t, uint32_t>, uint32_t> counts;
  std::lock_guard lock(mutex);
  uint32_t n = ++counts[{file, line}];
  return n <= 20 || n % 200 == 0;
}

// printf for a guest varargs call: argument i of the call (r3 = 0) is in a register up to r10,
// then in the caller's parameter save area. Handles the conversions the gss messages use.
std::string GuestFormat(PPCContext &ctx, uint8_t *base, uint32_t format, int first_arg) {
  auto arg = [&](int i) -> uint64_t {
    switch (i) {
    case 0: return ctx.r3.u64;
    case 1: return ctx.r4.u64;
    case 2: return ctx.r5.u64;
    case 3: return ctx.r6.u64;
    case 4: return ctx.r7.u64;
    case 5: return ctx.r8.u64;
    case 6: return ctx.r9.u64;
    case 7: return ctx.r10.u64;
    default: return Load64(base, ctx.r1.u32 + 16 + 8 * i);
    }
  };
  std::string fmt = GuestString(base, format), out;
  int next = first_arg;
  for (size_t i = 0; i < fmt.size(); ++i) {
    if (fmt[i] != '%' || i + 1 >= fmt.size()) {
      out += fmt[i];
      continue;
    }
    size_t j = i + 1;
    while (j < fmt.size() && std::string("-+ #0123456789.lh").find(fmt[j]) != std::string::npos)
      ++j;
    if (j >= fmt.size())
      break;
    switch (fmt[j]) {
    case '%': out += '%'; break;
    case 'd': case 'i': out += std::to_string(int32_t(arg(next++))); break;
    case 'u': out += std::to_string(uint32_t(arg(next++))); break;
    case 'x': case 'X': case 'p': {
      char hex[16];
      std::snprintf(hex, sizeof(hex), fmt[j] == 'x' ? "%x" : "%X", uint32_t(arg(next++)));
      out += hex;
      break;
    }
    case 'c': out += char(arg(next++)); break;
    case 's': out += GuestString(base, uint32_t(arg(next++)), 120); break;
    default: out += fmt.substr(i, j - i + 1); ++next; break;
    }
    i = j;
  }
  return out;
}

} // namespace

// DemonWare's log sink (bdLogMessage's last stop; every level goes through sub_82200198, which
// formats the message): r4 = source file, r5 = function, r6 = line, r7 = "dw/<level>/<channel>",
// r8 = the message. Retail builds print it nowhere; this prints it to our log, e.g.
// "[dw] dw/warn/bdPeer/session: Join refused. (handleJoinReply, bdSession.cpp:1234)".
REX_EXTERN(__imp__sub_821FFF80);
REX_HOOK_RAW(sub_821FFF80) {
  const uint32_t file = ctx.r4.u32, function = ctx.r5.u32, line = ctx.r6.u32;
  if (ShouldLog(file, line)) {
    std::string path = GuestString(base, file);
    size_t slash = path.find_last_of("\\/");
    REXLOG_INFO("[dw] {}: {} ({}, {}:{})", GuestString(base, ctx.r7.u32),
                GuestString(base, ctx.r8.u32, 1000), ShortFunction(GuestString(base, function)),
                slash == std::string::npos ? path : path.substr(slash + 1), line);
  }
  __imp__sub_821FFF80(ctx, base);
}

// Yuke's gss network layer (the match sync on top of DemonWare) reports errors through
// sub_82400AA8(function, line, ?, error code, format, ...), printed only when its debug level
// is 3 or more. Logged here whatever the level, e.g. "[gss] gssXenonRegisterKey:149 error
// -10010: XNetRegisterKey [0x...]".
REX_EXTERN(__imp__sub_82400AA8);
REX_HOOK_RAW(sub_82400AA8) {
  const uint32_t function = ctx.r3.u32, line = ctx.r4.u32;
  if (ShouldLog(function, line)) {
    REXLOG_INFO("[gss] {}:{} error {}: {}", GuestString(base, function, 80), line,
                int32_t(ctx.r6.u32), GuestFormat(ctx, base, ctx.r7.u32, 5));
  }
  __imp__sub_82400AA8(ctx, base);
}

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
