#include "jalak/state_machine.h"

namespace jalak {

const char* to_string(State s) {
    switch (s) {
        case State::Boot: return "Boot";
        case State::Ready: return "Ready";
        case State::Listening: return "Listening";
        case State::Uploading: return "Uploading";
        case State::Processing: return "Processing";
        case State::Speaking: return "Speaking";
        case State::BookHandoff: return "BookHandoff";
        case State::Stopped: return "Stopped";
        case State::Error: return "Error";
    }
    return "Unknown";
}

StateMachine::StateMachine(StateConfig cfg) : cfg_(cfg) {}

TransitionResult StateMachine::fail(Event, const char* why) const {
    TransitionResult r;
    r.accepted = false;
    r.from = state_;
    r.to = state_;
    r.reason = why;
    return r;
}

TransitionResult StateMachine::handle(Event ev) {
    const State from = state_;

    // Reset is always allowed and returns to a clean Ready state.
    if (ev == Event::Reset) {
        state_ = State::Ready;
        capture_ms_ = 0;
        session_ms_ = 0;
        turns_ = 0;
        return {true, from, state_, "reset"};
    }

    // STOP dominates from any active state.
    if (ev == Event::Stop) {
        if (from == State::Stopped) return fail(ev, "already stopped");
        state_ = State::Stopped;
        capture_ms_ = 0;
        return {true, from, state_, "stop"};
    }

    switch (from) {
        case State::Boot:
            if (ev == Event::BootComplete) {
                state_ = State::Ready;
                return {true, from, state_, "boot complete"};
            }
            break;

        case State::Ready:
            if (ev == Event::StartCapture) {
                if (turns_ >= cfg_.max_turns)
                    return fail(ev, "turn cap reached");
                if (session_ms_ >= cfg_.max_session_ms)
                    return fail(ev, "session cap reached");
                state_ = State::Listening;
                capture_ms_ = 0;
                return {true, from, state_, "capture started"};
            }
            if (ev == Event::HandoffToBook) {
                state_ = State::BookHandoff;
                return {true, from, state_, "handoff"};
            }
            break;

        case State::Listening:
            if (ev == Event::StopCapture) {
                state_ = State::Uploading;
                capture_ms_ = 0;
                return {true, from, state_, "capture stopped"};
            }
            break;

        case State::Uploading:
            if (ev == Event::UploadComplete) {
                state_ = State::Processing;
                return {true, from, state_, "uploaded"};
            }
            if (ev == Event::TurnFailed) {
                state_ = State::Error;
                return {true, from, state_, "upload failed"};
            }
            break;

        case State::Processing:
            if (ev == Event::TurnSucceeded) {
                state_ = State::Speaking;
                ++turns_;
                return {true, from, state_, "reply ready"};
            }
            if (ev == Event::TurnFailed) {
                state_ = State::Error;
                return {true, from, state_, "turn failed"};
            }
            break;

        case State::Speaking:
            if (ev == Event::SpeechFinished) {
                state_ = State::Ready;
                return {true, from, state_, "speech finished"};
            }
            if (ev == Event::HandoffToBook) {
                state_ = State::BookHandoff;
                return {true, from, state_, "handoff after speech"};
            }
            break;

        case State::BookHandoff:
            if (ev == Event::Reset) {
                state_ = State::Ready;
                return {true, from, state_, "reset from handoff"};
            }
            // While handing off to a book, further talk is not solicited.
            return fail(ev, "book handoff is quiet");

        case State::Stopped:
            if (ev == Event::Reset) {
                state_ = State::Ready;
                return {true, from, state_, "reset from stopped"};
            }
            return fail(ev, "stopped");

        case State::Error:
            if (ev == Event::Reset) {
                state_ = State::Ready;
                return {true, from, state_, "reset from error"};
            }
            return fail(ev, "error state");
    }
    return fail(ev, "invalid transition");
}

std::optional<Event> StateMachine::tick(int dt_ms) {
    if (dt_ms < 0) dt_ms = 0;
    // Session clock advances in every non-terminal state except Boot/Stopped.
    if (state_ != State::Boot && state_ != State::Stopped) {
        session_ms_ += dt_ms;
    }
    if (state_ == State::Listening) {
        capture_ms_ += dt_ms;
        if (capture_ms_ >= cfg_.max_capture_ms) {
            // Emulate the 8 s cap as if the button was released.
            handle(Event::StopCapture);
            return Event::StopCapture;
        }
    }
    if (state_ != State::Boot && state_ != State::Stopped &&
        session_ms_ >= cfg_.max_session_ms) {
        handle(Event::Stop);
        return Event::Stop;
    }
    return std::nullopt;
}

bool StateMachine::mic_muted() const {
    switch (state_) {
        case State::Listening:
            return false;  // only state where capture is permitted
        default:
            return true;
    }
}

Expression StateMachine::current_expression() {
    switch (state_) {
        case State::Boot: return expression_for(Mood::Sleepy);
        case State::Ready: return expression_for(Mood::Neutral);
        case State::Listening: return expression_for(Mood::Listening);
        case State::Uploading: return expression_for(Mood::Thinking);
        case State::Processing: return expression_for(Mood::Thinking);
        case State::Speaking: return expression_for(Mood::Speaking);
        case State::BookHandoff: return expression_for(Mood::Happy);
        case State::Stopped: return expression_for(Mood::Sleepy);
        case State::Error: return expression_for(Mood::Error);
    }
    return expression_for(Mood::Neutral);
}

Gesture classify_press(int duration_ms, int tap_max_ms, int long_press_min_ms) {
    if (duration_ms < 0) return Gesture::None;
    if (duration_ms <= tap_max_ms) return Gesture::Tap;
    if (duration_ms >= long_press_min_ms) return Gesture::LongPress;
    return Gesture::None;  // a "hold" that is neither tap nor long press
}

}  // namespace jalak
