#include <iostream>

#include "MidiOut.h"
#include "Gesture.h"
#include "Scheduler.h"
#include "test_midi.h"
#include "test_gesture.h"

using std::cout;
using std::endl;

// move to general_midi.h
int const acoustic_piano = 1;
int const c4 = 60;
int const a4 = 69;
int const b4 = 71;

int main()
{
    int ret = 0;

    MidiOut midi_out{};

    //test_polyphony(midi_out);
    //test_gesture_wrap();
    //test_param_block();

    Gesture rhythm = make_gesture(1000, -500, 500);
    Gesture pitch = make_gesture(c4, b4, a4);
    ParamBlock pb = make_param_block(rhythm, pitch);
    Voice v = make_voice(acoustic_piano, pb);
    Piece p = make_piece(v);

    Scheduler s;
    ret = s.Play(midi_out, p);

    return ret;
}
