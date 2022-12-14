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
#include <chrono>
#include <thread>
#include "test_midi.h"
#include "Gesture.h"
#include "Scheduler.h"
#include "generalmidi.h"

using std::cout;
using std::endl;
using std::cin;
using std::this_thread::sleep_for;
using std::chrono::seconds;
using std::chrono::milliseconds;

void test_note_on_off(MidiOut& mout)
{
    cout << "Starting MIDI out test" << endl;

    const int channel = 0x1;
    const int key = 0x3c;
    const int velocity = 0x24;

    // Send note-on
    mout.NoteOn(channel, key, velocity);

    // Wait a bit
    sleep_for(seconds(2));

    // Send note-off
    mout.NoteOff(channel, key);

    cout << "Ending of MIDI out test" << endl;
}

void test_channels(MidiOut& mout)
{
    cout << "Starting MIDI out channels test" << endl;

    const int key = 0x3c;
    const int velocity = 0x24;

    // Channel numbers are 1 - 16
    // Channel 10 is percussion

    for (int i = 1; i < 17; i++)
    {
        cout << "Channel = " << i << endl;

        // Send note-on
        mout.NoteOn(i, key, velocity);

        // Wait a bit
        sleep_for(seconds(1));

        // Send note-off
        mout.NoteOff(i, key);

        //cout << "Enter for next: ";
        //cin.get();
    }

    cout << "Ending of MIDI out test" << endl;
}

void test_program_change(MidiOut& mout)
{
    cout << "Starting MIDI out program test" << endl;

    const int channel = 1;
    const int key = 0x3c;
    const int velocity = 0x24;

    // Channel numbers are 1 - 16
    // Channel 10 is percussion
    // Program numbers are 1 - 128

    int program = 0;
    while (true)
    {
        cout << "Enter program, 0 to quit: ";
        cin >> program;
        if (program < 1 || program > 128)
        {
            break;
        }
        cout << " Program = " << program << endl;

        // Send program-change
        mout.ProgramChange(channel, program);

        // Send note-on
        mout.NoteOn(channel, key, velocity);

        // Wait a bit
        sleep_for(seconds(2));

        // Send note-off
        mout.NoteOff(channel, key);
    }

    cout << "Ending of MIDI out test" << endl;
}

void test_polyphony(MidiOut& mout)
{
    cout << "Starting MIDI out polyphony test" << endl;

    int const chan1 = 1;
    int const chan2 = 2;
    int const prog1 = 53; // Choir Aahs
    int const prog2 = 1;  // Acoustic Grand Piano
    int const velocity = 0x24;
    int const long_key = 60;
    int const short_key1 = 58;
    int const short_key2 = 66;
    int const short_key3 = 71;

    //
    // Test polyphony using two channels
    //

    mout.ProgramChange(chan1, prog1);
    mout.ProgramChange(chan2, prog2);

    // Long note on chan1.
    mout.NoteOn(chan1, long_key, velocity);

    // Three short notes on chan2.
    sleep_for(milliseconds(500));
    mout.NoteOn(chan2, short_key1, velocity);
    sleep_for(milliseconds(200));
    mout.NoteOff(chan2, short_key1);
    mout.NoteOn(chan2, short_key2, velocity);
    sleep_for(milliseconds(200));
    mout.NoteOff(chan2, short_key2);
    mout.NoteOn(chan2, short_key3, velocity);
    sleep_for(milliseconds(200));
    mout.NoteOff(chan2, short_key3);

    // Turn off long note
    sleep_for(milliseconds(1000));
    mout.NoteOff(chan1, long_key);
    sleep_for(milliseconds(1000));

    //
    // Test polyphony on a single channel
    //

    mout.ProgramChange(chan1, prog1);

    // Long note on chan1.
    mout.NoteOn(chan1, long_key, velocity);

    // Three short notes on chan1.
    sleep_for(milliseconds(500));
    mout.NoteOn(chan1, short_key1, velocity);
    sleep_for(milliseconds(200));
    mout.NoteOff(chan1, short_key1);
    mout.NoteOn(chan1, short_key2, velocity);
    sleep_for(milliseconds(200));
    mout.NoteOff(chan1, short_key2);
    mout.NoteOn(chan1, short_key3, velocity);
    sleep_for(milliseconds(200));
    mout.NoteOff(chan1, short_key3);

    // Turn off long note
    sleep_for(milliseconds(1000));
    mout.NoteOff(chan1, long_key);
    sleep_for(milliseconds(1000));

    cout << "Ending of MIDI out test" << endl;
}

