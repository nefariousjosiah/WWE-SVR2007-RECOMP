// The settings menu's Controls page (F1 > Controls): button prompts, keyboard controls,
// controllers, and from there the Keyboard & mouse setup page: the keys (and mouse buttons) of
// every controller input for the keyboard's controller emulation (mnk_mode; rexglue-sdk
// src/input/mnk/mnk_input_driver.cpp), drawn as key caps like the in-game button prompts
// (key_glyphs.cpp), with the mouse settings above them. Changes apply at once and are saved with
// the menu's other settings; the prompts redraw by themselves (pad_icons.cpp watches
// keys::BindingsSignature). The same file in every game of the series: settings_menu.h opens the
// Controls page from a row and hands it its input and drawing while it's open.

#pragma once

#include <imgui.h>
#include <rex/cvar.h>
#include <rex/ui/keybinds.h>
#include <rex/ui/ui_event.h>
#include <rex/ui/virtual_key.h>
#include <rex/ui/window.h>
#include <rex/ui/window_listener.h>

#include <algorithm>
#include <cctype>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <mutex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

// Roboto Medium (the in-game key pictures' font) for the names on the key caps, set by the app's
// OnConfigureFonts; nullptr: the menu's own font.
inline ImFont* g_key_font = nullptr;

// From rexglue-sdk rex/input/mnk/mnk_input_driver.h, declared here rather than included: that
// header brings in rex/input/input.h, whose names clash with <Xinput.h> where a file includes it
// first.
namespace rex::input::mnk {
void SetMouseLookActive(bool active);
}

