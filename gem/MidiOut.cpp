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
#include <mutex>
#include "MidiOut.h"
#include "generalmidi.h"

using std::cout;
using std::endl;
using std::cerr;
using std::wcout;
using std::wcerr;
using std::hex;
using std::dec;
using std::lock_guard;
using std::mutex;

// Requires:Winmm.lib

std::mutex g_midi_out_mutex{};  // protects device access

MidiOut::MidiOut()
{
    UINT num_devs{ midiOutGetNumDevs() };

    if (num_devs < 1)
    {
        cerr << "No MIDI output devices found\n";
        throw std::runtime_error("");
    }

    // For now, use the first output device we find. Later, could ask user to choose.
    UINT dev_num{ 0 };
    HMIDIOUT dev_handle{};
    MMRESULT mmr = midiOutOpen(&dev_handle, dev_num, NULL, 0, CALLBACK_NULL);
    if (mmr != MMSYSERR_NOERROR)
    {
        wchar_t err_text[MAXERRORLENGTH]{};
        static_cast<void>(midiOutGetErrorText(mmr, err_text, MAXERRORLENGTH));
        wcerr << "Error: midiOutOpen returns \"" << err_text << "\" (" << mmr << ")" << '\n';
        throw std::runtime_error("");
    }
    m_device_handle = dev_handle;
    cout << "MIDI OUT Device opened\n";
}

MidiOut::~MidiOut()
{
    MMRESULT mmr{ midiOutClose(m_device_handle) };
    if (mmr != MMSYSERR_NOERROR)
    {
        wchar_t err_text[MAXERRORLENGTH]{};
        static_cast<void>(midiOutGetErrorText(mmr, err_text, MAXERRORLENGTH));
        wcerr << "Error: midiOutClose returns \"" << err_text << "\" (" << mmr << ")" << '\n';
    }
    else
    {
        cout << "MIDI OUT Device closed\n";
    }
}

void MidiOut::ShowInfo()
{
    UINT num_devs{ midiOutGetNumDevs() };
    cout << "Number of MIDI output devices = " << num_devs << '\n';

    if (num_devs < 1)
    {
        cerr << "No MIDI output devices found\n";
        return;
    }

    // For now, use the first output device we find. Later, could ask user to choose.
    UINT dev_num{ 0 };
    MIDIOUTCAPS moc{};
    const UINT smoc{ static_cast<UINT>(sizeof(moc)) };
    MMRESULT mmr{ midiOutGetDevCaps(dev_num, &moc, smoc) };
    if (mmr != MMSYSERR_NOERROR)
    {
        wchar_t err_text[MAXERRORLENGTH]{};
        static_cast<void>(midiOutGetErrorText(mmr, err_text, MAXERRORLENGTH));
        wcerr << "Error: midiOutGetDevCaps returns \"" << err_text << "\" (" << mmr << ")" << '\n';
        return;
    }

    cout << "MIDI OUT Device " << dev_num << " MIDIOUTCAPS:" << '\n';
    cout << "wMid = " << moc.wMid << '\n';
    cout << "wPid = " << moc.wPid << '\n';
    cout << "vDriverVersion = " << moc.vDriverVersion << '\n';
    wcout << "szPname = " << moc.szPname << '\n';
    cout << "wTechnology = " << moc.wTechnology << '\n';
    cout << "wVoices = " << moc.wVoices << '\n';
    cout << "wNotes = " << moc.wNotes << '\n';
    cout << "wChannelMask (hex) = " << hex << moc.wChannelMask << dec << '\n';
    cout << "dwSupport = " << moc.dwSupport << '\n';
    cout << endl;
}

void MidiOut::SendMIDIEvent(BYTE bStatus, BYTE bData1, BYTE bData2)
{
    // todo: use variant
    union {
        DWORD dwData;
        BYTE bData[4];
    } u{ 0 };

    const lock_guard<mutex> lock(g_midi_out_mutex);

    // Construct the MIDI message.

    // Running Status does not seem to work with Microsoft GS Wavetable Synth.
    // Works with CoolSoft VirtualMidiSynth.
#define USE_RUNNING_STATUS
#ifdef USE_RUNNING_STATUS
    if (bStatus == m_last_status)
    {
        // Running Status.
        u.bData[0] = bData1;    // first MIDI data byte 
        u.bData[1] = bData2;    // second MIDI data byte 
        u.bData[2] = 0;         // not used
        u.bData[3] = 0;         // not used
        //cout << "Running Status (" << hex << static_cast<unsigned int>(bStatus) << dec << ")" << '\n';
    }
    else
    {
        u.bData[0] = bStatus;   // MIDI status byte 
        u.bData[1] = bData1;    // first MIDI data byte 
        u.bData[2] = bData2;    // second MIDI data byte 
        u.bData[3] = 0;         // not used
        m_last_status = bStatus;
        //cout << "New Status (" << hex << static_cast<unsigned int>(bStatus) << dec << ")" << '\n';
    }
#else
    u.bData[0] = bStatus;   // MIDI status byte 
    u.bData[1] = bData1;    // first MIDI data byte 
    u.bData[2] = bData2;    // second MIDI data byte 
    u.bData[3] = 0;         // not used
#endif

    // Send the message.
    MMRESULT mmr{ midiOutShortMsg(m_device_handle, u.dwData) };
    if (mmr != MMSYSERR_NOERROR)
    {
        wchar_t err_text[MAXERRORLENGTH]{};
        static_cast<void>(midiOutGetErrorText(mmr, err_text, MAXERRORLENGTH));
        wcerr << "Error: midiOutShortMsg returns \"" << err_text << "\" (" << mmr << ")" << '\n';
    }
}

