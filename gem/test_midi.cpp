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

using std::cout;
using std::endl;
using std::cin;
using std::this_thread::sleep_for;
using std::chrono::seconds;
using std::chrono::milliseconds;

void test_note_on_off(MidiOut const& mout)
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

void test_channels(MidiOut const& mout)
{
    cout << "Starting MIDI out test" << endl;

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

void test_program_change(MidiOut const& mout)
{
    cout << "Starting MIDI out test" << endl;

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
        sleep_for(seconds(1));

        // Send note-off
        mout.NoteOff(channel, key);
    }

    cout << "Ending of MIDI out test" << endl;
}

void test_polyphony(MidiOut const& mout)
{
    cout << "Starting MIDI out test" << endl;

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
    // (needed for a future voice allocation algo)
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

    cout << "Ending of MIDI out test" << endl;
}

void test_parameters(MidiOut const& mout)
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