namespace svr {

class KeyboardSetupPage final : public rex::ui::WindowInputListener,
                                public rex::ui::WindowListener {
 public:
  // The controller as the menu reads it this frame (buttons held, not presses).
  struct Pad {
    bool up = false, down = false, left = false, right = false, a = false, b = false, x = false;
  };
  // The menu's own save: one "key = value" line of the settings file.
  using SaveFn = std::function<void(const char* key, const std::string& value)>;

  // `window`: the game's window, where the keys for a binding are read from.
  KeyboardSetupPage(rex::ui::Window* window, SaveFn save)
      : window_(window), save_(std::move(save)) {}
  ~KeyboardSetupPage() override { Close(); }
  KeyboardSetupPage(const KeyboardSetupPage&) = delete;
  KeyboardSetupPage& operator=(const KeyboardSetupPage&) = delete;

  bool IsOpen() const { return open_; }

  void Open() {
    open_ = true;
    first_input_ = true;
    selected_ = 0;
    slot_ = 0;
    follow_ = 3;
    confirm_reset_ = dragging_ = false;
    message_.clear();
    Reload();
    if (window_ && !listening_) {
      // Above the ImGui overlays (64): while a key is being picked, it reaches nothing else, so
      // F1 and the other hotkeys don't fire and the menu doesn't move.
      window_->AddInputListener(this, 200);
      window_->AddListener(this);
      listening_ = true;
    }
    if (dev_item_ >= 0) {
      selected_ = std::clamp(dev_item_, 0, kItems - 1);
      dev_item_ = -1;
      if (std::exchange(dev_capture_, false) && IsBind(selected_))
        StartCapture(selected_, 0);
    }
  }

  void Close() {
    if (!open_)
      return;
    open_ = false;
    if (capturing_)
      EndCapture();
    if (listening_ && window_) {
      window_->RemoveInputListener(this);
      window_->RemoveListener(this);
    }
    listening_ = false;
    std::lock_guard lock(mutex_);
    std::fill(std::begin(held_), std::end(held_), false);
    queue_.clear();
  }

  // The settings menu was opened or closed. Mouse look is off while it's open: it would hide
  // and lock the pointer, and the menu is used with the mouse.
  static void MenuShown(bool shown) { rex::input::mnk::SetMouseLookActive(!shown); }

  // Developer aid: SVR_OPEN_KEYS_PAGE_AT_MS=<ms>[,<item>[,1]] opens the page once that long
  // after start, on that item, picking a key for it with ",1" (screenshots without input).
  // True once, when the page should open (ControlsPage::DevAutoOpen).
  bool DevAutoOpen() {
    static const char* const spec = std::getenv("SVR_OPEN_KEYS_PAGE_AT_MS");
    static const auto start = Clock::now();
    static bool done = false;
    if (!spec || done)
      return false;
    char* end = nullptr;
    const long long at = std::strtoll(spec, &end, 10);
    if (end == spec || at < 0 || Clock::now() - start < std::chrono::milliseconds(at))
      return false;
    done = true;
    dev_item_ = *end == ',' ? int(std::strtol(end + 1, &end, 10)) : -1;
    dev_capture_ = *end == ',' && std::atoi(end + 1) != 0;
    return true;
  }

  // The page's input for this frame (before Draw).
  void Input(const Pad& pad) {
    std::vector<Event> events;
    bool held = false, lost = false;
    {
      std::lock_guard lock(mutex_);
      events.swap(queue_);
      held = std::find(std::begin(held_), std::end(held_), true) != std::end(held_);
      lost = std::exchange(lost_focus_, false);
      if (mute_ && !held)
        mute_ = false;
    }
    if (first_input_) {  // what's held already was pressed to get here
      first_input_ = false;
      last_ = pad;
      return;
    }
    if (capturing_) {
      Capture(events, pad, held, lost);
      last_ = pad;
      return;
    }
    // With a key or mouse button held, the controller may be the keyboard's own (its emulated
    // pad): only the page's keys count then, or testing a key would move around the page.
    const bool pad_ok = !held;
    const auto press = [&](bool now, bool before) { return pad_ok && now && !before; };
    const bool up = (pad_ok && Fire(pad.up, last_.up, rep_[0])) ||
                    ImGui::IsKeyPressed(ImGuiKey_UpArrow);
    const bool down = (pad_ok && Fire(pad.down, last_.down, rep_[1])) ||
                      ImGui::IsKeyPressed(ImGuiKey_DownArrow);
    const bool left = (pad_ok && Fire(pad.left, last_.left, rep_[2])) ||
                      ImGui::IsKeyPressed(ImGuiKey_LeftArrow);
    const bool right = (pad_ok && Fire(pad.right, last_.right, rep_[3])) ||
                       ImGui::IsKeyPressed(ImGuiKey_RightArrow);
    const bool accept = press(pad.a, last_.a) || ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
                        ImGui::IsKeyPressed(ImGuiKey_Space, false);
    const bool clear = press(pad.x, last_.x) || ImGui::IsKeyPressed(ImGuiKey_Delete, false);
    const bool back = press(pad.b, last_.b) || ImGui::IsKeyPressed(ImGuiKey_Escape, false);
    last_ = pad;
    if (up || down) {
      selected_ = (selected_ + (up ? kItems - 1 : 1)) % kItems;
      follow_ = 2;
      confirm_reset_ = false;
    }
    if (left || right) {
      if (IsBind(selected_))
        slot_ = right ? 1 : 0;
      else if (selected_ <= kSensitivityRow)
        Change(selected_, right ? +1 : -1);
    }
    if (accept)
      Activate(selected_);
    if (clear && IsBind(selected_))
      Clear(selected_ - kFirstBind, slot_);
    if (back)
      Close();
  }

  void Draw(ImGuiIO& io, ImFont* font) {
    if (!font)
      font = ImGui::GetFont();
    const float s = std::max(0.6f, io.DisplaySize.y / 1080.0f);
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), io.DisplaySize,
                                                  IM_COL32(0, 0, 0, 150));
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(900 * s, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14 * s);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30 * s, 26 * s));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(18, 21, 30, 245));
    ImGui::PushFont(font, 16 * s);
    constexpr ImGuiWindowFlags kFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav |
                                        ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin("##svr_keys_page", nullptr, kFlags)) {
      const float inner = ImGui::GetContentRegionAvail().x;
      ImGui::PushFont(font, 30 * s);
      ImGui::TextColored(ImColor(kWhite), "KEYBOARD & MOUSE");
      ImGui::PopFont();
      ImGui::PushFont(font, 13 * s);
      ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + inner);
      if (On("mnk_mode")) {
        ImGui::PushStyleColor(ImGuiCol_Text, kDim);
        ImGui::TextWrapped("Up to two keys for each controller button. The game's button "
                           "prompts show your keys.");
      } else {
        ImGui::PushStyleColor(ImGuiCol_Text, kAccent);
        ImGui::TextWrapped("Keyboard controls are off: switch them on in the first row to play "
                           "with these keys.");
      }
      ImGui::PopStyleColor();
      ImGui::PopTextWrapPos();
      ImGui::PopFont();
      ImGui::Dummy(ImVec2(0, 12 * s));

      // The rows scroll in a list as tall as the screen allows.
      const float row_h = 46 * s, header_h = 34 * s;
      const float list_h = std::clamp(io.DisplaySize.y * 0.94f - 250 * s, 6 * row_h,
                                      kItems * row_h + header_h);
      ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 8 * s);
      ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 4 * s);
      ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, IM_COL32(255, 255, 255, 8));
      ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, IM_COL32(255, 255, 255, 46));
      ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, IM_COL32(255, 255, 255, 80));
      ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, kAccent);
      if (ImGui::BeginChild("##keys_list", ImVec2(inner, list_h), ImGuiChildFlags_None,
                            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysVerticalScrollbar))
        DrawList(io, font, s, row_h, header_h);
      ImGui::EndChild();
      ImGui::PopStyleColor(4);
      ImGui::PopStyleVar(2);

      ImGui::Dummy(ImVec2(0, 10 * s));
      ImGui::PushFont(font, 13 * s);
      const bool message = !message_.empty() && Clock::now() < message_until_;
      ImGui::PushStyleColor(ImGuiCol_Text, message ? kWhite : IM_COL32(178, 184, 198, 255));
      ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + inner);
      const float hint_y = ImGui::GetCursorPosY();
      ImGui::TextWrapped("%s", message ? message_.c_str() : Hint().c_str());
      ImGui::SetCursorPosY(
          std::max(ImGui::GetCursorPosY(), hint_y + 3 * ImGui::GetTextLineHeightWithSpacing()));
      ImGui::PopTextWrapPos();
      ImGui::PopStyleColor();
      ImGui::Dummy(ImVec2(0, 6 * s));
      const char* legend =
          capturing_          ? "Esc cancel   Delete/Backspace clear"
          : IsBind(selected_) ? "Up/Down choose  Left/Right key 1/2  A change  X clear  B back"
          : selected_ <= kSensitivityRow ? "Up/Down choose   Left/Right change   B/Esc back"
                                         : "Up/Down choose   A/Enter select   B/Esc back";
      ImGui::TextColored(ImColor(kDim), "%s", legend);
      ImGui::PopFont();
    }
    ImGui::End();
    ImGui::PopFont();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
  }

  // The window's keys and mouse buttons (UI thread): what's held, and while a key is being
  // picked, every press, kept from the game and the rest of the menu.
  void OnKeyDown(rex::ui::KeyEvent& e) override { Key(e, true); }
  void OnKeyUp(rex::ui::KeyEvent& e) override { Key(e, false); }
  void OnMouseDown(rex::ui::MouseEvent& e) override { Mouse(e, true); }
  void OnMouseUp(rex::ui::MouseEvent& e) override { Mouse(e, false); }
  void OnLostFocus(rex::ui::UISetupEvent&) override {
    std::lock_guard lock(mutex_);
    std::fill(std::begin(held_), std::end(held_), false);
    lost_focus_ = true;
  }

 private:
  using Clock = std::chrono::steady_clock;
  using VK = rex::ui::VirtualKey;

  // A key or mouse button going down or up, from the window.
  struct Event {
    uint16_t vk;
    bool down;
    uint8_t mods;  // modifiers held at the time
  };

  // The page's items: the mouse settings, a row for each controller input, then Reset and Back.
  static constexpr int kKeyboardRow = 0, kMouseRow = 1, kSensitivityRow = 2, kFirstBind = 3;
  static constexpr int kBinds = 25;
  static constexpr int kResetRow = kFirstBind + kBinds, kBackRow = kResetRow + 1;
  static constexpr int kItems = kBackRow + 1;
  static bool IsBind(int item) { return item >= kFirstBind && item < kFirstBind + kBinds; }

  // Each controller input: its binding (rexglue-sdk mnk_input_driver.cpp) and its names on a
  // PlayStation and an Xbox controller.
  struct Binding {
    const char* cvar;
    const char* label;
  };
  static constexpr Binding kInputs[kBinds] = {
      {"keybind_a", "Cross / A"},
      {"keybind_b", "Circle / B"},
      {"keybind_x", "Square / X"},
      {"keybind_y", "Triangle / Y"},
      {"keybind_left_shoulder", "L1 / LB"},
      {"keybind_right_shoulder", "R1 / RB"},
      {"keybind_left_trigger", "L2 / LT"},
      {"keybind_right_trigger", "R2 / RT"},
      {"keybind_lstick_press", "L3 / LS press"},
      {"keybind_rstick_press", "R3 / RS press"},
      {"keybind_back", "Select / Back"},
      {"keybind_start", "Start"},
      {"keybind_guide", "PS / Guide"},
      {"keybind_dpad_up", "D-pad up"},
      {"keybind_dpad_down", "D-pad down"},
      {"keybind_dpad_left", "D-pad left"},
      {"keybind_dpad_right", "D-pad right"},
      {"keybind_lstick_up", "Left stick up"},
      {"keybind_lstick_down", "Left stick down"},
      {"keybind_lstick_left", "Left stick left"},
      {"keybind_lstick_right", "Left stick right"},
      {"keybind_rstick_up", "Right stick up"},
      {"keybind_rstick_down", "Right stick down"},
      {"keybind_rstick_left", "Right stick left"},
      {"keybind_rstick_right", "Right stick right"},
  };

  // --- Key names ---------------------------------------------------------------------------

  // The bindings' key names (rexglue-sdk src/ui/keybinds.cpp), what a key cap shows for each
  // (as the in-game pictures do, key_glyphs.cpp) and the key. The first name of a key is the one
  // written; "NumpadEnter" and "Ctrl" are only read.
  struct KeyName {
    std::string name, shown;
    uint16_t vk;
  };
  static const std::vector<KeyName>& KeyNames() {
    static const std::vector<KeyName> names = [] {
      std::vector<KeyName> n;
      const auto add = [&n](std::string name, std::string shown, int vk) {
        n.push_back({std::move(name), std::move(shown), uint16_t(vk)});
      };
      for (int i = 0; i < 26; ++i)
        add(std::string(1, char('A' + i)), std::string(1, char('A' + i)), int(VK::kA) + i);
      for (int i = 0; i < 10; ++i)
        add(std::string(1, char('0' + i)), std::string(1, char('0' + i)), int(VK::k0) + i);
      for (int i = 0; i < 24; ++i)
        add("F" + std::to_string(i + 1), "F" + std::to_string(i + 1), int(VK::kF1) + i);
      for (int i = 0; i < 10; ++i)
        add("Numpad" + std::to_string(i), "NUM " + std::to_string(i), int(VK::kNumpad0) + i);
      static constexpr struct {
        const char* name;
        const char* shown;
        VK vk;
      } kOthers[] = {
          {"Semicolon", ";", VK::kOem1},       {"Quote", "'", VK::kOem7},
          {"Comma", ",", VK::kOemComma},       {"Period", ".", VK::kOemPeriod},
          {"Slash", "/", VK::kOem2},           {"Backslash", "\\", VK::kOem5},
          {"Minus", "-", VK::kOemMinus},       {"Plus", "=", VK::kOemPlus},
          {"Backtick", "`", VK::kOem3},        {"LBracket", "[", VK::kOem4},
          {"RBracket", "]", VK::kOem6},        {"Space", "SPACE", VK::kSpace},
          {"Return", "ENTER", VK::kReturn},    {"Escape", "ESC", VK::kEscape},
          {"Tab", "TAB", VK::kTab},            {"Backspace", "BKSP", VK::kBack},
          {"Delete", "DEL", VK::kDelete},      {"Insert", "INS", VK::kInsert},
          {"Home", "HOME", VK::kHome},         {"End", "END", VK::kEnd},
          {"PageUp", "PG UP", VK::kPrior},     {"PageDown", "PG DN", VK::kNext},
          {"Up", "UP", VK::kUp},               {"Down", "DOWN", VK::kDown},
          {"Left", "LEFT", VK::kLeft},         {"Right", "RIGHT", VK::kRight},
          {"Shift", "SHIFT", VK::kShift},      {"Control", "CTRL", VK::kControl},
          {"Alt", "ALT", VK::kMenu},           {"CapsLock", "CAPS", VK::kCapital},
          {"NumLock", "NUM", VK::kNumLock},    {"ScrollLock", "SCRL", VK::kScroll},
          {"PrintScreen", "PRT SC", VK::kSnapshot}, {"Pause", "PAUSE", VK::kPause},
          {"NumpadPlus", "NUM +", VK::kAdd},   {"NumpadMinus", "NUM -", VK::kSubtract},
          {"NumpadStar", "NUM *", VK::kMultiply}, {"NumpadSlash", "NUM /", VK::kDivide},
          {"LMB", "LEFT MOUSE", VK::kLButton}, {"RMB", "RIGHT MOUSE", VK::kRButton},
          {"MMB", "MIDDLE MOUSE", VK::kMButton},
          {"NumpadEnter", "ENTER", VK::kReturn}, {"Ctrl", "CTRL", VK::kControl},
      };
      for (const auto& o : kOthers)
        add(o.name, o.shown, int(o.vk));
      return n;
    }();
    return names;
  }
  static uint16_t VkOf(std::string_view name) {
    for (const KeyName& k : KeyNames())
      if (k.name == name)
        return k.vk;
    return 0;
  }
  static std::string NameOf(uint16_t vk) {
    for (const KeyName& k : KeyNames())
      if (k.vk == vk)
        return k.name;
    return {};
  }
  static std::string Shown(std::string_view name) {
    for (const KeyName& k : KeyNames())
      if (k.name == name)
        return k.shown;
    std::string upper;
    for (char c : name)
      upper += char(std::toupper(static_cast<unsigned char>(c)));
    return upper;
  }

  // Modifiers as the driver reads them: a combination fires only with exactly its modifiers.
  static constexpr uint8_t kShift = 1, kCtrl = 2, kAlt = 4;
  static uint8_t ModBit(uint16_t vk) {
    return vk == uint16_t(VK::kShift)     ? kShift
           : vk == uint16_t(VK::kControl) ? kCtrl
           : vk == uint16_t(VK::kMenu)    ? kAlt
                                          : 0;
  }
  static std::string ModPrefix(uint8_t mods) {
    return std::string(mods & kShift ? "Shift+" : "") + (mods & kCtrl ? "Ctrl+" : "") +
           (mods & kAlt ? "Alt+" : "");
  }
  static std::string ShownMods(uint8_t mods) {
    return std::string(mods & kShift ? "SHIFT+" : "") + (mods & kCtrl ? "CTRL+" : "") +
           (mods & kAlt ? "ALT+" : "");
  }

  // --- Bindings ----------------------------------------------------------------------------

  // One alternative of a binding ("Shift+Up").
  struct Token {
    uint8_t mods = 0;
    uint16_t vk = 0;  // 0: a name the game doesn't know (kept as written)
    std::string text, key;  // as written, and its key without the modifiers
    bool Valid() const { return !text.empty(); }
    bool Same(const Token& o) const { return vk && vk == o.vk && mods == o.mods; }
  };
  static std::string_view Trim(std::string_view s) {
    while (!s.empty() && s.front() == ' ')
      s.remove_prefix(1);
    while (!s.empty() && s.back() == ' ')
      s.remove_suffix(1);
    return s;
  }
  // A binding's alternatives ("Semicolon,Space"), read as the driver does.
  static std::vector<Token> Parse(std::string_view value) {
    std::vector<Token> out;
    while (!value.empty()) {
      const size_t comma = value.find(',');
      std::string_view part = Trim(value.substr(0, comma));
      value = comma == std::string_view::npos ? std::string_view() : value.substr(comma + 1);
      if (part.empty())
        continue;
      Token t;
      t.text = std::string(part);
      for (size_t plus; (plus = part.find('+')) != std::string_view::npos && plus > 0;) {
        const std::string_view head = part.substr(0, plus);
        const uint8_t bit = head == "Shift"                       ? kShift
                            : head == "Ctrl" || head == "Control" ? kCtrl
                            : head == "Alt"                       ? kAlt
                                                                  : 0;
        if (!bit)
          break;
        t.mods |= bit;
        part.remove_prefix(plus + 1);
      }
      t.key = std::string(part);
      t.vk = VkOf(part);
      out.push_back(std::move(t));
    }
    return out;
  }
  static Token MakeToken(uint8_t mods, uint16_t vk) {
    Token t;
    t.mods = uint8_t(mods & ~ModBit(vk));  // a modifier doesn't modify itself
    t.vk = vk;
    t.key = NameOf(vk);
    t.text = ModPrefix(t.mods) + t.key;
    return t;
  }
  static std::string ShownToken(const Token& t) {
    return t.vk ? ShownMods(t.mods) + Shown(t.key) : Shown(t.text);
  }

  static std::string Get(const char* name) { return rex::cvar::GetFlagByName(name); }
  static bool On(const char* name) { return Get(name) == "true"; }
  static std::string Default(const char* name) {
    const rex::cvar::FlagEntry* info = rex::cvar::GetFlagInfo(name);
    return info ? info->default_value : std::string();
  }
  void SetPlain(const char* name, const std::string& value) {
    rex::cvar::SetFlagByName(name, value);
    if (save_)
      save_(name, value);
  }
  // A string setting: quoted in the toml file.
  void SetString(const char* name, const std::string& value) {
    rex::cvar::SetFlagByName(name, value);
    std::string quoted = "\"";
    for (char c : value) {
      if (c == '"' || c == '\\')
        quoted += '\\';
      quoted += c;
    }
    if (save_)
      save_(name, quoted + "\"");
  }

  void Reload() {
    binds_.assign(kBinds, {});
    for (int i = 0; i < kBinds; ++i)
      binds_[i] = Parse(Get(kInputs[i].cvar));
  }
  void Write(int row) {
    std::string value;
    for (const Token& t : binds_[row])
      if (t.Valid())
        value += (value.empty() ? "" : ",") + t.text;
    SetString(kInputs[row].cvar, value);
  }

  void Note(std::string text) {
    message_ = std::move(text);
    message_until_ = Clock::now() + std::chrono::seconds(6);
  }

  // Puts `t` in slot `slot` of input `row`. Another input that had the key gets this slot's old
  // key instead (or loses it), so a key never does two things by accident.
  void Assign(int row, int slot, const Token& t) {
    Reload();
    std::vector<Token>& mine = binds_[row];
    const Token old = slot < int(mine.size()) ? mine[slot] : Token{};
    const std::string label = kInputs[row].label, key = ShownToken(t);
    if (old.Same(t)) {
      Note(label + ": key " + std::to_string(slot + 1) + " is already " + key + ".");
      return;
    }
    std::string moved;
    for (int r = 0; r < kBinds; ++r) {
      bool changed = false;
      for (int i = 0; i < int(binds_[r].size()); ++i) {
        if (!binds_[r][i].Same(t) || (r == row && i == slot))
          continue;
        const bool takes_old =
            old.Valid() && (r == row || std::none_of(binds_[r].begin(), binds_[r].end(),
                                                     [&](const Token& o) { return o.Same(old); }));
        binds_[r][i] = takes_old ? old : Token{};
        changed = true;
        if (r != row)
          moved = key + " was on " + kInputs[r].label +
                  (takes_old ? ", which gets " + ShownToken(old) + " instead."
                             : ": it's taken off there.");
      }
      if (changed && r != row)
        Write(r);
    }
    if (slot < int(mine.size()))
      mine[slot] = t;
    else
      mine.push_back(t);
    Write(row);
    std::string note = moved.empty() ? label + ": key " + std::to_string(slot + 1) + " is now " +
                                           key + "."
                                     : moved;
    // A modifier bound on its own is a plain button for the driver: its combinations stop.
    if (const uint8_t bit = ModBit(t.vk); bit && !t.mods) {
      bool combos = false;
      for (const auto& bind : binds_)
        for (const Token& o : bind)
          combos = combos || (o.mods & bit);
      if (combos)
        note += " " + Shown(t.key) + " on its own is a button now, so " + Shown(t.key) +
                "+ keys (like the D-pad's) stop working.";
    }
    Reload();
    Note(note);
  }

  void Clear(int row, int slot) {
    Reload();
    std::vector<Token>& mine = binds_[row];
    if (slot >= int(mine.size())) {
      Note(std::string(kInputs[row].label) + ": key " + std::to_string(slot + 1) +
           " has nothing to clear.");
      return;
    }
    mine.erase(mine.begin() + slot);
    Write(row);
    Reload();
    Note(std::string(kInputs[row].label) + ": key " + std::to_string(slot + 1) + " cleared.");
  }

  // Every binding and the mouse settings back to the driver's defaults, after a second press.
  void ResetAction() {
    if (!confirm_reset_) {
      confirm_reset_ = true;
      return;
    }
    confirm_reset_ = false;
    for (const Binding& in : kInputs)
      SetString(in.cvar, Default(in.cvar));
    for (const char* name : {"mnk_mouse", "mnk_sensitivity"})
      SetPlain(name, Default(name));
    Reload();
    Note("Every key and the mouse settings are back to the defaults.");
  }

  // Mouse sensitivity: 0.1 to 5 in steps of 0.1 (the driver takes 0.01 to 10).
  static constexpr double kMinSensitivity = 0.1, kMaxSensitivity = 5.0;
  static double Sensitivity() { return std::atof(Get("mnk_sensitivity").c_str()); }
  void SetSensitivity(double v) {
    v = std::clamp(std::round(v * 10.0) / 10.0, kMinSensitivity, kMaxSensitivity);
    if (std::abs(v - Sensitivity()) < 1e-6)
      return;
    char text[16];
    std::snprintf(text, sizeof(text), "%.1f", v);
    SetPlain("mnk_sensitivity", text);
  }

  void Change(int item, int delta) {
    if (item == kKeyboardRow)
      SetPlain("mnk_mode", On("mnk_mode") ? "false" : "true");
    else if (item == kMouseRow)
      SetPlain("mnk_mouse", On("mnk_mouse") ? "false" : "true");
    else if (item == kSensitivityRow)
      SetSensitivity(Sensitivity() + 0.1 * delta);
  }

  void Activate(int item) {
    if (item == kKeyboardRow || item == kMouseRow)
      Change(item, +1);
    else if (IsBind(item))
      StartCapture(item, slot_);
    else if (item == kResetRow)
      ResetAction();
    else if (item == kBackRow)
      Close();
  }

  // --- Picking a key -----------------------------------------------------------------------

  // The hotkeys (F1 settings, F2 FPS counter, the SDK's overlays...): every bind registered with
  // rex::ui::RegisterBind, matched by key alone as rex::ui::ProcessKeyEvent does.
  struct Hotkey {
    uint16_t vk;
    std::string what;
  };
  static std::vector<Hotkey> Hotkeys() {
    std::vector<Hotkey> out;
    for (const std::string& name : rex::cvar::ListFlagsByCategory("Input/Keybinds/System")) {
      const VK vk = rex::ui::ParseVirtualKey(rex::cvar::GetFlagByName(name));
      if (vk == VK::kNone)
        continue;
      const rex::cvar::FlagEntry* info = rex::cvar::GetFlagInfo(name);
      std::string what = info && !info->description.empty() ? info->description : name;
      what[0] = char(std::tolower(static_cast<unsigned char>(what[0])));
      out.push_back({uint16_t(vk), std::move(what)});
    }
    return out;
  }

  void StartCapture(int item, int slot) {
    capturing_ = true;
    capture_item_ = item;
    capture_slot_ = slot;
    capture_start_ = Clock::now();
    pending_mod_ = 0;
    message_.clear();
    confirm_reset_ = false;
    std::lock_guard lock(mutex_);
    // The key or click that started this is still down: it isn't the answer.
    std::copy(std::begin(held_), std::end(held_), std::begin(ignore_));
    queue_.clear();
    listen_capture_ = true;
  }
  void EndCapture() {
    capturing_ = false;
    std::lock_guard lock(mutex_);
    listen_capture_ = false;
    // The key that ended it stays away from the menu (and its repeats from a new capture) until
    // every key is let go.
    mute_ = true;
  }

  void Capture(const std::vector<Event>& events, const Pad& pad, bool held, bool lost) {
    for (const Event& ev : events) {
      if (ignore_[ev.vk]) {
        if (!ev.down)
          ignore_[ev.vk] = false;
        continue;
      }
      const uint8_t bit = ModBit(ev.vk);
      if (!ev.down) {
        // A modifier let go with no other key pressed meanwhile binds itself.
        if (bit && ev.vk == pending_mod_ && Finish(MakeToken(ev.mods, ev.vk)))
          return;
        continue;
      }
      if (ev.vk == uint16_t(VK::kEscape)) {
        Note("Nothing changed.");
        EndCapture();
        return;
      }
      if (ev.vk == uint16_t(VK::kDelete) || ev.vk == uint16_t(VK::kBack)) {
        EndCapture();
        Clear(capture_item_ - kFirstBind, capture_slot_);
        return;
      }
      if (bit) {
        pending_mod_ = ev.vk;
        continue;
      }
      pending_mod_ = 0;
      if (Finish(MakeToken(ev.mods, ev.vk)))
        return;
    }
    // A controller's B (with no key held, so not the keyboard's own) cancels too, as does
    // leaving the window or waiting.
    const bool pad_back = pad.b && !last_.b && !held;
    if (lost || pad_back || Clock::now() - capture_start_ > kCaptureTime) {
      Note(lost || pad_back ? "Nothing changed." : "No key pressed: nothing changed.");
      EndCapture();
    }
  }

  // The key picked: refused (true: keep waiting) or put in place.
  bool Finish(const Token& t) {
    if (t.key.empty()) {
      Note("That key can't be used: pick another one.");
      return false;
    }
    for (const Hotkey& h : Hotkeys())
      if (h.vk == t.vk) {
        Note(Shown(t.key) + " is a hotkey (" + h.what + "): pick another key.");
        return false;
      }
    EndCapture();
    Assign(capture_item_ - kFirstBind, capture_slot_, t);
    return true;
  }

  // A held direction fires once, then repeats after a short delay (as in the menu).
  static bool Fire(bool now, bool before, Clock::time_point& next) {
    const auto t = Clock::now();
    if (now && !before) {
      next = t + std::chrono::milliseconds(380);
      return true;
    }
    if (now && t >= next) {
      next = t + std::chrono::milliseconds(120);
      return true;
    }
    return false;
  }

  // --- Listening -----------------------------------------------------------------------------

  uint8_t HeldMods() const {  // with mutex_ held
    return uint8_t((held_[uint16_t(VK::kShift)] ? kShift : 0) |
                   (held_[uint16_t(VK::kControl)] ? kCtrl : 0) |
                   (held_[uint16_t(VK::kMenu)] ? kAlt : 0));
  }
  void Record(uint16_t vk, bool down, bool repeat, bool& handled) {
    if (vk >= 256)
      return;
    std::lock_guard lock(mutex_);
    held_[vk] = down;
    if (down && (listen_capture_ || mute_))
      handled = true;
    if (listen_capture_ && !(down && repeat))
      queue_.push_back({vk, down, HeldMods()});
  }
  void Key(rex::ui::KeyEvent& e, bool down) {
    bool handled = false;
    Record(uint16_t(e.virtual_key()), down, e.prev_state(), handled);
    if (handled)
      e.set_handled(true);
  }
  void Mouse(rex::ui::MouseEvent& e, bool down) {
    using B = rex::ui::MouseEvent::Button;
    const VK vk = e.button() == B::kLeft     ? VK::kLButton
                  : e.button() == B::kRight  ? VK::kRButton
                  : e.button() == B::kMiddle ? VK::kMButton
                                             : VK::kNone;
    if (vk == VK::kNone)
      return;
    bool handled = false;
    Record(uint16_t(vk), down, false, handled);
    if (handled)
      e.set_handled(true);
  }

  // --- Drawing -------------------------------------------------------------------------------

  static constexpr ImU32 kAccent = IM_COL32(214, 38, 38, 255);
  static constexpr ImU32 kText = IM_COL32(236, 238, 244, 255);
  static constexpr ImU32 kDim = IM_COL32(150, 156, 172, 255);
  static constexpr ImU32 kWhite = IM_COL32(255, 255, 255, 255);
  static constexpr ImU32 kArrow = IM_COL32(90, 96, 112, 255);
  // The key pictures' colours (key_glyphs.cpp).
  static constexpr ImU32 kInk = IM_COL32(30, 30, 30, 255);
  static constexpr ImU32 kSide = IM_COL32(142, 142, 142, 255);
  static constexpr ImU32 kFaceTop = IM_COL32(255, 255, 255, 255);
  static constexpr ImU32 kFaceBottom = IM_COL32(212, 212, 212, 255);
  static constexpr ImU32 kLit = IM_COL32(47, 123, 246, 255);
  static constexpr auto kCaptureTime = std::chrono::seconds(10);

  // Colours the vertices from `first` on top to bottom, from `top` at y0 to `bottom` at y1,
  // keeping their alpha (a rounded rectangle's anti-aliased edge).
  static void Shade(ImDrawList* dl, int first, float y0, float y1, ImU32 top, ImU32 bottom) {
    const auto channel = [](ImU32 c, int shift) { return float((c >> shift) & 0xFF); };
    for (int i = first; i < dl->VtxBuffer.Size; ++i) {
      ImDrawVert& v = dl->VtxBuffer[i];
      const float t = std::clamp((v.pos.y - y0) / std::max(y1 - y0, 1.0f), 0.0f, 1.0f);
      ImU32 col = v.col & IM_COL32_A_MASK;
      for (int shift : {IM_COL32_R_SHIFT, IM_COL32_G_SHIFT, IM_COL32_B_SHIFT}) {
        const float a = channel(top, shift), b = channel(bottom, shift);
        col |= ImU32(std::lround(a + (b - a) * t)) << shift;
      }
      v.col = col;
    }
  }

  // One part of a key picture: a name on a cap, an arrow cap, or a mouse with a button lit.
  struct Part {
    std::string text;
    int arrow = -1;  // 0 up, 1 left, 2 down, 3 right
    int mouse = -1;  // 0 left, 1 right, 2 middle button
  };
  static std::vector<Part> PartsOf(const Token& t) {
    std::vector<Part> parts;
    if (!t.vk) {
      parts.push_back({Shown(t.text)});
      return parts;
    }
    for (const auto& [bit, name] : {std::pair{kShift, "SHIFT"}, std::pair{kCtrl, "CTRL"},
                                    std::pair{kAlt, "ALT"}})
      if (t.mods & bit)
        parts.push_back({name});
    Part key;
    static constexpr VK kArrows[] = {VK::kUp, VK::kLeft, VK::kDown, VK::kRight};
    static constexpr VK kMice[] = {VK::kLButton, VK::kRButton, VK::kMButton};
    for (int i = 0; i < 4; ++i)
      if (t.vk == uint16_t(kArrows[i]))
        key.arrow = i;
    for (int i = 0; i < 3; ++i)
      if (t.vk == uint16_t(kMice[i]))
        key.mouse = i;
    if (key.arrow < 0 && key.mouse < 0)
      key.text = Shown(t.key);
    parts.push_back(key);
    return parts;
  }

  static ImFont* KeyFont() { return g_key_font ? g_key_font : ImGui::GetFont(); }
  // The size of the key font whose capitals fill `cap` pixels.
  static float KeyFontSize(float cap) { return cap / (g_key_font ? 0.61f : 0.8f); }

  // The width a part of height h takes: square for one letter or an arrow, wider for a name.
  static float PartWidth(const Part& p, float h) {
    if (p.mouse >= 0)
      return h * 0.68f;
    if (p.arrow >= 0 || p.text.size() <= 1)
      return h;
    const float face = h * (1.0f - 0.22f);
    const float size = KeyFontSize(face * 0.56f);
    const float tw = KeyFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, p.text.c_str()).x;
    return std::max(h, tw / 0.84f + h * 0.16f);
  }

  // A key cap at p, w x h (key_glyphs.cpp's look): a dark outline, a grey side below a white
  // face, the name or arrow on the face.
  static void DrawCap(ImDrawList* dl, ImVec2 p, float w, float h, const Part& part) {
    const float line = std::max(1.0f, h * 0.06f), side = std::max(1.0f, h * 0.1f), r = h * 0.2f;
    dl->AddRectFilled(p, ImVec2(p.x + w, p.y + h), kInk, r);
    dl->AddRectFilled(ImVec2(p.x + line, p.y + line), ImVec2(p.x + w - line, p.y + h - line),
                      kSide, r - line);
    const ImVec2 f0(p.x + line, p.y + line), f1(p.x + w - line, p.y + h - line - side);
    const int first = dl->VtxBuffer.Size;
    dl->AddRectFilled(f0, f1, kFaceTop, r - line);
    Shade(dl, first, f0.y, f1.y, kFaceTop, kFaceBottom);
    const float cx = (f0.x + f1.x) / 2, cy = (f0.y + f1.y) / 2, fh = f1.y - f0.y, fw = f1.x - f0.x;
    if (part.arrow >= 0) {
      static constexpr float kTip[4][2] = {{0, -1}, {-1, 0}, {0, 1}, {1, 0}};
      const float tx = kTip[part.arrow][0], ty = kTip[part.arrow][1], half = fh * 0.26f;
      dl->AddTriangleFilled(
          ImVec2(cx + tx * half, cy + ty * half),
          ImVec2(cx - tx * half * 0.6f - ty * half, cy - ty * half * 0.6f + tx * half),
          ImVec2(cx - tx * half * 0.6f + ty * half, cy - ty * half * 0.6f - tx * half), kInk);
      return;
    }
    ImFont* font = KeyFont();
    float size = KeyFontSize(fh * 0.56f);
    ImVec2 ts = font->CalcTextSizeA(size, FLT_MAX, 0.0f, part.text.c_str());
    if (ts.x > fw * 0.84f) {
      size *= fw * 0.84f / ts.x;
      ts = font->CalcTextSizeA(size, FLT_MAX, 0.0f, part.text.c_str());
    }
    dl->AddText(font, size, ImVec2(cx - ts.x / 2, cy - ts.y / 2), kInk, part.text.c_str());
  }

  // A mouse at p, as tall as h, the button meant lit (key_glyphs.cpp's look).
  static void DrawMouse(ImDrawList* dl, ImVec2 p, float h, int which) {
    const float bw = h * 0.68f, line = std::max(1.0f, h * 0.05f), r = bw * 0.48f;
    const ImVec2 b0 = p, b1(p.x + bw, p.y + h);
    dl->AddRectFilled(b0, b1, kInk, r);
    const ImVec2 i0(b0.x + line, b0.y + line), i1(b1.x - line, b1.y - line);
    const int first = dl->VtxBuffer.Size;
    dl->AddRectFilled(i0, i1, kFaceTop, r - line);
    Shade(dl, first, i0.y, i1.y, kFaceTop, kFaceBottom);
    const float split = p.y + h * 0.42f, mid = p.x + bw / 2;
    if (which == 0)
      dl->AddRectFilled(i0, ImVec2(mid, split), kLit, r - line, ImDrawFlags_RoundCornersTopLeft);
    if (which == 1)
      dl->AddRectFilled(ImVec2(mid, i0.y), ImVec2(i1.x, split), kLit, r - line,
                        ImDrawFlags_RoundCornersTopRight);
    dl->AddRectFilled(ImVec2(mid - line / 2, p.y), ImVec2(mid + line / 2, split), kInk);
    dl->AddRectFilled(ImVec2(i0.x, split - line / 2), ImVec2(i1.x, split + line / 2), kInk);
    const float ww = bw * 0.16f, wy0 = p.y + h * 0.12f, wy1 = split - h * 0.07f;
    dl->AddRectFilled(ImVec2(mid - ww / 2, wy0), ImVec2(mid + ww / 2, wy1),
                      which == 2 ? kLit : kInk, ww / 2);
  }

  // A binding's picture, centred on c, at most max_w wide: its modifiers' caps, "+", its key.
  static void DrawToken(ImDrawList* dl, ImFont* font, ImVec2 c, float h, float max_w,
                        const Token& t, float s) {
    const std::vector<Part> parts = PartsOf(t);
    const float plus_w = 16 * s;
    std::vector<float> widths;
    float total = 0;
    for (const Part& p : parts) {
      widths.push_back(PartWidth(p, h));
      total += widths.back();
    }
    total += plus_w * float(parts.size() - 1);
    const float squeeze = total > max_w ? max_w / total : 1.0f;
    float x = c.x - total * squeeze / 2;
    for (size_t i = 0; i < parts.size(); ++i) {
      const float w = widths[i] * squeeze;
      if (parts[i].mouse >= 0)
        DrawMouse(dl, ImVec2(x + (w - h * 0.68f * squeeze) / 2, c.y - h * squeeze / 2), h * squeeze,
                  parts[i].mouse);
      else
        DrawCap(dl, ImVec2(x, c.y - h / 2), w, h, parts[i]);
      x += w;
      if (i + 1 < parts.size()) {
        const float size = 14 * s;
        const ImVec2 ps = font->CalcTextSizeA(size, FLT_MAX, 0.0f, "+");
        dl->AddText(font, size, ImVec2(x + (plus_w * squeeze - ps.x) / 2, c.y - ps.y / 2), kDim,
                    "+");
        x += plus_w * squeeze;
      }
    }
  }

  static void Centred(ImDrawList* dl, ImFont* font, float size, ImVec2 c, ImU32 col,
                      const char* text) {
    const ImVec2 ts = font->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
    dl->AddText(font, size, ImVec2(c.x - ts.x / 2, c.y - ts.y / 2), col, text);
  }

  void DrawList(ImGuiIO& io, ImFont* font, float s, float row_h, float header_h) {
    ImDrawList* dl = ImGui::GetWindowDrawList();
    const float w = ImGui::GetContentRegionAvail().x;
    const float value_x = 360 * s, gap = 14 * s;
    const float slot_w = (w - value_x - 12 * s - gap) / 2;
    const bool mouse_ok = !capturing_;  // clicks are the answer while a key is being picked
    const bool moved = io.MouseDelta.x != 0 || io.MouseDelta.y != 0;
    const float fh = ImGui::GetFontSize();
    for (int item = 0; item < kItems; ++item) {
      if (item == kFirstBind) {  // the bindings' heading
        const ImVec2 h0 = ImGui::GetCursorScreenPos();
        ImGui::Dummy(ImVec2(w, header_h));
        dl->AddLine(ImVec2(h0.x, h0.y + 6 * s), ImVec2(h0.x + w, h0.y + 6 * s),
                    IM_COL32(255, 255, 255, 30), 1.5f * s);
        const float hy = h0.y + header_h * 0.6f;
        dl->AddText(font, 11 * s, ImVec2(h0.x + 20 * s, hy - 5.5f * s), kDim, "BUTTON");
        for (int k = 0; k < 2; ++k)
          Centred(dl, font, 11 * s, ImVec2(h0.x + value_x + k * (slot_w + gap) + slot_w / 2, hy),
                  kDim, k == 0 ? "KEY 1" : "KEY 2");
      }
      const float local_y = ImGui::GetCursorPosY();
      const ImVec2 p0 = ImGui::GetCursorScreenPos(), p1(p0.x + w, p0.y + row_h);
      ImGui::PushID(item);
      ImGui::InvisibleButton("row", ImVec2(w, row_h));
      const bool hovered = mouse_ok && ImGui::IsItemHovered();
      ImGui::PopID();
      // Keeps the selection in view when the pad or the keys moved it.
      if (follow_ > 0 && item == selected_) {
        const float top = local_y - (item == kFirstBind ? header_h : 0.0f);
        const float sy = ImGui::GetScrollY(), view = ImGui::GetWindowHeight();
        if (item == 0)
          ImGui::SetScrollY(0);
        else if (top < sy)
          ImGui::SetScrollY(top);
        else if (local_y + row_h + 4 * s > sy + view)
          ImGui::SetScrollY(local_y + row_h + 4 * s - view);
        --follow_;
      }
      const float cy = p0.y + row_h * 0.5f;
      // The two key places of a binding row.
      ImVec2 slot0[2], slot1[2];
      for (int k = 0; k < 2; ++k) {
        slot0[k] = ImVec2(p0.x + value_x + k * (slot_w + gap), p0.y + 4 * s);
        slot1[k] = ImVec2(slot0[k].x + slot_w, p1.y - 4 * s);
      }
      int hover_slot = -1;
      if (hovered && IsBind(item))
        for (int k = 0; k < 2; ++k)
          if (ImGui::IsMouseHoveringRect(slot0[k], slot1[k]))
            hover_slot = k;
      if (hovered && (moved || ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
                      ImGui::IsMouseClicked(ImGuiMouseButton_Right))) {
        if (selected_ != item)
          confirm_reset_ = false;
        selected_ = item;
        if (hover_slot >= 0)
          slot_ = hover_slot;
      }
      if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        const float mid = p0.x + value_x + (w - value_x) * 0.5f;
        if (item == kSensitivityRow && io.MousePos.x > p0.x + value_x + 34 * s &&
            io.MousePos.x < p1.x - 26 * s)
          dragging_ = true;
        else if (item <= kSensitivityRow)
          Change(item, io.MousePos.x < mid ? -1 : +1);
        else if (IsBind(item)) {
          if (hover_slot >= 0)
            StartCapture(item, hover_slot);
        } else {
          Activate(item);
        }
      }
      if (hovered && hover_slot >= 0 && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        Clear(item - kFirstBind, hover_slot);

      const bool sel = item == selected_;
      if (sel) {
        dl->AddRectFilled(p0, p1, IM_COL32(255, 255, 255, 16), 8 * s);
        dl->AddRectFilled(p0, ImVec2(p0.x + 5 * s, p1.y), kAccent, 3 * s);
      }
      if (item == kResetRow || item == kBackRow) {
        const char* label = item == kBackRow    ? "Back"
                            : confirm_reset_    ? "Press again to reset"
                                                : "Reset to defaults";
        Centred(dl, font, fh, ImVec2(p0.x + w / 2, cy),
                item == kResetRow && confirm_reset_ ? kAccent : (sel ? kText : kDim), label);
        continue;
      }
      if (IsBind(item)) {
        DrawBindRow(dl, font, s, item, p0, cy, slot0, slot1, sel, hover_slot);
        continue;
      }
      DrawSettingRow(io, dl, font, s, item, p0, p1, cy, value_x, sel);
    }
  }

  void DrawSettingRow(ImGuiIO& io, ImDrawList* dl, ImFont* font, float s, int item, ImVec2 p0,
                      ImVec2 p1, float cy, float value_x, bool sel) {
    const float fh = ImGui::GetFontSize();
    static constexpr const char* kNames[] = {"Keyboard controls", "Mouse as right stick",
                                             "Mouse sensitivity"};
    dl->AddText(ImVec2(p0.x + 20 * s, cy - fh * 0.5f), sel ? kText : kDim, kNames[item]);
    const float lx = p0.x + value_x + 10 * s, rx = p1.x - 18 * s;
    const float mid = p0.x + value_x + (p1.x - p0.x - value_x) * 0.5f;
    const ImU32 value_col = sel ? kWhite : kText;
    if (item == kSensitivityRow) {
      // The value over a bar: click or drag on the bar, or step with the arrows.
      const float bx0 = lx + 34 * s, bx1 = rx - 16 * s, by = cy + 11 * s;
      if (dragging_) {
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
          dragging_ = false;
        else
          SetSensitivity(kMinSensitivity + (kMaxSensitivity - kMinSensitivity) *
                                                std::clamp((io.MousePos.x - bx0) / (bx1 - bx0),
                                                           0.0f, 1.0f));
      }
      const double v = Sensitivity();
      char text[24];
      std::snprintf(text, sizeof(text), std::abs(v * 10 - std::round(v * 10)) < 1e-6 ? "%.1f"
                                                                                     : "%.2f",
                    v);
      Centred(dl, font, fh, ImVec2(mid, cy - 7 * s), value_col, text);
      const float t = float(std::clamp((v - kMinSensitivity) /
                                           (kMaxSensitivity - kMinSensitivity), 0.0, 1.0));
      dl->AddRectFilled(ImVec2(bx0, by - 2 * s), ImVec2(bx1, by + 2 * s),
                        IM_COL32(255, 255, 255, 40), 2 * s);
      const float kx = bx0 + (bx1 - bx0) * t;
      dl->AddRectFilled(ImVec2(bx0, by - 2 * s), ImVec2(kx, by + 2 * s),
                        sel ? kAccent : IM_COL32(178, 184, 198, 255), 2 * s);
      dl->AddCircleFilled(ImVec2(kx, by), 6 * s, sel ? kWhite : kText);
    } else {
      const bool on = On(item == kKeyboardRow ? "mnk_mode" : "mnk_mouse");
      Centred(dl, font, fh, ImVec2(mid, cy), value_col, on ? "On" : "Off");
    }
    const ImU32 arrow = sel ? kAccent : kArrow;
    const float ax = 9 * s, ay = 8 * s;
    dl->AddTriangleFilled(ImVec2(lx, cy), ImVec2(lx + ax, cy - ay), ImVec2(lx + ax, cy + ay),
                          arrow);
    dl->AddTriangleFilled(ImVec2(rx + ax, cy), ImVec2(rx, cy - ay), ImVec2(rx, cy + ay), arrow);
  }

  void DrawBindRow(ImDrawList* dl, ImFont* font, float s, int item, ImVec2 p0, float cy,
                   const ImVec2 (&slot0)[2], const ImVec2 (&slot1)[2], bool sel,
                   int hover_slot) {
    const int row = item - kFirstBind;
    const float fh = ImGui::GetFontSize();
    const bool keyboard_on = On("mnk_mode");
    dl->AddText(ImVec2(p0.x + 20 * s, cy - fh * 0.5f),
                sel ? kText : (keyboard_on ? kDim : IM_COL32(110, 115, 130, 255)),
                kInputs[row].label);
    const std::vector<Token>& tokens = binds_[row];
    const float t = float(ImGui::GetTime());
    for (int k = 0; k < 2; ++k) {
      const bool picking = capturing_ && capture_item_ == item && capture_slot_ == k;
      const bool chosen = sel && slot_ == k;
      dl->AddRectFilled(slot0[k], slot1[k], IM_COL32(255, 255, 255, chosen ? 22 : 9), 7 * s);
      const ImVec2 c((slot0[k].x + slot1[k].x) / 2, (slot0[k].y + slot1[k].y) / 2);
      if (picking) {
        const ImU32 pulse = ImU32(170 + 85 * std::sin(t * 6.0f)) << IM_COL32_A_SHIFT;
        dl->AddRect(slot0[k], slot1[k], (kAccent & ~IM_COL32_A_MASK) | pulse, 7 * s, 0, 2.5f * s);
        Centred(dl, font, 11 * s, ImVec2(c.x, c.y - 2 * s), kWhite, "Press a key...");
        // The time left, as a line along the bottom.
        const float left = 1.0f - std::chrono::duration<float>(Clock::now() - capture_start_) /
                                      std::chrono::duration<float>(kCaptureTime);
        const float lw = (slot1[k].x - slot0[k].x - 16 * s) * std::clamp(left, 0.0f, 1.0f);
        dl->AddRectFilled(ImVec2(slot0[k].x + 8 * s, slot1[k].y - 6 * s),
                          ImVec2(slot0[k].x + 8 * s + lw, slot1[k].y - 4 * s), kAccent);
        continue;
      }
      if (chosen)
        dl->AddRect(slot0[k], slot1[k], kAccent, 7 * s, 0, 2 * s);
      else if (hover_slot == k)
        dl->AddRect(slot0[k], slot1[k], IM_COL32(255, 255, 255, 70), 7 * s, 0, 1.5f * s);
      if (k < int(tokens.size()))
        DrawToken(dl, font, c, (slot1[k].y - slot0[k].y) - 5 * s,
                  slot1[k].x - slot0[k].x - 14 * s, tokens[k], s);
      else
        Centred(dl, font, 11 * s, c, IM_COL32(110, 115, 130, 255), "none");
    }
    // More keys than the two places (set in the settings file): kept, and counted here.
    if (tokens.size() > 2) {
      const std::string more = "+" + std::to_string(tokens.size() - 2);
      dl->AddText(font, 11 * s, ImVec2(slot1[1].x + 3 * s, cy - 5.5f * s), kDim, more.c_str());
    }
  }

  std::string Hint() const {
    if (capturing_)
      return std::string("Press the key or mouse button for ") +
             kInputs[capture_item_ - kFirstBind].label + " (key " +
             std::to_string(capture_slot_ + 1) +
             "). Hold Shift, Ctrl or Alt with it for a combination. Esc cancels; Delete or "
             "Backspace clears the key.";
    switch (selected_) {
      case kKeyboardRow:
        return "On: the keyboard and mouse work as a controller, with the keys below. Off: only "
               "controllers work.";
      case kMouseRow:
        return "On: moving the mouse moves the controller's right stick, as the right stick's "
               "keys below do. The pointer is hidden while you play.";
      case kSensitivityRow:
        return std::string("How far the right stick moves when you move the mouse. 1.0 is the "
                           "default.") +
               (On("mnk_mouse") ? "" : " Mouse as right stick is off.");
      case kResetRow:
        return confirm_reset_ ? "Press again to set every key and the mouse settings back to "
                                "the defaults."
                              : "Every key and the mouse settings back to the defaults (press "
                                "twice).";
      case kBackRow:
        return "Back to Controls. Your changes are saved already.";
    }
    std::string hint =
        "A, Enter or a click: change the key. X, Delete or a right-click: clear it. Hold "
        "Shift, Ctrl or Alt with a key for a combination.";
    const std::vector<Token>& tokens = binds_[selected_ - kFirstBind];
    if (tokens.size() > 2) {
      hint += " Also from the settings file:";
      for (size_t i = 2; i < tokens.size(); ++i)
        hint += (i > 2 ? ", " : " ") + ShownToken(tokens[i]);
      hint += ".";
    }
    return hint;
  }

  rex::ui::Window* window_;
  SaveFn save_;
  bool open_ = false, listening_ = false, first_input_ = false;
  int selected_ = 0, slot_ = 0;
  int follow_ = 0;  // frames left to keep the selection in view (the list may not be laid out yet)
  bool confirm_reset_ = false, dragging_ = false;
  std::vector<std::vector<Token>> binds_;
  std::string message_;
  Clock::time_point message_until_;
  Pad last_;
  Clock::time_point rep_[4];
  int dev_item_ = -1;
  bool dev_capture_ = false;
  // Picking a key (UI side).
  bool capturing_ = false;
  int capture_item_ = 0, capture_slot_ = 0;
  Clock::time_point capture_start_;
  bool ignore_[256] = {};
  uint16_t pending_mod_ = 0;
  // From the window's listener (the UI thread, as the drawing; guarded all the same).
  std::mutex mutex_;
  bool held_[256] = {};
  bool listen_capture_ = false, mute_ = false, lost_focus_ = false;
  std::vector<Event> queue_;
};

