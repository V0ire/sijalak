#pragma once
// Interaction state machine + gesture/stimulus logic. Portable and testable.

#include <cstdint>
#include <functional>
#include <optional>

#include "jalak/expression.h"

namespace jalak {

enum class State {
    Boot,
    Ready,
    Listening,   // push-to-talk active, microphone capturing
    Uploading,
    Processing,
    Speaking,
    BookHandoff, // attention handed to a physical book; robot goes quiet
    Stopped,
    Error,
};

const char* to_string(State s);

// Inputs the app can deliver. No hardware assumptions: the caller decides how
// a press/gesture is detected.
enum class Event {
    BootComplete,
    StartCapture,       // e.g. PTT pressed
    StopCapture,        // e.g. PTT released, or 8 s cap reached
    UploadComplete,
    TurnSucceeded,
    TurnFailed,
    SpeechFinished,
    HandoffToBook,
    Stop,               // explicit STOP
    Reset,
};

struct StateConfig {
    int max_capture_ms = 8000;    // canonical input cap
    int max_session_ms = 180000;  // 180 s
    int max_turns = 4;
};

struct TransitionResult {
    bool accepted = false;
    State from = State::Boot;
    State to = State::Boot;
    const char* reason = "";
};

// Pure state machine. Invalid transitions are rejected without changing state,
// which is what the firmware fault tests rely on.
class StateMachine {
public:
    explicit StateMachine(StateConfig cfg = {});

    State state() const { return state_; }
    const StateConfig& config() const { return cfg_; }

    // Elapsed ms within the current capture, if listening.
    int capture_elapsed_ms() const { return capture_ms_; }
    int turn_count() const { return turns_; }
    int session_elapsed_ms() const { return session_ms_; }

    TransitionResult handle(Event ev);

    // Advance wall-clock by dt_ms. Enforces the 8 s capture cap and session
    // cap without needing a real timer. Returns the effective event emitted,
    // if any (e.g. StopCapture on timeout).
    std::optional<Event> tick(int dt_ms);

    // Whether the microphone must be muted (half-duplex rule): true whenever
    // we are speaking, stopped, in error, or handling a book.
    bool mic_muted() const;

    // Expression the UI should show for the current state.
    Expression current_expression();

private:
    TransitionResult fail(Event ev, const char* why) const;

    StateConfig cfg_;
    State state_ = State::Boot;
    int capture_ms_ = 0;
    int session_ms_ = 0;
    int turns_ = 0;
};

// Gesture logic: a tiny amount of deterministic interpretation on top of raw
// events, so tests can assert behavior rather than driver details.
enum class Gesture { None, Tap, DoubleTap, LongPress };

// Classify a press duration. Thresholds are configurable; defaults are chosen
// conservatively and are unit-tested.
Gesture classify_press(int duration_ms,
                       int tap_max_ms = 300,
                       int long_press_min_ms = 900);

}  // namespace jalak
