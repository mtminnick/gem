#pragma once
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

#include <windows.h>
#include <mmeapi.h>

class MidiOut
{
private:
	HMIDIOUT m_device_handle{ NULL };
	// This Running Status is why functions must take MidiOut as non-const.
	unsigned char m_last_status{ 0 };

	void SendMIDIEvent(BYTE status, BYTE data1, BYTE data2);
	unsigned char ClampChannel(int channel) const;
	unsigned char ClampKey(int key) const;
	unsigned char ClampVelocity(int velocity) const;
	unsigned char ClampProgram(int program) const;
	unsigned char ClampController(int val) const;
	void ControlChange(unsigned char channel, unsigned char control, unsigned char value);

public:
	MidiOut();
	~MidiOut();
	MidiOut(MidiOut const& p) = delete;
	MidiOut& operator=(const MidiOut& p) = delete;

	static void ShowInfo();
	void NoteOn(int channel, int key, int velocity);
	void NoteOff(int channel, int key);
	void ProgramChange(int channel, int program);
	void PanControlChange(int channel, int pan);
	void ModWheelControlChange(int channel, int mod);
};