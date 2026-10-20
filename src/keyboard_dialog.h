// On-screen keyboard for the game's text entry (Create-a-Superstar name and the like), in place of
// the Xbox guide keyboard (XShowKeyboardUI) that SvR 2007 asks for: a key grid played with the
// controller like the Xbox 360's, in the settings menu's look. A keyboard types into it too.
//
//   D-pad / left stick  move        A  type the key      B  delete      X  space
//   Y  caps             START  done  BACK  cancel         (keyboard: Enter done, Escape cancel)
//
// The game gets no input while it is open (Svr*App routes the input system's active callback
// through BlocksGameInput), and until the buttons that closed it are released.

#pragma once

#include <imgui.h>
#include <rex/ui/imgui_dialog.h>

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <functional>
#include <string>
#include <utility>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <Xinput.h>
#endif

#include "settings_menu.h"  // g_menu_font

struct KeyboardRequest {
  std::string title;
  std::string description;
  std::string text;      // starting text
  size_t max_chars = 0;  // without the terminating null
  // Called on the UI thread when the player finishes: ok false = cancelled.
  std::function<void(bool ok, const std::string& text)> done;
};

class OnScreenKeyboardDialog : public rex::ui::ImGuiDialog {
 public:
  using ReadPadFn = std::function<bool(uint16_t& buttons, int16_t& lx, int16_t& ly)>;

  OnScreenKeyboardDialog(rex::ui::ImGuiDrawer* drawer, ReadPadFn read_pad)
      : ImGuiDialog(drawer), read_pad_(std::move(read_pad)) {
#if defined(_WIN32)
    for (const char* dll : {"xinput1_4.dll", "xinput1_3.dll", "xinput9_1_0.dll"}) {
      if (HMODULE m = LoadLibraryA(dll)) {
        xinput_get_state_ = reinterpret_cast<XInputGetStateFn>(GetProcAddress(m, "XInputGetState"));
        if (xinput_get_state_)
          break;
      }
    }
#endif
  }

  // UI thread.
  void Open(KeyboardRequest request) {
    request_ = std::move(request);
    text_ = request_.text.substr(0, request_.max_chars);
    caps_ = text_.empty();
    caps_lock_ = false;
    row_ = 1;
    col_ = 0;
    open_ = true;
    hold_ = true;  // the button that opened it is still down
    reload_ = true;
  }
  bool IsOpen() const { return open_; }
  bool BlocksGameInput() const { return open_ || hold_; }

 protected:
  void OnDraw(ImGuiIO& io) override {
    const Pad pad = ReadPad();
    if (!open_) {
      if (hold_ && !pad.any)
        hold_ = false;
      last_ = pad;
      return;
    }
    // Ignore what was already held when it opened (the press that started text entry).
    if (hold_) {
      if (!pad.any)
        hold_ = false;
      last_ = pad;
    } else {
      HandleInput(pad);
      last_ = pad;
    }
    if (open_)
      Draw(io);
  }

 private:
  struct Pad {
    bool up = false, down = false, left = false, right = false;
    bool a = false, b = false, x = false, y = false, start = false, back = false, any = false;
  };
  using Clock = std::chrono::steady_clock;

  static constexpr int kCols = 10;
  static constexpr int kRows = 5;
  static constexpr const char* kKeys[4] = {"1234567890", "QWERTYUIOP", "ASDFGHJKL-",
                                           "ZXCVBNM.,'"};
  // Bottom row: caps (2 columns), space (4), delete (2), done (2).
  enum Special { kCaps, kSpace, kDelete, kDone };
  static int SpecialAt(int col) { return col < 2 ? kCaps : col < 6 ? kSpace : col < 8 ? kDelete : kDone; }
  static int SpecialFirstCol(int key) { return key == kCaps ? 0 : key == kSpace ? 2 : key == kDelete ? 6 : 8; }
  static int SpecialCols(int key) { return key == kSpace ? 4 : 2; }

  Pad ReadPad() {
    Pad p;
    constexpr int kDead = 16000;
    auto add = [&p](uint16_t b, int lx, int ly) {
      p.up |= (b & 0x0001) || ly > kDead;
      p.down |= (b & 0x0002) || ly < -kDead;
      p.left |= (b & 0x0004) || lx < -kDead;
      p.right |= (b & 0x0008) || lx > kDead;
      p.start |= (b & 0x0010) != 0;
      p.back |= (b & 0x0020) != 0;
      p.a |= (b & 0x1000) != 0;
      p.b |= (b & 0x2000) != 0;
      p.x |= (b & 0x4000) != 0;
      p.y |= (b & 0x8000) != 0;
      p.any |= b != 0 || ly > kDead || ly < -kDead || lx > kDead || lx < -kDead;
    };
    if (read_pad_) {
      uint16_t b = 0;
      int16_t lx = 0, ly = 0;
      if (read_pad_(b, lx, ly))
        add(b, lx, ly);
    }
#if defined(_WIN32)
    if (xinput_get_state_) {
      for (DWORD i = 0; i < 4; ++i) {
        XINPUT_STATE st{};
        if (xinput_get_state_(i, &st) == ERROR_SUCCESS)
          add(st.Gamepad.wButtons, st.Gamepad.sThumbLX, st.Gamepad.sThumbLY);
      }
    }
#endif
    return p;
  }