// The Controls page: one row of the settings menu opens it, and it opens the Keyboard & mouse
// setup page above. B or Esc goes back a page at a time.
class ControlsPage {
 public:
  using Pad = KeyboardSetupPage::Pad;
  using SaveFn = KeyboardSetupPage::SaveFn;

  // `set_controllers_shared`: the game's Controllers switch (every controller is player 1, or
  // each its own player); empty in a game without it, which then has no Controllers row.
  ControlsPage(rex::ui::Window* window, SaveFn save,
               std::function<void(bool)> set_controllers_shared)
      : keys_(window, save), save_(std::move(save)),
        set_shared_(std::move(set_controllers_shared)) {}

  bool IsOpen() const { return open_; }
  void Open() {
    open_ = true;
    first_input_ = true;
    selected_ = 0;
    if (std::exchange(dev_keys_, false)) {
      selected_ = int(Items().size()) - 2;  // Keyboard & mouse setup
      keys_.Open();
    }
  }
  void Close() {
    keys_.Close();
    open_ = false;
  }
  // The settings menu was opened or closed (mouse look is off while it's open).
  static void MenuShown(bool shown) { KeyboardSetupPage::MenuShown(shown); }

  // Developer aid: SVR_OPEN_CONTROLS_AT_MS=<ms> opens this page once that long after start;
  // SVR_OPEN_KEYS_PAGE_AT_MS (KeyboardSetupPage) the Keyboard & mouse page over it. True once,
  // when the menu should open and then Open() this page.
  bool DevAutoOpen() {
    if (keys_.DevAutoOpen()) {
      dev_keys_ = true;
      return true;
    }
    static const char* const spec = std::getenv("SVR_OPEN_CONTROLS_AT_MS");
    static const auto start = Clock::now();
    static bool done = false;
    if (!spec || done || Clock::now() - start < std::chrono::milliseconds(std::atoll(spec)))
      return false;
    done = true;
    return true;
  }

