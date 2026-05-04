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

void tests(MidiOut &)
{
    // Note: tests are interactive on the command line.

    test_note_on_off();
    test_channels();
    test_program_change();
    test_polyphony();
    test_parameters();  // todo: investigate - makes a cool noise
    test_performance();
    test_durations();
    test_velocity();
    //test_pan();         // generally unreliable
    test_modwheel();
    test_percussion();
    //test_sustain();     // does not work on coolsoft

    test_gesture_wrap();
    test_param_block();
    test_voice_alloc();
    test_dictionary();
}

void pieces(MidiOut &)
{
    piece1();
    piece2();
    piece3();
    piece4();
    piece5();
    piece6();
}
int main()
{
    int ret{ 0 };
    std::srand(static_cast<unsigned int>(std::time(0)));

    MidiOut::ShowInfo();

    try
    {
        MidiOut::Create();

        interactive_piece();
        //test_note_on_off();

        //tests();
        //pieces();

        MidiOut::Destroy();
    }
    catch (const std::exception&)
    {
        cerr << "Caught exception!\n";
        ret = 1;
    }

    return ret;
}