  bool Fire(bool now, bool before, Clock::time_point& next) {
    const auto t = Clock::now();
    if (now && !before) {
      next = t + std::chrono::milliseconds(380);
      return true;
    }
    if (now && t >= next) {
      next = t + std::chrono::milliseconds(110);
      return true;
    }
    return false;
  }

  void Insert(char c) {
    if (text_.size() >= request_.max_chars)
      return;
    if (c >= 'A' && c <= 'Z' && !caps_)
      c = char(c - 'A' + 'a');
    text_.push_back(c);
    if (std::isalpha(static_cast<unsigned char>(c)) && !caps_lock_)
      caps_ = false;
    reload_ = true;
  }
  void Delete() {
    if (!text_.empty()) {
      text_.pop_back();
      reload_ = true;
    }
    if (text_.empty() && !caps_lock_)
      caps_ = true;
  }
  void ToggleCaps() {
    caps_lock_ = !caps_lock_;
    caps_ = caps_lock_;
  }
  void Press() {
    if (row_ < 4) {
      Insert(kKeys[row_][col_]);
      return;
    }
    switch (SpecialAt(col_)) {
      case kCaps: ToggleCaps(); break;
      case kSpace: Insert(' '); break;
      case kDelete: Delete(); break;
      case kDone: Finish(true); break;
    }
  }
  void Finish(bool ok) {
    open_ = false;
    hold_ = true;
    auto done = std::move(request_.done);
    request_.done = nullptr;
    // Trailing spaces would be part of the name.
    std::string text = text_;
    while (!text.empty() && text.back() == ' ')
      text.pop_back();
    if (done)
      done(ok, ok ? text : request_.text);
  }

  void Move(int drow, int dcol) {
    if (drow) {
      row_ = (row_ + drow + kRows) % kRows;
      return;
    }
    if (row_ < 4) {
      col_ = (col_ + dcol + kCols) % kCols;
      return;
    }
    // Bottom row: whole keys.
    const int key = (SpecialAt(col_) + dcol + 4) % 4;
    col_ = SpecialFirstCol(key);
  }

  void HandleInput(const Pad& pad) {
    if (Fire(pad.up, last_.up, rep_[0])) Move(-1, 0);
    if (Fire(pad.down, last_.down, rep_[1])) Move(1, 0);
    if (Fire(pad.left, last_.left, rep_[2])) Move(0, -1);
    if (Fire(pad.right, last_.right, rep_[3])) Move(0, 1);
    if (pad.a && !last_.a) Press();
    if (Fire(pad.b, last_.b, rep_[4])) Delete();
    if (pad.x && !last_.x) Insert(' ');
    if (pad.y && !last_.y) ToggleCaps();
    if (open_ && pad.start && !last_.start) Finish(true);
    if (open_ && pad.back && !last_.back) Finish(false);
  }