  void Input(const Pad& pad) {
    if (keys_.IsOpen()) {
      keys_.Input(pad);
      last_ = pad;
      return;
    }
    if (first_input_) {  // what's held already was pressed to get here
      first_input_ = false;
      last_ = pad;
      return;
    }
    const bool up = Fire(pad.up, last_.up, rep_[0]) || ImGui::IsKeyPressed(ImGuiKey_UpArrow);
    const bool down =
        Fire(pad.down, last_.down, rep_[1]) || ImGui::IsKeyPressed(ImGuiKey_DownArrow);
    const bool left =
        Fire(pad.left, last_.left, rep_[2]) || ImGui::IsKeyPressed(ImGuiKey_LeftArrow);
    const bool right =
        Fire(pad.right, last_.right, rep_[3]) || ImGui::IsKeyPressed(ImGuiKey_RightArrow);
    const bool accept = (pad.a && !last_.a) || ImGui::IsKeyPressed(ImGuiKey_Enter, false) ||
                        ImGui::IsKeyPressed(ImGuiKey_Space, false);
    const bool back = (pad.b && !last_.b) || ImGui::IsKeyPressed(ImGuiKey_Escape, false);
    last_ = pad;
    const int n = int(Items().size());
    if (up || down)
      selected_ = (selected_ + (up ? n - 1 : 1)) % n;
    if (left || right)
      Change(Items()[selected_], right ? +1 : -1);
    if (accept)
      Activate(Items()[selected_]);
    if (back)
      Close();
  }

