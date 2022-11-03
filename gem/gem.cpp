#include <iostream>

#include "MidiOut.h"
#include "Gesture.h"
#include "Scheduler.h"
#include "test_midi.h"

using std::cout;
using std::endl;

// move to general_midi.h
int const c4 = 60;
int const b4 = 71;

int main()
{
    MidiOut midi_out{};

    test_polyphony(midi_out);

    Gesture rhythm_gesture = make_gesture(1000, 500);   // durations in milliseconds, negative is rest
    Gesture pitch_gesture = make_gesture(c4, b4);
    // todo: add instrument_gesture (one element)

    ParamBlock pb1 = make_param_block(rhythm_gesture, pitch_gesture);
    ParamBlock pb2 = make_param_block(rhythm_gesture, pitch_gesture);

    Voice v1 = make_voice(pb1, pb2);
    Voice v2 = make_voice(pb1, pb2);

    Piece p = make_piece(v1, v2);

    // Scheduler should take a const& to midi out?
    Scheduler s;
    int ret = s.Play(p);

    return ret;
}
