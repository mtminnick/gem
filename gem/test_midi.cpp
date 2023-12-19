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
#include <string>
#include <cassert>
#include "test_midi.h"
#include "Gesture.h"
#include "Scheduler.h"
#include "generalmidi.h"
#include "Test.h"

using std::cout;
using std::endl;
using std::flush;
using std::cin;
using std::this_thread::sleep_for;
using std::chrono::seconds;
using std::chrono::milliseconds;
using std::string;
using std::vector;

void test_note_on_off(MidiOut& mout)
{
    Test::Enter(__func__, "Should hear a single note.");

    const int channel = 0x1;
    const int key = 0x3c;
    const int velocity = 0x24;

    // Send note-on
    mout.NoteOn(channel, key, velocity);

    // Wait a bit
    sleep_for(seconds(2));

    // Send note-off
    mout.NoteOff(channel, key);

    Test::Exit();
}

void test_channels(MidiOut& mout)
{
    Test::Enter(__func__, "Should hear the same note on each channel (channel 10 is Percussion).");

    const int key = 0x3c;
    const int velocity = 0x24;

    // Channel numbers are 1 - 16
    // Channel 10 is percussion

    for (int i = 1; i < 17; i++)
    {
        cout << "Channel = " << i << '\n';

        // Send note-on
        mout.NoteOn(i, key, velocity);

        // Wait a bit
        sleep_for(seconds(1));

        // Send note-off
        mout.NoteOff(i, key);

        //cout << "Enter for next: " << flush;
        //cin.get();
    }

    Test::Exit();
}

void test_program_change(MidiOut& mout)
{
    Test::Enter(__func__, "Should hear a single note using the selected program (instrument).");

    const int channel = 1;
    const int key = 0x3c;
    const int velocity = 0x24;

    // Channel numbers are 1 - 16
    // Channel 10 is percussion
    // Program numbers are 1 - 128

    int program{ 0 };
    while (true)
    {
        cout << "Enter program 1 through 128, 0 to quit: ";
        cin >> program;
        if (program < 1 || program > 128)
        {
            break;
        }
        cout << " Program = " << program << '\n';

        // Send program-change
        mout.ProgramChange(channel, program);

        // Send note-on
        mout.NoteOn(channel, key, velocity);

        // Wait a bit
        sleep_for(seconds(2));

        // Send note-off
        mout.NoteOff(channel, key);
    }

    Test::Exit();
}

void test_polyphony(MidiOut& mout)
{
    Test::Enter(__func__, "Should hear a long sustained note with 3 short notes on top; second time with same instrument on top.");

    int const chan1 = 1;
    int const chan2 = 2;
    int const prog1 = 53; // Choir Aahs
    int const prog2 = 1;  // Acoustic Grand Piano
    int const velocity = 0x45;
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

    Test::Exit();
}

void test_parameters(MidiOut& mout)
{
    Test::Enter(__func__, "Should see parameter range warnings.");

    const int chan = 0;
    const int key = -1;
    const int velocity = 129;
    const int prog = 0;
    mout.ProgramChange(chan, prog);
    mout.NoteOn(chan, key, velocity);
    sleep_for(milliseconds(1000));
    mout.NoteOff(chan, key);

    Test::Exit();
}

void test_performance(MidiOut& mout)
{
    Test::Enter(__func__, "Two voices starting at the same time should sound like a single voice.");
    
    int const pb_total_time = 10000;

	Gesture rhythm = make_gesture(1000, -500, 500);
	Gesture pitch = make_gesture(c4, b4, a4);
	ParamBlock pb = make_param_block(pb_total_time, rhythm, pitch);
	Voice v1 = make_voice(pb);
	Voice v2 = make_voice(pb);

	Piece p = make_piece(v1, v2);

	Scheduler s;
    s.Play(mout, p);

    Test::Exit();
}

void test_durations()
{
    Test::Enter(__func__, "Displays durations in milliseconds.");

    cout << "W H Q 8th 16th 32nd\n";
    cout << nW << " " << nH << " " << nQ << " " << n8 << " " << n16 << " " << n32 << '\n';
    cout << "Dots\n";
    cout << nWd << " " << nHd << " " << nQd << " " << n8d << " " << n16d << " " << n32d << '\n';
    cout << "Triplets\n";
    cout << nWt << " " << nHt << " " << nQt << " " << n8t << " " << n16t << " " << n32t << '\n';

    Test::Exit();
}

void test_velocity(MidiOut& mout)
{
    Test::Enter(__func__, "Should hear velocity increasing in steps.");

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

    Test::Exit();
}

