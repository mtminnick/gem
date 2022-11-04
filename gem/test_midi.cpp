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