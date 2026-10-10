#include "jalak/oled/adapter.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace jalak;
using namespace jalak::oled;
namespace {
Snapshot sample(State state, std::uint8_t level = 0) {
    // Obtain state expressions from the existing controller, not a preview-only
    // expression/art implementation. Child rendering still has no capture path.
    StateMachine machine;
    if (state != State::Boot) machine.handle(Event::BootComplete);
    if (state == State::Listening || state == State::Uploading || state == State::Processing ||
        state == State::Speaking || state == State::Error) machine.handle(Event::StartCapture);
    if (state == State::Uploading || state == State::Processing || state == State::Speaking ||
        state == State::Error) machine.handle(Event::StopCapture);
    if (state == State::Processing || state == State::Speaking) machine.handle(Event::UploadComplete);
    if (state == State::Speaking) machine.handle(Event::TurnSucceeded);
    if (state == State::Error) machine.handle(Event::TurnFailed);
    if (state == State::Stopped) machine.handle(Event::Stop);
    if (state == State::BookHandoff) machine.handle(Event::HandoffToBook);
    return snapshot_from(machine, {state == State::Listening, state == State::Speaking, level});
}

struct Moment { State state; std::uint64_t time; std::uint8_t level; };
struct Clip {
    const char* name;
    const char* label;
    Profile profile;
    std::vector<Moment> moments;
    unsigned still = 0;
};

std::uint8_t envelope(unsigned frame) {
    // Synthetic TX envelope test fixture, not recorded audio or lip-sync evidence.
    constexpr std::uint8_t levels[]{0,20,45,75,100,60,30,0,0,40,70,20};
    return levels[frame % 12u];
}

void section(Clip& clip, State state, unsigned count) {
    for (unsigned i = 0; i < count; ++i)
        clip.moments.push_back({state, clip.moments.size() * 100u,
                               state == State::Speaking ? envelope(i) : std::uint8_t{0}});
}

std::vector<Clip> clips() {
    std::vector<Clip> result;
    const State states[]{State::Boot, State::Ready, State::Listening, State::Uploading,
        State::Processing, State::Speaking, State::Stopped, State::Error, State::BookHandoff};
    const char* names[]{"boot","ready","listening","uploading","processing","speaking",
        "stopped","error","book-handoff"};
    const char* labels[]{"BOOT / MULAI","READY / SIAP","LISTENING / REKAM","UPLOADING / KIRIM",
        "PROCESSING / PROSES","SPEAKING / SUARA","STOPPED / STOP","ERROR / GALAT","BOOK_HANDOFF / BUKU"};
    for (unsigned i = 0; i < 9; ++i) {
        result.push_back({names[i], labels[i], Profile::AdultLab, {}, i == 5 ? 4u : 7u});
        section(result.back(), states[i], i == 1 ? 50 : 30);
    }
    const State local[]{State::Boot, State::Ready, State::Speaking, State::Stopped, State::Error, State::BookHandoff};
    const char* local_names[]{"local-boot","local-ready","local-speaking","local-stopped","local-error","local-book"};
    const char* local_labels[]{"LOCAL / MULAI","LOCAL / SIAP","LOCAL / SUARA","LOCAL / STOP","LOCAL / GALAT","LOCAL / BUKU"};
    for (unsigned i = 0; i < 6; ++i) {
        result.push_back({local_names[i], local_labels[i], Profile::ChildLocal, {}, i == 2 ? 4u : 7u});
        section(result.back(), local[i], i == 1 ? 50 : 30);
    }
    result.push_back({"local-denied", "LOCAL / capture request rejected", Profile::ChildLocal, {}});
    section(result.back(), State::Listening, 20);
    result.push_back({"adult-flow", "Adult state sequence (host fixture)", Profile::AdultLab, {}});
    for (const auto state : {State::Boot, State::Ready, State::Listening, State::Uploading,
                            State::Processing, State::Speaking, State::BookHandoff})
        section(result.back(), state, state == State::Ready ? 50 : 20);
    result.push_back({"local-flow", "Local state sequence (host fixture)", Profile::ChildLocal, {}});
    for (const auto state : {State::Boot, State::Ready, State::Speaking, State::BookHandoff})
        section(result.back(), state, 20);
    result.push_back({"stop-interrupt", "STOP replaces speaking without transition", Profile::AdultLab, {}, 10});
    section(result.back(), State::Speaking, 10);
    section(result.back(), State::Stopped, 20);
    return result;
}
}  // namespace

int main(int argc, char** argv) {
    try {
        if (argc != 2) { std::cerr << "usage: oled_render OUTPUT_DIRECTORY\n"; return 2; }
        const std::filesystem::path output(argv[1]);
        std::filesystem::create_directories(output);
        std::ofstream raw(output / "frames.raw", std::ios::binary);
        std::ofstream manifest(output / "frames.json");
        if (!raw || !manifest) throw std::runtime_error("cannot open preview outputs");
        manifest << "{\n\"width\":128,\"height\":64,\"format\":\"page-lsb-top\","
                    "\"frame_bytes\":1024,\"duration_ms\":100,\"clips\":[\n";
        std::size_t offset = 0;
        unsigned clip_count = 0;
        for (const auto& clip : clips()) {
            Adapter adapter({clip.profile, 10, Motion::Subtle});
            if (clip_count++ != 0) manifest << ",\n";
            manifest << "{\"name\":\"" << clip.name << "\",\"label\":\"" << clip.label
                     << "\",\"profile\":\"" << (clip.profile == Profile::AdultLab ? "adult-lab" : "child-local")
                     << "\",\"offset\":" << offset << ",\"count\":" << clip.moments.size()
                     << ",\"still\":" << clip.still << ",\"visible_states\":[";
            unsigned i = 0;
            for (const auto& moment : clip.moments) {
                adapter.update(sample(moment.state, moment.level), moment.time);
                const auto& bytes = adapter.page_bytes();
                raw.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
                offset += bytes.size();
                if (i++ != 0) manifest << ',';
                manifest << '"' << to_string(adapter.info().visible_state) << '"';
            }
            manifest << "]}";
        }
        manifest << "\n]}\n";
        raw.close(); manifest.close();
        if (!raw || !manifest) throw std::runtime_error("preview write failed");

        // Host rendering+packing benchmark; no I2C, audio tasks, or panel involved.
        Adapter measured({Profile::AdultLab, 10});
        const auto snapshot = sample(State::Speaking, 75);
        constexpr unsigned iterations = 10000;
        const auto start = std::chrono::steady_clock::now();
        for (unsigned i = 0; i < iterations; ++i) measured.update(snapshot, i * UINT64_C(100));
        const auto micros = std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - start).count();
        std::ofstream metrics(output / "host-metrics.json");
        metrics << "{\"scope\":\"host-only rendering plus page packing, no panel\","
                << "\"iterations\":" << iterations << ",\"total_us\":" << micros
                << ",\"mean_us\":" << static_cast<double>(micros) / iterations
                << ",\"adapter_object_bytes\":" << sizeof(Adapter)
                << ",\"framebuffer_payload_bytes\":8192,\"page_payload_bytes\":1024}\n";
        metrics.close();
        if (!metrics) throw std::runtime_error("metric write failed");
        std::cout << "Rendered " << offset / kPageBytes << " frames in " << clip_count
                  << " clips, " << offset << " bytes. Host render+pack mean "
                  << static_cast<double>(micros) / iterations << " us/frame.\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n'; return 1;
    }
}
