// SvR 2007's textures with Xbox buttons, and where the buttons are on them: read by pad_icons.cpp
// (inside its namespace, after the types). Hashes: the FNV-1a 64 of the base level as stored on
// the disc and with its 16-bit words swapped (32-bit words for 32-bit colour). The game always
// runs in HD here.

// The fonts' icon page: 256 x 512 (menuHD.pac's FNT2, FNTG and FNTM fonts 0 to 30 and TITL's font,
// game2dHD.pac's, gm_mode.pac's GFNT).
constexpr uint64_t kPageHashes[] = {0xB43FC5CABCE7B3D4ull, 0x5B6C8BF73FA91E02ull};

// The places of the Xbox buttons on the page (x, y, width, height), from the fonts' icon glyphs;
// tools/pad_icons/make_playstation_icons.ps1 draws the PlayStation ones in the same places
// (playstation.png).
constexpr Cell kCells[] = {
    {1, 0, 55, 56},    {57, 0, 55, 56},    {113, 0, 55, 56},   {169, 0, 55, 56},   // X Y B A
    {0, 56, 58, 56},   {59, 56, 58, 56},   {178, 56, 57, 56},  // Back Start Guide
    {0, 112, 52, 56},  {53, 112, 52, 56},  {106, 112, 70, 56}, {177, 112, 70, 56}, // LT RT LB RB
    {0, 168, 61, 56},  {62, 168, 60, 56},  {176, 168, 59, 56}, {198, 224, 58, 56}, // sticks
    {0, 224, 58, 56},  {119, 224, 51, 56},  // the sticks of "LS with arrows" (arrows stay)
};

// Font 40's page (menuHD.pac's FNT2 and FNTG, evt2dHD.pac's): the same but for the shoulder
// buttons' row, with narrower LT / RT and wider LB / RB (playstation_40.png).
constexpr uint64_t kPage40Hashes[] = {0xCB935074DC8DF88Cull, 0x716362435F666F86ull};
constexpr Cell kCells40[] = {
    {1, 0, 55, 56},    {57, 0, 55, 56},    {113, 0, 55, 56},   {169, 0, 55, 56},   // X Y B A
    {0, 56, 58, 56},   {59, 56, 58, 56},   {178, 56, 57, 56},  // Back Start Guide
    {0, 112, 39, 56},  {40, 112, 41, 56},  {82, 112, 82, 56},  {165, 112, 82, 56}, // LT RT LB RB
    {0, 168, 61, 56},  {62, 168, 60, 56},  {176, 168, 59, 56}, {198, 224, 58, 56}, // sticks
    {0, 224, 58, 56},  {119, 224, 51, 56},  // the sticks of "LS with arrows" (arrows stay)
};

// The FNTM font 0's page, 512 x 512 (menuHD.pac): its top left is the 256 x 512 page's layout
// again; below that (LS) and on the right (RS) are the sticks with arrows, 78 x 76 each. Their
// "LS" / "RS" get a stick top with PlayStation's L / R laid over them; the arrows stay.
constexpr uint64_t kBigPageHashes[] = {0x1FD8A58C1860239Eull, 0x5AF23A7411404240ull};
constexpr Cell kStickLabels[] = {
    {24, 304, 36, 26},  {102, 304, 36, 26}, {180, 304, 36, 26},  // LS
    {24, 380, 36, 26},  {102, 380, 36, 26}, {180, 380, 36, 26},
    {24, 456, 36, 26},  {102, 456, 36, 26}, {180, 456, 36, 26},
    {282, 24, 36, 26},  {360, 24, 36, 26},  {438, 24, 36, 26},   // RS
    {282, 100, 36, 26}, {360, 100, 36, 26}, {438, 100, 36, 26},
    {282, 176, 36, 26}, {360, 176, 36, 26}, {438, 176, 36, 26},
};

// The loading tips' pictures with buttons (game2dHD.pac TIPS / TIPI, 1024 x 1024; the same in
// both languages). The controller diagram (tip 0): the controller's place given a DualShock 3.
// Two screenshots: ours laid over their sticks (tip 9: "REACH LS or RS", "Move LS RS to find
// Sweet Spot") and their B and right stick (tip 19: "Press B to STOP").
constexpr uint64_t kTipControllerHashes[] = {0xFF91A2F2D53B4E15ull, 0x5B1AC02885E0A189ull};
constexpr Cell kTipController[] = {{356, 146, 314, 216}};
constexpr uint64_t kTipSticksHashes[] = {0xA5C7FA6BD8A99948ull, 0xE9E2FC7D479C2302ull};
constexpr Cell kTipSticks[] = {
    {598, 119, 56, 56}, {704, 119, 56, 56}, {624, 395, 50, 54}, {674, 395, 50, 54}};
