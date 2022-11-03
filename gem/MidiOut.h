#pragma once

#include <windows.h>
#include <mmeapi.h>

// todo: make thread safe

class MidiOut
{
private:
	UINT m_device_num = 0;
	HMIDIOUT m_device_handle = 0;	// todo: init to invalid handle
	bool m_is_device_open = false;

	void SendMIDIEvent(BYTE status, BYTE data1, BYTE data2) const;
	unsigned char ClampChannel(int channel) const;
	unsigned char ClampKey(int key) const;
	unsigned char ClampVelocity(int velocity) const;
	unsigned char ClampProgram(int program) const;

public:
	MidiOut();
	~MidiOut();
	MidiOut(MidiOut const& p) = delete;
	MidiOut& operator=(const MidiOut& p) = delete;

	void NoteOn(int channel, int key, int velocity) const;
	void NoteOff(int channel, int key) const;
	void ProgramChange(int channel, int program) const;
};