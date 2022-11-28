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
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include "Scheduler.h"
#include "generalmidi.h"

using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::abs;
using std::cout;
using std::cerr;
using std::endl;
using std::thread;
using std::vector;

// Voice thread.
void Scheduler::Play(MidiOut& midi_out, int voice_num, ParamBlock param_block) const
{
	// Rhythm gesture drives the output.
	// Run through rhythm gesture one time.
	// Other gestures may loop around or not get completely used.

	// todo: apply order and value modulators to rhythm and value gestures
	// todo: start separate threads to play async controllers like pitch wheel and mod wheel. They
	//       will get their own rhythm gestures.

	Gesture rhythm_gesture = param_block.GetRhythmGesture();
	Gesture pitch_gesture = param_block.GetPitchGesture();
	Gesture velocity_gesture = param_block.GetVelocityGesture();
	Gesture instrument_gesture = param_block.GetInstrumentGesture();

	// Indicies are updated by Next() through a reference.
	int rhythm_index = 0;
	int pitch_index = 0;
	int velocity_index = 0;
	int instrument_index = 0;

	int const max_dur = param_block.GetDuration();
	int total_dur = 0;
	int last_instrument = 0;

	while (total_dur < max_dur)
	{
		int dur = rhythm_gesture.Next(rhythm_index);
		int absdur = abs(dur);
		if (dur <= 0)
		{
			// Negative value for duration is a rest - no other gestures are consumed.
			//cout << dur << "<rest>" << endl;
			sleep_for(milliseconds(absdur));
		}
		else
		{
			// Apply the instrument change first, but only if it has changed.
			int ins = instrument_gesture.Next(instrument_index);
			if (ins != last_instrument)
			{
				midi_out.ProgramChange(voice_num, ins);
				//cout << "new ins " << ins << endl;
				last_instrument = ins;
			}

			// Genearate the note-on with velocity
			int pitch = pitch_gesture.Next(pitch_index);
			int velocity = velocity_gesture.Next(velocity_index);
			//cout << dur << "<p:" << pitch << ">" << "[v:" << velocity << "]" << endl;

			midi_out.NoteOn(voice_num, pitch, velocity);
			sleep_for(milliseconds(dur));
			midi_out.NoteOff(voice_num, pitch);
		}
		total_dur += absdur;
		//cout << "Total dur = " << total_dur << endl;
	}

	// Let things settle
	sleep_for(milliseconds(1000));
}

void Scheduler::Play(MidiOut& midi_out, Voice voice) const
{
	auto param_blocks = voice.GetParamBlocks();
	//int i = 0;
	for (auto pb : param_blocks)
	{
		//cout << "Starting param block " << i++ << endl;
		Play(midi_out, voice.GetVoiceNumber(), pb);
	}
}

int Scheduler::Play(MidiOut& midi_out, Piece piece) const
{
	cout << "Scheduler: running" << endl;

	AllocateVoices(piece);

	vector<thread> voice_threads{};

	// Start a thread for each voice.
	int i = 0;
	for (auto v : piece)
	{
		cout << "Starting voice " << i++ << endl;

		// Get address of overloaded const member function Play(midi_out, voice)
		void (Scheduler:: *fpv)(MidiOut&, Voice) const = &Scheduler::Play;

		// First arg to thread constructor is pointer to member function, second arg is ref to valid object
		// with this function, rest are args to the function.
		voice_threads.push_back(thread(fpv, std::ref(*this), std::ref(midi_out), v));
	}

	for (auto & t : voice_threads)
	{
		t.join();
	}

	cout << "Scheduler: done" << endl;
    return 0;
}

void Scheduler::AllocateVoices(std::vector<Voice>& voices) const
{
	// Static voice allocation algorithm.
	// For each voice, set channel number to the next available channel number.
	// Skip channel 10 (percussion).
	// Stop allocating when run out of channels.

	int chan = 1;
	int i = 0; // running count of allocated voices for logging
	for (Voice & v : voices)
	{
		// Don't automatically assign the percussion channel.
		if (chan == kPercussionChannel)
		{
			++chan;
		}
		if (chan > kMaxChannelNumber)
		{
			cerr << "Warning: out of channels for voice " << i << endl;
		}
		else
		{
			cout << "Setting voice " << i << " to chan " << chan << endl;
			v.SetVoiceNumberOnce(chan++);
		}
		++i;
	}
}