constexpr uint64_t kTipStopHashes[] = {0x04F68D9B3D94B584ull, 0x6305EC107822BF46ull};
constexpr Cell kTipStop[] = {{637, 344, 31, 27}, {629, 370, 48, 45}};

// The button of each place, for the keyboard's keys (key_glyphs.h).
constexpr Btn kCellKeys[] = {
    Square, Triangle, Circle, Cross, Select, Start, PS, L2, R2, L1, R1, LStick, RStick, L3, R3,
    LStick, LStick,
};
constexpr Btn kStickLabelKeys[] = {
    LStick, LStick, LStick, LStick, LStick, LStick, LStick, LStick, LStick,
    RStick, RStick, RStick, RStick, RStick, RStick, RStick, RStick, RStick,
};

constexpr Page kPages[] = {
    {256, 512, kPageHashes, "playstation", kCells, {}, kCellKeys},
    {256, 512, kPage40Hashes, "playstation_40", kCells40, {}, kCellKeys},
    {512, 512, kBigPageHashes, "playstation_512", kCells, kStickLabels, kCellKeys,
     kStickLabelKeys},
    {1024, 1024, kTipControllerHashes, "tip_controller", kTipController, {}},
    {1024, 1024, kTipSticksHashes, "tip_sticks", {}, kTipSticks},
    {1024, 1024, kTipStopHashes, "tip_stop", {}, kTipStop},
};

// Pictures of their own, replaced whole by the drawing of the same size: the controls screen's
// controller and the silhouette drawn behind it (menuHD.pac POSI contro_01A / 01B, 128 x 128, and
// contro_02A / 02B, 256 x 128; 32-bit and 16-bit colour).
constexpr Whole kWhole[] = {
    {128, 128, {0x5223C826224AE1C7ull, 0x60F3AA4F04D521F3ull}, "controller"},
    {128, 128, {0xA687B3F202B36AFFull, 0xDBA75A6F50A56F9Full}, "controller_shadow"},
    {256, 128, {0x31541002B612089Eull, 0xFC1F596430AE7294ull}, "controller_wide"},
    {256, 128, {0x99A9E0FC4A15ADD5ull, 0xF6E620C55AADEA75ull}, "controller_wide_shadow"},
};