  void Draw(ImGuiIO& io, ImFont* font) {
    if (keys_.IsOpen()) {
      keys_.Draw(io, font);
      return;
    }
    if (!font)
      font = ImGui::GetFont();
    const float s = std::max(0.6f, io.DisplaySize.y / 1080.0f);
    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(0, 0), io.DisplaySize,
                                                  IM_COL32(0, 0, 0, 150));
    ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
                            ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(900 * s, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 14 * s);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(30 * s, 26 * s));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(18, 21, 30, 245));
    ImGui::PushFont(font, 16 * s);
    constexpr ImGuiWindowFlags kFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav |
                                        ImGuiWindowFlags_AlwaysAutoResize;
    if (ImGui::Begin("##svr_controls_page", nullptr, kFlags)) {
      ImDrawList* dl = ImGui::GetWindowDrawList();
      const float inner = ImGui::GetContentRegionAvail().x;
      ImGui::PushFont(font, 30 * s);
      ImGui::TextColored(ImColor(kWhite), "CONTROLS");
      ImGui::PopFont();
      ImGui::PushFont(font, 13 * s);
      ImGui::TextColored(ImColor(kDim), "Button prompts, keyboard and controllers");
      ImGui::PopFont();
      ImGui::Dummy(ImVec2(0, 12 * s));

      const float row_h = 40 * s, value_x = 400 * s, fh = ImGui::GetFontSize();
      const std::vector<Item> items = Items();
      const bool moved = io.MouseDelta.x != 0 || io.MouseDelta.y != 0;
      for (int i = 0; i < int(items.size()); ++i) {
        const Item item = items[i];
        const ImVec2 p0 = ImGui::GetCursorScreenPos(), p1(p0.x + inner, p0.y + row_h);
        ImGui::PushID(i);
        ImGui::InvisibleButton("row", ImVec2(inner, row_h));
        const bool hovered = ImGui::IsItemHovered();
        ImGui::PopID();
        if (hovered && (moved || ImGui::IsMouseClicked(ImGuiMouseButton_Left)))
          selected_ = i;
        if (hovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
          if (item == kSetup || item == kBack)
            Activate(item);
          else
            Change(item, io.MousePos.x < p0.x + value_x + (inner - value_x) * 0.5f ? -1 : +1);
        }
        const bool sel = i == selected_;
        if (sel) {
          dl->AddRectFilled(p0, p1, IM_COL32(255, 255, 255, 16), 8 * s);
          dl->AddRectFilled(p0, ImVec2(p0.x + 5 * s, p1.y), kAccent, 3 * s);
        }
        const float cy = p0.y + row_h * 0.5f;
        if (item == kBack) {
          const float tw = ImGui::CalcTextSize("Back").x;
          dl->AddText(ImVec2(p0.x + (inner - tw) * 0.5f, cy - fh * 0.5f), sel ? kText : kDim,
                      "Back");
          continue;
        }
        dl->AddText(ImVec2(p0.x + 20 * s, cy - fh * 0.5f), sel ? kText : kDim, Name(item));
        const std::string value = Value(item);
        const float vw = ImGui::CalcTextSize(value.c_str()).x;
        const float mid = p0.x + value_x + (inner - value_x) * 0.5f;
        dl->AddText(ImVec2(mid - vw * 0.5f, cy - fh * 0.5f), sel ? kWhite : kText,
                    value.c_str());
        if (item == kSetup)  // a button, not a choice: no arrows
          continue;
        const ImU32 arrow = sel ? kAccent : IM_COL32(90, 96, 112, 255);
        const float ax = 9 * s, ay = 8 * s;
        const float lx = p0.x + value_x + 10 * s, rx = p1.x - 18 * s;
        dl->AddTriangleFilled(ImVec2(lx, cy), ImVec2(lx + ax, cy - ay), ImVec2(lx + ax, cy + ay),
                              arrow);
        dl->AddTriangleFilled(ImVec2(rx + ax, cy), ImVec2(rx, cy - ay), ImVec2(rx, cy + ay),
                              arrow);
      }

      ImGui::Dummy(ImVec2(0, 10 * s));
      ImGui::PushFont(font, 13 * s);
      ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(178, 184, 198, 255));
      ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + inner);
      const float hint_y = ImGui::GetCursorPosY();
      ImGui::TextWrapped("%s", Hint(items[std::clamp(selected_, 0, int(items.size()) - 1)]));
      ImGui::SetCursorPosY(
          std::max(ImGui::GetCursorPosY(), hint_y + 3 * ImGui::GetTextLineHeightWithSpacing()));
      ImGui::PopTextWrapPos();
      ImGui::PopStyleColor();
      ImGui::Dummy(ImVec2(0, 6 * s));
      ImGui::TextColored(ImColor(kDim), "Up/Down choose   Left/Right change   B/Esc back");
      ImGui::PopFont();
    }
    ImGui::End();
    ImGui::PopFont();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
  }

 private:
  using Clock = std::chrono::steady_clock;
  enum Item { kPrompts, kKeyboard, kControllers, kSetup, kBack };
  static constexpr ImU32 kAccent = IM_COL32(214, 38, 38, 255);
  static constexpr ImU32 kText = IM_COL32(236, 238, 244, 255);
  static constexpr ImU32 kDim = IM_COL32(150, 156, 172, 255);
  static constexpr ImU32 kWhite = IM_COL32(255, 255, 255, 255);

  std::vector<Item> Items() const {
    std::vector<Item> items = {kPrompts, kKeyboard};
    if (set_shared_)
      items.push_back(kControllers);
    items.push_back(kSetup);
    items.push_back(kBack);
    return items;
  }

  static std::string Get(const char* name) { return rex::cvar::GetFlagByName(name); }
  void Set(const char* name, const std::string& value) {
    rex::cvar::SetFlagByName(name, value);
    if (save_)
      save_(name, value);
  }

  // svr_button_prompts: 0 Auto, 1 Xbox, 2 PlayStation, 3 Keyboard (pad_icons.cpp).
  static constexpr const char* kPromptNames[] = {"Auto", "Xbox", "PlayStation", "Keyboard"};
  static int Prompts() { return std::clamp(std::atoi(Get("svr_button_prompts").c_str()), 0, 3); }

  static const char* Name(Item item) {
    switch (item) {
      case kPrompts: return "Button prompts";
      case kKeyboard: return "Keyboard controls";
      case kControllers: return "Controllers";
      case kSetup: return "Keyboard & mouse setup";
      default: return "Back";
    }
  }
  static std::string Value(Item item) {
    switch (item) {
      case kPrompts: return kPromptNames[Prompts()];
      case kKeyboard: return Get("mnk_mode") == "true" ? "On" : "Off";
      case kControllers:
        return Get("svr_controllers_shared") == "true" ? "All on player 1" : "Separate players";
      case kSetup: return "Set up keys";
      default: return "";
    }
  }
  static const char* Hint(Item item) {
    switch (item) {
      case kPrompts:
        return "The buttons the game shows in its menus, tips and prompts. Auto follows what you "
               "play with: PlayStation buttons for a DualShock or DualSense, your keys (and "
               "mouse) for the keyboard, Xbox buttons otherwise.";
      case kKeyboard:
        return "On: the keyboard (and mouse) work as a controller, with the keys in Keyboard & "
               "mouse setup. Off: only controllers work.";
      case kControllers:
        return "Separate players: each controller is its own player, for local multiplayer. All "
               "on player 1: every controller controls player 1, so a controller that drops out "
               "or shows up twice (Steam Input, DS4Windows) can't leave the game waiting for a "
               "player 2.";
      case kSetup:
        return "The keys and mouse buttons for every controller button, and the mouse as the "
               "right stick. Press A to open.";
      default:
        return "Back to the settings. Your changes are saved.";
    }
  }

  void Change(Item item, int delta) {
    switch (item) {
      case kPrompts:
        Set("svr_button_prompts", std::to_string((Prompts() + delta + 4) % 4));
        break;
      case kKeyboard:
        Set("mnk_mode", Get("mnk_mode") == "true" ? "false" : "true");
        break;
      case kControllers: {
        const bool shared = Get("svr_controllers_shared") != "true";
        Set("svr_controllers_shared", shared ? "true" : "false");
        set_shared_(shared);
        break;
      }
      default:
        break;
    }
  }
  void Activate(Item item) {
    if (item == kSetup)
      keys_.Open();
    else if (item == kBack)
      Close();
    else
      Change(item, +1);
  }

  // A held direction fires once, then repeats after a short delay (as in the menu).
  static bool Fire(bool now, bool before, Clock::time_point& next) {
    const auto t = Clock::now();
    if (now && !before) {
      next = t + std::chrono::milliseconds(380);
      return true;
    }
    if (now && t >= next) {
      next = t + std::chrono::milliseconds(120);
      return true;
    }
    return false;
  }

  KeyboardSetupPage keys_;
  SaveFn save_;
  std::function<void(bool)> set_shared_;
  bool open_ = false, first_input_ = false, dev_keys_ = false;
  int selected_ = 0;
  Pad last_;
  Clock::time_point rep_[4];
};

}  // namespace svr