void test_parameters(MidiOut& mout)
{
    const int chan = 0;
    const int key = -1;
    const int velocity = 129;
    const int prog = 0;
    mout.ProgramChange(chan, prog);
    mout.NoteOn(chan, key, velocity);
    sleep_for(milliseconds(1000));
    mout.NoteOff(chan, key);
}

// Should sound like a single voice.
void test_performance(MidiOut& mout)
{
	int const pb_total_time = 10000;

	Gesture rhythm = make_gesture(1000, -500, 500);
	Gesture pitch = make_gesture(c4, b4, a4);
	ParamBlock pb = make_param_block(pb_total_time, rhythm, pitch);
	Voice v1 = make_voice(acoustic_grand_piano, pb);

	Voice v2 = make_voice(acoustic_grand_piano, pb);

	Piece p = make_piece(v1, v2);

	Scheduler s;
    s.Play(mout, p);
}

void test_durations()
{
    cout << "W H Q 8th 16th 32nd" << endl;
    cout << nW << " " << nH << " " << nQ << " " << n8 << " " << n16 << " " << n32 << endl;
    cout << "Dots" << endl;
    cout << nWd << " " << nHd << " " << nQd << " " << n8d << " " << n16d << " " << n32d << endl;
    cout << "Triplets" << endl;
    cout << nWt << " " << nHt << " " << nQt << " " << n8t << " " << n16t << " " << n32t << endl;
}

void test_velocity(MidiOut& mout)
{
    const int chan = 1;
    const int prog = 1;
    const int key = a4;
    const int step = 10;
    int velocity = step;
    mout.ProgramChange(chan, prog);

    while (velocity < 127)
    {
        mout.NoteOn(chan, key, velocity);
        cout << velocity << " ";
        sleep_for(milliseconds(1000));
        mout.NoteOff(chan, key);
        sleep_for(milliseconds(1000));
        velocity += step;
    }
    cout << endl;
}

// Pan support depends on instrument in soundfont; generally unreliable.
void test_pan(MidiOut& mout)
{
    const int chan = 1;
    const int prog = flute;
    const int key = c5;
    const int velocity = 24;

    mout.ProgramChange(chan, prog);
    
    // Test pan change before note-on
    int pan = 0;    // full left
    mout.PanControlChange(chan, pan);
    mout.NoteOn(chan, key, velocity);
    sleep_for(milliseconds(2000));
    mout.NoteOff(chan, key);
    sleep_for(milliseconds(2000));

    pan = 127;  // full right
    mout.PanControlChange(chan, pan);
    mout.NoteOn(chan, key, velocity);
    sleep_for(milliseconds(2000));
    mout.NoteOff(chan, key);
    sleep_for(milliseconds(2000));

    pan = 64;   // middle
    mout.PanControlChange(chan, pan);
    mout.NoteOn(chan, key, velocity);
    sleep_for(milliseconds(2000));
    mout.NoteOff(chan, key);
    sleep_for(milliseconds(2000));

    // todo: Test pan change during note-on

}

// Works on some instruments (e.g. violin) and not others (e.g. flute)
void test_async_controller(MidiOut& mout)
{
    const int chan = 1;
    const int prog = 41; // violin
    const int key = c5;
    const int velocity = 24;

    mout.ProgramChange(chan, prog);

    //// Turn note on
    //mout.NoteOn(chan, key, velocity);
    //sleep_for(milliseconds(4000));

    //// Apply controller change
    //const int mod = 127;
    //cout << "Mod Wheel " << mod << endl;
    //mout.ModWheelControlChange(chan, 127);
    //sleep_for(milliseconds(4000));

    //mout.NoteOff(chan, key);

    // Apply controller change
    const int mod = 127;
    cout << "Mod Wheel " << mod << endl;
    mout.ModWheelControlChange(chan, 127);

    // Turn note on
    mout.NoteOn(chan, key, velocity);
    sleep_for(milliseconds(4000));

    mout.NoteOff(chan, key);
}

void test_percussion(MidiOut& mout)
{
    const int chan = 10;
    const int velocity = 65;

    for (int key = acoustic_bass_drum; key <= open_triangle; key += 10)
    {
        mout.NoteOn(chan, key, velocity);
        sleep_for(milliseconds(1000));
        mout.NoteOff(chan, key);
        sleep_for(milliseconds(500));
    }

    sleep_for(milliseconds(1000));
}