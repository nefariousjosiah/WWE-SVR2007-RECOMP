// Skip the intros (cvar svr_skip_intros): the game starts at the title screen.
//
// SvR 2007's front end is a chain of scenes run by one sequencer object (*0x827A84B8, made by
// sub_821AF570, run every frame by sub_821AF638). The current scene's number is at 0x827A84BC,
// and a table at 0x8274AC00 has 16 bytes per scene: the function that creates it, the next scene
// when it ends with result 1, the next scene with result 0, and a function run on leaving it. A
// scene whose create function gives no scene is passed over at once (the table's scene 8 has
// none). From launch:
//
//   0  start-up                                   sub_821AFA50   sets up the game's managers,
//                                                                  queues /EXH/0000, /ARE/0000 and
//                                                                  more, and waits until the loader
//                                                                  queue is empty and the sound
//                                                                  system is ready
//   2  legal screen (MENU/LEGL or MENU/LGLP)      sub_821AEBF0   loads its pack, fades in, stays
//                                                                  300 frames, fades out
//   3  logos: movies\logo.bik (sound 1000)        sub_821AF060
//   4  opening movie: movies\opening.bik or       sub_821AF2D0   (picked at random; sound 1010 or
//      movies\opening2.bik                                         1011; leaving it, sub_821AF360,
//                                                                  starts a fade-out)
//   5  title screen, "Press START", title.bik     sub_821B1288
//   6  -> 8 after START: what follows it (the profile and save loading) and the main menu.
//      (6 -> 7, the attract-mode demo, when the title screen times out; 7 -> 5.)
//
// The legal screen's 300 frames are a plain wait: its shown state (sub_821AE740) only counts
// frames. Everything the game loads at startup is loaded by scene 0 before the legal screen, and
// the title screen waits for its own loading (and for the loader queue to be empty) before it
// shows. With the skip:
//
//   - the legal screen treats its 300 frames as reached. It still loads its pack, fades in and
//     fades out as usual, so it shows for about half a second.
//   - the sequencer goes from the legal screen straight to the title screen (the next scene
//     number 3 becomes 5), as if the logos and the opening movie had played. Neither movie scene
//     is created, so their movies, sounds and fades never start; the legal screen's own fade-out
//     (in its destructor) is the one the title screen fades in from, the same calls the opening
//     movie makes. Nothing in the two scenes is needed later: they only play and draw a movie.
//
// The title screen stays: the START pressed there is the player's. The game keeps the controller
// that pressed it as player 1, and loads the profile and save from it. Loading and saves are
// untouched.

#include <rex/cvar.h>
#include <rex/logging.h>
#include <rex/ppc.h>

#include <atomic>
#include <cstdint>

REXCVAR_DEFINE_BOOL(svr_skip_intros, false, "Game",
                    "Skip the legal screen's wait, the logo movie and the opening movie at "
                    "startup: the game starts at the title screen (Press START)");

namespace {
constexpr int32_t kLegalFrames = 300;  // sub_821AE740: cmpwi r11,300
constexpr uint32_t kLogoScene = 3;     // scene table 0x8274AC00: legal (2) -> logos (3)
constexpr uint32_t kTitleScene = 5;    // opening movie (4) -> title screen (5)
std::atomic<bool> g_legal_logged{false};
}  // namespace

// The legal screen's shown state (sub_821AE740) at 0x821AE748: r11 = frames shown so far.
void SvrSkipIntrosLegalHook(PPCRegister& r11) {
  if (!REXCVAR_GET(svr_skip_intros) || r11.s32 > kLegalFrames)
    return;
  if (!g_legal_logged.exchange(true))
    REXLOG_INFO("Skip intros: legal screen shortened (frame {} of {}); it fades out now",
                r11.s32, kLegalFrames);
  r11.u64 = kLegalFrames + 1;
}

// The sequencer (sub_821AF638) at 0x821AF6BC, about to store the next scene number: r11.
void SvrSkipIntrosMoviesHook(PPCRegister& r11) {
  if (r11.u32 != kLogoScene || !REXCVAR_GET(svr_skip_intros))
    return;
  REXLOG_INFO("Skip intros: logos (movies\\logo.bik) and opening movie (movies\\opening.bik or "
              "opening2.bik) skipped, on to the title screen");
  r11.u64 = kTitleScene;
}
