#include "MidiOut.h"
#include <iostream>

using std::cout;
using std::endl;
using std::wcout;
using std::wcerr;
using std::hex;
using std::dec;

// Requires:Winmm.lib

// todo: consider using running status - no need to send status byte for sequences of all note-on/off.

unsigned char const kNoteOn = 0x90;
unsigned char const kProgramChange = 0xc0;

MidiOut::MidiOut()
{
    UINT num_devs = midiOutGetNumDevs();
    cout << "Number of MIDI output devices = " << num_devs << endl;

    if (num_devs < 1)
    {
        cout << "No MIDI output devices found" << endl;
        // todo: throw error
        return;
    }

    // For now, use the first output device we find. Later, could ask user to choose.
    UINT dev_num = 0;
    MIDIOUTCAPS moc;
    const UINT smoc = static_cast<UINT>(sizeof(moc));
    MMRESULT mmr = midiOutGetDevCaps(dev_num, &moc, smoc);
    if (mmr != MMSYSERR_NOERROR)
    {
        wchar_t err_text[MAXERRORLENGTH];
        static_cast<void>(midiOutGetErrorText(mmr, err_text, MAXERRORLENGTH));
        wcerr << "Error: midiOutGetDevCaps returns \"" << err_text << "\" (" << mmr << ")" << endl;
        // todo: throw error
        return;
    }

    cout << "MIDI OUT Device " << dev_num << " MIDIOUTCAPS:" << endl;
    cout << "wMid = " << moc.wMid << endl;
    cout << "wPid = " << moc.wPid << endl;
    cout << "vDriverVersion = " << moc.vDriverVersion << endl;
    wcout << "szPname = " << moc.szPname << endl;
    cout << "wTechnology = " << moc.wTechnology << endl;
    cout << "wVoices = " << moc.wVoices << endl;
    cout << "wNotes = " << moc.wNotes << endl;
    cout << "wChannelMask (hex) = " << hex << moc.wChannelMask << dec << endl;
    cout << "dwSupport = " << moc.dwSupport << endl;
    cout << endl;

    HMIDIOUT dev_handle;
    mmr = midiOutOpen(&dev_handle, dev_num, NULL, 0, CALLBACK_NULL);
    if (mmr != MMSYSERR_NOERROR)
    {
        wchar_t err_text[MAXERRORLENGTH];
        static_cast<void>(midiOutGetErrorText(mmr, err_text, MAXERRORLENGTH));
        wcerr << "Error: midiOutOpen returns \"" << err_text << "\" (" << mmr << ")" << endl;
        // todo: throw error
    }
    m_device_handle = dev_handle;
    m_is_device_open = true;
    cout << "MIDI OUT Device opened" << endl;
}

MidiOut::~MidiOut()
{
    // todo: check not needed if constructor throws error
    if (m_is_device_open)
    {
        MMRESULT mmr = midiOutReset(m_device_handle);
        if (mmr != MMSYSERR_NOERROR)
        {
            wchar_t err_text[MAXERRORLENGTH];
            static_cast<void>(midiOutGetErrorText(mmr, err_text, MAXERRORLENGTH));
            wcerr << "Error: midiOutReset returns \"" << err_text << "\" (" << mmr << ")" << endl;
        }
        else
        {
            cout << "MIDI OUT Device reset" << endl;
        }

        mmr = midiOutClose(m_device_handle);
        if (mmr != MMSYSERR_NOERROR)
        {
            wchar_t err_text[MAXERRORLENGTH];
            static_cast<void>(midiOutGetErrorText(mmr, err_text, MAXERRORLENGTH));
            wcerr << "Error: midiOutClose returns \"" << err_text << "\" (" << mmr << ")" << endl;
        }
        else
        {
            m_is_device_open = false;
            cout << "MIDI OUT Device closed" << endl;
        }
    }
}

void MidiOut::SendMIDIEvent(BYTE bStatus, BYTE bData1, BYTE bData2) const
{
    // todo: use variant
    union {
        DWORD dwData;
        BYTE bData[4];
    } u;

    // Construct the MIDI message. 

    u.bData[0] = bStatus;  // MIDI status byte 
    u.bData[1] = bData1;   // first MIDI data byte 
    u.bData[2] = bData2;   // second MIDI data byte 
    u.bData[3] = 0;

    // Send the message. 
    MMRESULT mmr = midiOutShortMsg(m_device_handle, u.dwData);
    if (mmr != MMSYSERR_NOERROR)
    {
        wchar_t err_text[MAXERRORLENGTH];
        static_cast<void>(midiOutGetErrorText(mmr, err_text, MAXERRORLENGTH));
        wcerr << "Error: midiOutShortMsg returns \"" << err_text << "\" (" << mmr << ")" << endl;
    }
}

// Channel numbers are 1 - 16 (status nibble 0 - 15)
unsigned char MidiOut::ClampChannel(int channel) const
{
    if (channel < 1) {
        wcerr << "Warning: invalid channel number " << channel << endl;
        channel = 1;
    }
    if (channel > 16) {
        wcerr << "Warning: invalid channel number " << channel << endl;
        channel = 16;
    }
    return static_cast<unsigned char>(channel);
}

// Keys are 0 - 127
unsigned char MidiOut::ClampKey(int key) const
{
    if (key < 0) {
        wcerr << "Warning: invalid key number " << key << endl;
        key = 0;
    }
    if (key > 127) {
        wcerr << "Warning: invalid key number " << key << endl;
        key = 127;
    }
    return static_cast<unsigned char>(key);
}

// Velocity is 1 - 127 (0 is note-off)
unsigned char MidiOut::ClampVelocity(int velocity) const
{
    if (velocity < 1) {
        wcerr << "Warning: invalid velocity " << velocity << endl;
        velocity = 0;
    }
    if (velocity > 127) {
        wcerr << "Warning: invalid velocity " << velocity << endl;
        velocity = 127;
    }
    return static_cast<unsigned char>(velocity);
}

unsigned char MidiOut::ClampProgram(int program) const
{
    // Program numbers are 1 - 128 (data value 0 - 127)
    if (program < 1) {
        wcerr << "Warning: invalid program number " << program << endl;
        program = 1;
    }
    if (program > 128) {
        wcerr << "Warning: invalid program number " << program << endl;
        program = 128;
    }
    return static_cast<unsigned char>(program);
}

void MidiOut::NoteOn(int channel, int key, int velocity) const
{
    unsigned char status = kNoteOn + ClampChannel(channel) - 1;
    unsigned char data1 = ClampKey(key);
    unsigned char data2 = ClampVelocity(velocity);
    SendMIDIEvent(status, data1, data2);
}

void MidiOut::NoteOff(int channel, int key) const
{
    // A note-on with velocity zero is treated as a note-off.
    // Helps with running status.
    unsigned char status = kNoteOn + ClampChannel(channel) - 1;
    unsigned char data1 = ClampKey(key);
    SendMIDIEvent(status, data1, 0);
}

void MidiOut::ProgramChange(int channel, int program) const
{
    // Program numbers are 1 - 128 (data1 value 0 - 127)
    unsigned char status = kProgramChange + ClampChannel(channel) - 1;
    unsigned char data1 = ClampProgram(program) - 1;
    SendMIDIEvent(status, data1, 0);
}