  void Draw(ImGuiIO& io) {
    const float s = std::max(0.75f, io.DisplaySize.y / 720.0f);
    const float width = 640.0f * s;
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always,
                            ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(width, 0));
    ImGui::SetNextWindowFocus();
    constexpr ImGuiWindowFlags kFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoSavedSettings |
                                        ImGuiWindowFlags_AlwaysAutoResize;
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(18, 21, 30, 245));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(18 * s, 16 * s));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 6 * s);
    ImGui::PushFont(g_menu_font, 14 * s);
    if (ImGui::Begin("##svr_keyboard", nullptr, kFlags)) {
      ImGui::PushFont(g_menu_font, 20 * s);
      ImGui::TextUnformatted(request_.title.empty() ? "ENTER TEXT" : request_.title.c_str());
      ImGui::PopFont();
      if (!request_.description.empty()) {
        ImGui::PushFont(g_menu_font, 11 * s);
        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(178, 184, 198, 255));
        ImGui::PushTextWrapPos(width - 18 * s);
        ImGui::TextUnformatted(request_.description.c_str());
        ImGui::PopTextWrapPos();
        ImGui::PopStyleColor();
        ImGui::PopFont();
      }
      ImGui::Dummy(ImVec2(0, 6 * s));
      DrawTextField(s);
      ImGui::Dummy(ImVec2(0, 8 * s));
      DrawKeys(s);
      ImGui::Dummy(ImVec2(0, 6 * s));
      ImGui::PushFont(g_menu_font, 10 * s);
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(178, 184, 198, 255));
      ImGui::TextUnformatted("A type  B delete  X space  Y caps  START done  BACK cancel");
      ImGui::PopStyleColor();
      ImGui::PopFont();
    }
    ImGui::End();
    ImGui::PopFont();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor();
  }

  // The text, editable with a keyboard (Enter done, Escape cancel).
  void DrawTextField(float s) {
    if (reload_) {
      std::snprintf(buffer_, sizeof(buffer_), "%s", text_.c_str());
      ++generation_;
      reload_ = false;
    }
    char id[32];
    std::snprintf(id, sizeof(id), "##kb%d", generation_);
    ImGui::PushFont(g_menu_font, 18 * s);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(32, 38, 54, 255));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10 * s, 10 * s));
    ImGui::SetNextItemWidth(-1);
    if (!ImGui::IsAnyItemActive())
      ImGui::SetKeyboardFocusHere();
    const size_t cap = std::min(sizeof(buffer_), request_.max_chars + 1);
    const ImGuiInputTextFlags flags =
        ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCharFilter;
    const bool enter = ImGui::InputText(id, buffer_, cap, flags, [](ImGuiInputTextCallbackData* d) {
      // The game's names are plain ASCII.
      return d->EventChar >= 0x20 && d->EventChar < 0x7F ? 0 : 1;
    });
    if (std::strcmp(buffer_, text_.c_str()) != 0)
      text_ = buffer_;
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopFont();
    if (enter && open_)
      Finish(true);
    else if (open_ && ImGui::IsKeyPressed(ImGuiKey_Escape, false))
      Finish(false);
  }

  void DrawKeys(float s) {
    const float gap = 6 * s;
    const float avail = ImGui::GetContentRegionAvail().x;
    const float key_w = (avail - gap * (kCols - 1)) / kCols;
    const float key_h = 38 * s;
    const ImU32 normal = IM_COL32(40, 46, 64, 255);
    const ImU32 selected = IM_COL32(230, 230, 236, 255);
    ImDrawList* dl = ImGui::GetWindowDrawList();
    ImGui::PushFont(g_menu_font, 15 * s);
    const ImVec2 origin = ImGui::GetCursorScreenPos();
    auto draw_key = [&](int row, int col, int cols, const char* label, bool sel, bool lit) {
      const ImVec2 a(origin.x + col * (key_w + gap), origin.y + row * (key_h + gap));
      const ImVec2 b(a.x + key_w * cols + gap * (cols - 1), a.y + key_h);
      dl->AddRectFilled(a, b, sel ? selected : normal, 4 * s);
      if (lit && !sel)
        dl->AddRect(a, b, IM_COL32(120, 170, 255, 255), 4 * s, 0, 2 * s);
      const ImVec2 ts = ImGui::CalcTextSize(label);
      dl->AddText(ImVec2((a.x + b.x - ts.x) * 0.5f, (a.y + b.y - ts.y) * 0.5f),
                  sel ? IM_COL32(18, 21, 30, 255) : IM_COL32(255, 255, 255, 255), label);
      // Mouse: click a key.
      ImGui::SetCursorScreenPos(a);
      char id[16];
      std::snprintf(id, sizeof(id), "##k%d_%d", row, col);
      if (ImGui::InvisibleButton(id, ImVec2(b.x - a.x, b.y - a.y))) {
        row_ = row;
        col_ = col;
        Press();
      }
    };
    for (int row = 0; row < 4; ++row) {
      for (int col = 0; col < kCols; ++col) {
        char label[2] = {kKeys[row][col], 0};
        if (label[0] >= 'A' && label[0] <= 'Z' && !caps_)
          label[0] = char(label[0] - 'A' + 'a');
        draw_key(row, col, 1, label, row_ == row && col_ == col, false);
      }
    }
    static constexpr const char* kSpecialLabels[4] = {"CAPS", "SPACE", "DEL", "DONE"};
    for (int key = 0; key < 4; ++key)
      draw_key(4, SpecialFirstCol(key), SpecialCols(key), kSpecialLabels[key],
               row_ == 4 && SpecialAt(col_) == key, key == kCaps && caps_lock_);
    ImGui::SetCursorScreenPos(ImVec2(origin.x, origin.y + kRows * (key_h + gap)));
    ImGui::Dummy(ImVec2(avail, 0));
    ImGui::PopFont();
  }

  ReadPadFn read_pad_;
#if defined(_WIN32)
  using XInputGetStateFn = DWORD(WINAPI*)(DWORD, XINPUT_STATE*);
  XInputGetStateFn xinput_get_state_ = nullptr;
#endif
  KeyboardRequest request_;
  std::string text_;
  char buffer_[64] = {};
  int generation_ = 0;
  bool reload_ = false;
  bool open_ = false;
  bool hold_ = false;
  bool caps_ = true;
  bool caps_lock_ = false;
  int row_ = 1;
  int col_ = 0;
  Pad last_;
  Clock::time_point rep_[5];
};
