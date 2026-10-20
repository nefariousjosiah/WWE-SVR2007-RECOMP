// Text entry with a controller (Create-a-Superstar name and the like).
//
// SvR 2007 opens text entry in sub_8249FA10, which calls XShowKeyboardUI (sub_821CC890, a jump to
// the XAM import) for the Xbox guide keyboard; its own text screen (sub_8249E108 says yes) is only
// for a USB keyboard and only where the entry allows it (+292; the superstar name doesn't). The SDK
// shows XShowKeyboardUI as a plain ImGui text box the controller can't use, so it is answered here
// with OnScreenKeyboardDialog (keyboard_dialog.h), completed like the guide's: the text written to
// the result buffer, the overlapped completed (cancel: extended error ERROR_CANCELLED), and
// XN_SYS_UI raised while it is up.

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>

#include <rex/hook.h>
#include <rex/logging.h>
#include <rex/system/kernel_state.h>
#include <rex/types.h>

#include "keyboard_dialog.h"

// Set by the app: shows the keyboard on the UI thread. Empty: the SDK's own text box is used.
std::function<void(KeyboardRequest)> g_svr_open_keyboard;

namespace {

constexpr uint32_t kXnSysUi = 0x9;
constexpr uint32_t kErrorCancelled = 1223;

uint32_t LoadBe32(const uint8_t* p) {
  return uint32_t(p[0]) << 24 | uint32_t(p[1]) << 16 | uint32_t(p[2]) << 8 | p[3];
}
void StoreBe32(uint8_t* p, uint32_t v) {
  p[0] = uint8_t(v >> 24);
  p[1] = uint8_t(v >> 16);
  p[2] = uint8_t(v >> 8);
  p[3] = uint8_t(v);
}

// Guest UTF-16 (big-endian), as plain ASCII (anything else becomes '?').
std::string ReadWide(const uint8_t* base, uint32_t va, size_t limit = 256) {
  std::string out;
  if (!va)
    return out;
  for (size_t i = 0; i < limit; ++i) {
    const uint16_t c = uint16_t(base[va + 2 * i] << 8 | base[va + 2 * i + 1]);
    if (!c)
      break;
    out.push_back(c < 0x80 ? char(c) : '?');
  }
  return out;
}

void WriteWide(uint8_t* base, uint32_t va, const std::string& text, size_t chars) {
  if (!va || !chars)
    return;
  const size_t n = std::min(text.size(), chars - 1);
  for (size_t i = 0; i < n; ++i) {
    base[va + 2 * i] = 0;
    base[va + 2 * i + 1] = uint8_t(text[i]);
  }
  base[va + 2 * n] = 0;
  base[va + 2 * n + 1] = 0;
}

}  // namespace

// XShowKeyboardUI(user, flags, default text, title, description, result, result chars, overlapped).
REX_EXTERN(__imp__sub_821CC890);
REX_HOOK_RAW(sub_821CC890) {
  const uint32_t overlapped = ctx.r10.u32;
  const uint32_t result_va = ctx.r8.u32;
  const uint32_t result_chars = ctx.r9.u32;
  if (!g_svr_open_keyboard || !overlapped || !result_va || result_chars < 2) {
    __imp__sub_821CC890(ctx, base);
    return;
  }
  KeyboardRequest request;
  request.text = ReadWide(base, ctx.r5.u32);
  request.title = ReadWide(base, ctx.r6.u32);
  request.description = ReadWide(base, ctx.r7.u32);
  request.max_chars = result_chars - 1;
  REXLOG_INFO("Text entry: '{}' ({} characters), on-screen keyboard", request.title,
              request.max_chars);

  // Pending until the player finishes (XOVERLAPPED: InternalLow = result, InternalHigh = length).
  StoreBe32(base + overlapped, 997);  // ERROR_IO_PENDING
  StoreBe32(base + overlapped + 4, 0);
  REX_KERNEL_STATE()->BroadcastNotification(kXnSysUi, true);

  request.done = [base, overlapped, result_va, result_chars](bool ok, const std::string& text) {
    WriteWide(base, result_va, text, result_chars);
    REXLOG_INFO("Text entry: {} '{}'", ok ? "done" : "cancelled", text);
    REX_KERNEL_STATE()->CompleteOverlappedEx(overlapped, 0, ok ? 0 : kErrorCancelled, 0);
    // The guide raises XN_SYS_UI false a moment after it closes.
    std::thread([] {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      REX_KERNEL_STATE()->BroadcastNotification(kXnSysUi, false);
    }).detach();
  };
  g_svr_open_keyboard(std::move(request));
  ctx.r3.u64 = 997;  // ERROR_IO_PENDING
}
