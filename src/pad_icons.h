// PlayStation button pictures.
//
// The game shows its controller's buttons as pictures: characters of its fonts (U+E000 on), drawn
// from icon pages the fonts share, and pictures of their own (the in-match prompts such as the
// finisher and reversal ones, the controller on the controls screen). When PlayStation buttons are
// wanted, the native renderer shows those textures with this port's own drawings in the places of
// the Xbox buttons, in the style of the PlayStation 3 versions' prompts (res/pad_icons, drawn by
// tools/pad_icons): Cross, Circle, Square, Triangle, L1/R1, L2/R2, the sticks, START and SELECT.
// The game's text and everything else on those textures stay as they are. Which textures, and
// where their buttons are: pad_icons_tables.h, this game's own.
//
// Which buttons: the svr_button_prompts setting (Button prompts in the settings menu): Auto follows
// the controller used last (a DualShock or DualSense: PlayStation), or always Xbox / PlayStation.

#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace svr {

// A DXT5 texture (or DXT3: `dxt3`) the renderer is about to build (its base level, `stride` bytes
// a row of 4x4 blocks). When it is one of the textures with Xbox buttons and PlayStation buttons
// are wanted: fills `rgba` with it as it should look (RGBA8, width * height * 4) and returns true.
bool PadIconsPage(uint32_t guest_va, uint32_t width, uint32_t height, const uint8_t* blocks,
                  size_t stride, std::vector<uint8_t>& rgba, bool dxt3 = false);

// The same for a 16- or 32-bit colour texture (`texels`: its base level as stored, `stride` bytes a
// row of `bytes_per_texel` bytes a texel). `decode` fills its argument with the texture as the
// game samples it (RGBA8, width * height * 4); `rgba` is filled the same way.
bool PadIconsColour(uint32_t guest_va, uint32_t width, uint32_t height, uint32_t bytes_per_texel,
                    const uint8_t* texels, size_t stride,
                    const std::function<void(std::vector<uint8_t>&)>& decode,
                    std::vector<uint8_t>& rgba);

// Called once a frame: the textures to build again because the wanted buttons changed (the player
// picked up another controller, or changed the setting).
std::vector<uint32_t> PadIconsStalePages();

}  // namespace svr
