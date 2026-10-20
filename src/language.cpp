// Text language. SvR 2007's text (misc.pac) is in English and Italian only. The game keeps its
// language at 0x82934454 (1 English, 3 Italian; read through sub_821B8EE8 in about 75 places).
// Its setup (sub_821B9650) maps the console language to it (Italian -> 3) but then sets English
// regardless, and a menu flow sets it again from a saved option through sub_821B8ED8. With the
// console language set to Italian (the F1 menu's Language row), both give Italian.

#include <cstdint>
#include <cstdlib>
#include <string>

#include <rex/cvar.h>
#include <rex/hook.h>
#include <rex/logging.h>
#include <rex/types.h>

namespace {

constexpr uint32_t kLanguageVa = 0x82934454;  // 0x82933AA8 + 2476
constexpr uint32_t kGameEnglish = 1, kGameItalian = 3;
constexpr int kXboxItalian = 6;

bool ItalianPicked() {
  return std::atoi(rex::cvar::GetFlagByName("user_language").c_str()) == kXboxItalian;
}

void StoreBe32(uint8_t* p, uint32_t v) {
  p[0] = uint8_t(v >> 24);
  p[1] = uint8_t(v >> 16);
  p[2] = uint8_t(v >> 8);
  p[3] = uint8_t(v);
}

}  // namespace

// Setup: picks the language once at startup.
REX_EXTERN(__imp__sub_821B9650);
REX_HOOK_RAW(sub_821B9650) {
  __imp__sub_821B9650(ctx, base);
  if (ItalianPicked()) {
    StoreBe32(base + kLanguageVa, kGameItalian);
    REXLOG_INFO("Text language: Italian");
  }
}

// SetLanguage(r3).
REX_EXTERN(__imp__sub_821B8ED8);
REX_HOOK_RAW(sub_821B8ED8) {
  if (ctx.r3.u32 == kGameEnglish && ItalianPicked())
    ctx.r3.u64 = kGameItalian;
  __imp__sub_821B8ED8(ctx, base);
}