// The game's other button pictures: the match's prompts (game2dHD.pac BASE; GA2D English, GI2D
// Italian, the plain buttons shared) and Create-a-Superstar's (menuHD.pac CASI).
constexpr Prompt kPrompts[] = {
    // 0/3/1, 0/4/0, 0/8/1: the A, X and B of button prompts and mashing.
    {32, 32, {0xA03AFF08E78025F3ull, 0x82FC011A0C03EE63ull}, "small A", {{0, 0, 32, 32, Cross}}},
    {32, 32, {0x06DD722F2D2F08F2ull, 0x6FBC04AF1F84BB18ull}, "small X", {{0, 0, 32, 32, Square}}},
    {32, 32, {0x8ABC12A5D74F3899ull, 0x32BBDCEE67ED573Dull}, "small B", {{0, 0, 32, 32, Circle}}},
    // 1/0/2-5: A, B, Y and X, bigger.
    {64, 64, {0x91E06F38282AAA39ull, 0x919F47B0C0F65649ull}, "big A", {{10, 2, 46, 51, Cross}}},
    {64, 64, {0x700725B4CCC2D316ull, 0xA0C21082D9E35830ull}, "big B", {{10, 2, 46, 51, Circle}}},
    {64, 64, {0xE14B3800FF79DFBCull, 0x28899E45132165AAull}, "big Y",
     {{10, 2, 46, 51, Triangle}}},
    {64, 64, {0x274F22D988A09D69ull, 0x2BA44C9975CA93A5ull}, "big X", {{10, 2, 46, 51, Square}}},
    // "Press LB + LT to store Your Finisher" (0/6/0) and in Italian (0/5/0).
    {512, 64, {0xACED899F946BB388ull, 0xA7F555667F84D05Eull}, "store finisher prompt",
     {{134, 13, 74, 37, L1}, {232, 0, 42, 64, L2}}},
    {512, 64, {0x79BC1A6B7F463B43ull, 0x940E518DD2608EE7ull}, "store finisher prompt",
     {{123, 13, 74, 37, L1}, {221, 0, 40, 64, L2}}},
    // "Tap LB for Finisher" (0/7/0) and in Italian.
    {512, 32, {0x01EF25DC8D5D8E0Aull, 0x340C1F0D1F89B8DCull}, "finisher prompt",
     {{147, 0, 74, 32, L1}}},
    {512, 32, {0x8DF1AD0E5640345Cull, 0xF87878C88715BF6Aull}, "finisher prompt",
     {{140, 0, 68, 32, L1}}},
    // "Hold LB to ignore the Ropebreak" (0/9/0) and in Italian.
    {512, 32, {0xE63CD87586FE21E9ull, 0x2C9EFD5AC1C3A031ull}, "rope break prompt",
     {{97, 0, 74, 32, L1}}},
    {512, 32, {0xEAEAC915A8C7BC6Bull, 0x4DDA8D194AF7AC17ull}, "rope break prompt",
     {{158, 0, 70, 32, L1}}},
    // "Tap LB to complain to the Referee" (0/10/0) and in Italian.
    {512, 32, {0xE1D77155D1E2722Full, 0x01C24EEE69A4151Full}, "complain prompt",
     {{75, 0, 74, 32, L1}}},
    {512, 32, {0x26F9532ED1BD408Aull, 0x9C99792A80374FE8ull}, "complain prompt",
     {{91, 0, 74, 32, L1}}},
    // "RS click, change, release" (0/19/0) and in Italian.
    {256, 128, {0xB3774CB7D167BE5Eull, 0x2F0C78805001AD68ull}, "right stick prompt",
     {{2, 2, 62, 62, RStick}, {188, 2, 62, 62, RStick}}},
    {256, 128, {0x77E1DE00A1C1071Aull, 0xADE5E1DECF4CE0A8ull}, "right stick prompt",
     {{2, 2, 62, 62, RStick}, {188, 2, 62, 62, RStick}}},
    // "REVERSAL: RT, or LT" (0/20/0; LT lying on its side) and in Italian.
    {128, 64, {0xA6432E40BDC23D5Cull, 0x9CDF38F12ED6B16Eull}, "reversal prompt",
     {{99, 0, 29, 56, R2}, {42, 34, 55, 30, L2}}},
    {128, 64, {0x21DAA132FE190537ull, 0x3095D423C469378Full}, "reversal prompt",
     {{99, 0, 29, 56, R2}, {42, 34, 55, 30, L2}}},
    // "RS LS: push or hold" (7/0/0) and in Italian.
    {128, 128, {0xCAD07E10ED0B41E2ull, 0x3F219A3511297C74ull}, "sticks prompt",
     {{2, 2, 60, 60, RStick}, {66, 2, 60, 60, LStick}}},
    {128, 128, {0xACF66B64E12E1DC2ull, 0x700CA8DC8824E4FCull}, "sticks prompt",
     {{2, 2, 60, 60, RStick}, {66, 2, 60, 60, LStick}}},
    // "RS LS to find Sweet Spot" (7/1/0, drawn stretched: its sticks are stored squeezed) and in
    // Italian.
    {256, 64, {0xA7804AB6414333A7ull, 0x580A38261FE8F947ull}, "sweet spot prompt",
     {{1, 2, 30, 60, RStick, true}, {33, 2, 30, 60, LStick, true}}},
    {256, 64, {0x429611D68C8266C9ull, 0x59D81469CF10CF05ull}, "sweet spot prompt",
     {{1, 2, 30, 60, RStick, true}, {33, 2, 30, 60, LStick, true}}},
    // "Hold BACK button to regain STAMINA" (0/8/2) and in Italian: a SELECT button over the word.
    {256, 64, {0x4923B6F0DCEDFD5Bull, 0x6F19E6997A46FEFBull}, "stamina prompt",
     {{46, 1, 66, 31, Select}}},
    {256, 64, {0x2A9305FC4269657Cull, 0xABB460D3387D722Aull}, "stamina prompt",
     {{141, 1, 64, 31, Select}}},
    // "Tap LT or RT to prevent the door closing" (8/0/0) and in Italian.
    {128, 64, {0xDFD2E20C369382E3ull, 0xBC54FE8AD6AA31F3ull}, "door prompt",
     {{44, 0, 22, 31, L2}, {87, 0, 23, 31, R2}}},
    {128, 64, {0x61284CC6B6CE51B0ull, 0xED64B402F5574A8Aull}, "door prompt",
     {{57, 0, 20, 30, L2}, {98, 0, 21, 30, R2}}},
    // Create-a-Superstar's RB (CAS_move01) and X (CAS_rotate02).
    {128, 64, {0x6B4E01C6DE65F694ull, 0x006C4908A90BD892ull}, "CAS RB", {{27, 5, 74, 54, R1}}},
    {64, 64, {0xEFDCBBDA58265B88ull, 0xDC6D02E27B727702ull}, "CAS X", {{4, 0, 56, 64, Square}}},
};
