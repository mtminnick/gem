/*
 * Copyright (c) 2022, Michael Minnick
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */
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
    //test_voice_alloc(midi_out);

    Gesture rhythm = make_gesture(1000, -500, 500);
    Gesture pitch = make_gesture(c4, b4, a4);
    ParamBlock pb = make_param_block(10000, rhythm, pitch);
    Voice v = make_voice(acoustic_piano, pb);
    Piece p = make_piece(v);

    Scheduler s;
    ret = s.Play(midi_out, p);

    return ret;
}
