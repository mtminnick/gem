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
#include <cstdlib>
#include "MidiOut.h"
#include "test_midi.h"
#include "test_gesture.h"
#include "piece.h"

using std::cout;
using std::cerr;

void tests(MidiOut &midi_out)
{
    // Note: tests are interactive on the command line.

    test_note_on_off(midi_out);
    test_channels(midi_out);
    test_program_change(midi_out);
    test_polyphony(midi_out);
    test_parameters(midi_out);  // todo: investigate - makes a cool noise
    test_performance(midi_out);
    test_durations();
    test_velocity(midi_out);
    //test_pan(midi_out);         // generally unreliable
    test_modwheel(midi_out);
    test_percussion(midi_out);
    //test_sustain(midi_out);     // does not work on coolsoft

    test_gesture_wrap();
    test_param_block();
    test_voice_alloc(midi_out);
    test_dictionary();
}

void pieces(MidiOut &midi_out)
{
    piece1(midi_out);
    piece2(midi_out);
    piece3(midi_out);
    piece4(midi_out);
    piece5(midi_out);
}
int main()
{
    int ret{ 0 };
    std::srand(static_cast<unsigned int>(std::time(0)));

    MidiOut::ShowInfo();

    try
    {
        MidiOut midi_out{};

        //tests(midi_out);
        //pieces(midi_out);
    }
    catch (const std::exception&)
    {
        cerr << "Caught exception!\n";
        ret = 1;
    }

    return ret;
}
