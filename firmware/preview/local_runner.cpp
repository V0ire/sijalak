#include "jalak/local/clip.h"
#include "jalak/local/demo.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

// Preview runner: the SAME Demo/Button/Framebuffer logic as firmware. It loads a
// Mochi clip from a local assets folder when one is given (never committed), and
// reports the animation frame and any sound request so the web page can show them.
//
// Usage: jalak_local_preview [assets_frames_dir]
//   e.g. firmware/preview/assets/frames  (produced by tools/mochi_assets.py)

namespace {
bool load_clip(const std::string& dir, const std::string& name, jalak::local::Clip& out) {
    std::ifstream f(dir + "/" + name + ".bin", std::ios::binary);
    if (!f) return false;
    std::vector<std::uint8_t> blob((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    if (blob.empty() || blob.size() % jalak::local::kFrameBytes != 0) return false;
    out.name = name;
    out.frame_delay_ms = 100;
    out.frames = jalak::local::unpack_frames(blob, blob.size() / jalak::local::kFrameBytes);
    return !out.frames.empty();
}

const char* tone_name(jalak::local::Tone t) { return jalak::local::to_string(t); }
}  // namespace

int main(int argc, char** argv) {
    jalak::local::Demo demo;
    jalak::local::Button button;
    jalak::Framebuffer fb({128, 64});
    std::string assets = argc > 1 ? argv[1] : "";
    std::string clip_name;
    if (!assets.empty()) {
        for (const char* name : {"happy", "love", "wink", "sleepy"}) {
            jalak::local::Clip clip;
            if (load_clip(assets, name, clip) && demo.load_face_clip(std::move(clip))) {
                clip_name = name;
                break;
            }
        }
    }
    std::uint64_t now;
    int pressed, reduced;
    while (std::cin >> now >> pressed >> reduced) {
        if ((pressed != 0 && pressed != 1) || (reduced != 0 && reduced != 1)) return 2;
        demo.update(now, button.sample(pressed != 0, now));
        demo.render(fb, reduced != 0);
        const auto sound = demo.take_sound();
        std::cout << "{\"screen\":\"" << (demo.screen() == jalak::local::Screen::Face ? "face" : demo.screen() == jalak::local::Screen::Menu ? "menu" : "timer")
            << "\",\"phase\":\"" << (demo.phase() == jalak::local::Phase::Focus ? "focus" : demo.phase() == jalak::local::Phase::Break ? "break" : "done")
            << "\",\"running\":" << (demo.running() ? "true" : "false")
            << ",\"remaining_ms\":" << demo.remaining_ms() << ",\"selection\":" << demo.selection()
            << ",\"clip\":\"" << clip_name << "\",\"frame\":" << demo.face_frame_index()
            << ",\"sound\":\"" << (!sound.play ? "" : sound.is_pop ? "mochi_pop" : tone_name(sound.tone)) << "\""
            << ",\"pages\":[";
        bool first = true;
        for (const auto byte : fb.to_page_bytes()) {
            if (!first) std::cout << ',';
            first = false; std::cout << static_cast<unsigned>(byte);
        }
        std::cout << "]}" << std::endl;
    }
    return std::cin.eof() ? 0 : 2;
}