// Pan support depends on instrument in soundfont; generally unreliable.
void test_pan(MidiOut& mout)
{
    Test::Enter(__func__, "Should hear at least minimal panning.");

    const int chan = 1;
    const int prog = flute;
    const int key = c5;
    const int velocity = 24;

    mout.ProgramChange(chan, prog);
    
    // Test pan change before note-on
    int pan{ 0 };    // full left
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

    Test::Exit();
}

// Works on some instruments (e.g. violin) and not others (e.g. flute)
void test_modwheel(MidiOut& mout)
{
    Test::Enter(__func__, "Should hear violin vibrato during a note on and then applied before a note on.");

    const int chan = 1;
    const int prog = 41; // violin
    const int key = c5;
    const int velocity = 24;
    const int mod = 127;

    mout.ProgramChange(chan, prog);

    // Apply controller change during note on.

    mout.NoteOn(chan, key, velocity);
    sleep_for(milliseconds(4000));

    // Apply controller change
    cout << "Mod Wheel " << mod << '\n';
    mout.ModWheelControlChange(chan, mod);
    sleep_for(milliseconds(4000));

    mout.NoteOff(chan, key);

    sleep_for(milliseconds(1000));

    // Apply controller change before note on.

    cout << "Mod Wheel " << mod << '\n';
    mout.ModWheelControlChange(chan, mod);

    // Turn note on
    mout.NoteOn(chan, key, velocity);
    sleep_for(milliseconds(4000));
    mout.NoteOff(chan, key);

    Test::Exit();
}

void test_percussion(MidiOut& mout)
{
    Test::Enter(__func__, "Should hear a note for every instrument on the percussion channel.");

    const int chan = 10;
    const int velocity = 65;

    cout << "keys";

    for (int key = acoustic_bass_drum; key <= open_triangle; key++)
    {
        cout << " " << key;
        mout.NoteOn(chan, key, velocity);
        sleep_for(milliseconds(1000));
        mout.NoteOff(chan, key);
        sleep_for(milliseconds(500));
    }

    cout << endl;
    sleep_for(milliseconds(1000));

    Test::Exit();
}

// Does not work with coolsoft virtual midi synth, and not required by GM
void test_sustain(MidiOut& mout)
{
    Test::Enter(__func__, "Should hear notes sustained after notes off.");

    const int chan = 1;
    const int prog = 1; // piano
    const int key = c5;
    const int velocity = 24;

    mout.ProgramChange(chan, prog);

    // Enable sustain
    mout.SustainOnControlChange(chan);
    cout << "Sustain On\n";

    mout.NoteOn(chan, key, velocity);

    sleep_for(milliseconds(1000));

    mout.NoteOff(chan, key);
    cout << "Notes off, should be sustained\n";

    sleep_for(milliseconds(4000));

    // Disable sustain
    mout.SustainOffControlChange(chan);
    cout << "Sustain Off\n";

    Test::Exit();
}

// Clever, but doesn't really have a slur effect because the note re-articulates.
// Has a "chording" effect, but only between adjacent notes.
void test_articulation(MidiOut& mout)
{
    Test::Enter(__func__, "Should hear normal articulation first, then legato.");

    const int chan = 1;
    const int prog = 57;// 41; // violin
    const int velocity = 24;
    const int num_keys = 4;
    const int keys[]{ 69, 71, 73, 75 };
    const int duration = 1000;
    assert(duration > 0);

    mout.ProgramChange(chan, prog);

    // Play with normal articulation
    for (int i = 0; i < num_keys; i++)
    {
        cout << "Note on: " << keys[i] << '\n';
        mout.NoteOn(chan, keys[i], velocity);
        sleep_for(milliseconds(duration));
        cout << "Note off: " << keys[i] << '\n';
        mout.NoteOff(chan, keys[i]);
    }

    Test::Pause();

    const int delay = 200;
    assert(delay > 0);
    assert(delay < duration);

    // Play with legato articulation.
    for (int i = 0; i < num_keys; i++)
    {
        // Start the current note.
        cout << "Note on: " << keys[i] << '\n';
        mout.NoteOn(chan, keys[i], velocity);

        // Sleep until the previous note should end
        // (this is the overlap), unless this is the first note.
        if (i > 0)
        {
            sleep_for(milliseconds(delay));

            // End the previous note.
            cout << "Note off: " << keys[i-1] << '\n';
            mout.NoteOff(chan, keys[i-1]);

            // Sleep for the rest of the current note's duration.
            sleep_for(milliseconds(duration - delay));
        }
        else
        {
            // This is the first note, so sleep for the entire duration.
            sleep_for(milliseconds(duration));
        }
    }

    // End the last note.
    if constexpr(num_keys > 0)
    {
        cout << "Note off: " << keys[num_keys - 1] << '\n';
        mout.NoteOff(chan, keys[num_keys - 1]);
    }

    Test::Exit();
}