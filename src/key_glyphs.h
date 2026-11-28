// Keyboard and mouse pictures for the game's button prompts, for a player on the keyboard (its
// controller emulation, mnk_mode): each Xbox button is shown as the key it's bound to (the
// keybind_* settings, F1 > Controls), drawn here at the size it's needed in: a key cap with the
// key's name or arrow, W A S D style keys for a stick, a mouse with the button lit for a mouse
// button, and a mouse for the right stick when the mouse moves it (mnk_mouse). The same file in
// every game of the series; pad_icons.cpp decides where they go.

#pragma once

#include <cstdint>

namespace svr::keys {

// The controller's buttons, in the order of Btn in pad_icons.cpp.
enum class Pad : uint8_t {
  A, B, X, Y, LB, RB, LT, RT, LSPress, RSPress, LStick, RStick, Start, Back, Guide, DPad
};

// Changes whenever the bindings (or the mouse setting) do: the pictures are drawn again then.
uint64_t BindingsSignature();

// Whether the keyboard has something for the button (an unbound one keeps the game's picture).
bool Bound(Pad pad);

// Draws the button's key (or keys, or mouse) as large as it fits in the place x, y, w, h of
// `rgba` (RGBA8, `width` pixels a row), in the middle, over what's there; `fill`: stretched to the
// whole place (a picture the game draws stretched).
void Draw(Pad pad, uint32_t x, uint32_t y, uint32_t w, uint32_t h, bool fill, uint8_t* rgba,
          uint32_t width, uint32_t height);

}  // namespace svr::keys