// Channel numbers are 1 - 16 (status nibble 0 - 15)
unsigned char MidiOut::ClampChannel(int channel) const
{
    if (channel < 1) {
        wcerr << "Warning: invalid channel number " << channel << '\n';
        channel = 1;
    }
    if (channel > 16) {
        wcerr << "Warning: invalid channel number " << channel << '\n';
        channel = 16;
    }
    return static_cast<unsigned char>(channel);
}

// Keys are 0 - 127
unsigned char MidiOut::ClampKey(int key) const
{
    if (key < 0) {
        wcerr << "Warning: invalid key number " << key << '\n';
        key = 0;
    }
    if (key > 127) {
        wcerr << "Warning: invalid key number " << key << '\n';
        key = 127;
    }
    return static_cast<unsigned char>(key);
}

// Velocity is 1 - 127 (0 is note-off)
unsigned char MidiOut::ClampVelocity(int velocity) const
{
    if (velocity < 1) {
        wcerr << "Warning: invalid velocity " << velocity << '\n';
        velocity = 0;
    }
    if (velocity > 127) {
        wcerr << "Warning: invalid velocity " << velocity << '\n';
        velocity = 127;
    }
    return static_cast<unsigned char>(velocity);
}

unsigned char MidiOut::ClampProgram(int program) const
{
    // Program numbers are 1 - 128 (data value 0 - 127)
    if (program < 1) {
        wcerr << "Warning: invalid program number " << program << '\n';
        program = 1;
    }
    if (program > 128) {
        wcerr << "Warning: invalid program number " << program << '\n';
        program = 128;
    }
    return static_cast<unsigned char>(program);
}

unsigned char MidiOut::ClampController(int val) const
{
    // Controller numbers are 0 - 127
    if (val < 0) {
        wcerr << "Warning: invalid controller value " << val << '\n';
        val = 0;
    }
    if (val > 127) {
        wcerr << "Warning: invalid controller value " << val << '\n';
        val = 127;
    }
    return static_cast<unsigned char>(val);
}

void MidiOut::NoteOn(int channel, int key, int velocity)
{
    unsigned char status{ static_cast<unsigned char>(kNoteOn + ClampChannel(channel) - 1) };
    unsigned char data1{ ClampKey(key) };
    unsigned char data2{ ClampVelocity(velocity) };
    SendMIDIEvent(status, data1, data2);
}

void MidiOut::NoteOff(int channel, int key)
{
    // A note-on with velocity zero is treated as a note-off.
    // Helps with running status.
    unsigned char status{ static_cast<unsigned char>(kNoteOn + ClampChannel(channel) - 1) };
    unsigned char data1{ ClampKey(key) };
    SendMIDIEvent(status, data1, 0);
}

void MidiOut::ProgramChange(int channel, int program)
{
    // Program numbers are 1 - 128 (data1 value 0 - 127)
    unsigned char status{ static_cast<unsigned char>(kProgramChange + ClampChannel(channel) - 1) };
    unsigned char data1{ static_cast<unsigned char>(ClampProgram(program) - 1) };
    SendMIDIEvent(status, data1, 0);
}

void MidiOut::ControlChange(unsigned char channel, unsigned char control, unsigned char value)
{
    unsigned char status{ static_cast<unsigned char>(kControlChange + channel) }; // 0-based channel
    unsigned char data1{ control };
    unsigned char data2{ value };    // clamped value
    SendMIDIEvent(status, data1, data2);
}

void MidiOut::PanControlChange(int channel, int pan)
{
    // todo: support full resolution using control change LSB
    ControlChange(ClampChannel(channel) - 1, kControllerPanMSB, ClampController(pan));
}

void MidiOut::ModWheelControlChange(int channel, int mod)
{
    // todo: support full resolution using control change using LSB
    ControlChange(ClampChannel(channel) - 1, kControllerModWheelMSB, ClampController(mod));
}

void MidiOut::SustainOnControlChange(int channel)
{
    ControlChange(ClampChannel(channel) - 1, kControllerLegatoFootswitch, kControllerOn);
}

void MidiOut::SustainOffControlChange(int channel)
{
    ControlChange(ClampChannel(channel) - 1, kControllerLegatoFootswitch, kControllerOff);
}
