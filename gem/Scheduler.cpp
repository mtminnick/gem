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
using std::endl;
using std::thread;
using std::vector;

// scheduler algo
// start a thread for each voice
// each thread: work through each param block
// for each param block:
// use rhythm gesture to work through pitch gesture, sending note-on (unless negative duration)
// sleep for duration 
// send note-off (if needed)

// foreach voice in p (only do the first one until threads ready)
// foreach paramblock in voice (only do the first one for now
// for all values in rhy gesture (1st entry)
// deal with pitch in pitch gesture (2nd entry)
// Consider adding a modifer gesture for each param gesture, applied first (random, reverse, etc.)

// Possible future voice allocatipn algo.
// If voice has a fixed channel, use it (e.g. chan 10 for percussion).
// Search for an unused channel. If found change the program and use the channel.
// If not found, search for a channel with the same program and use it.
// When not in use (rest), channel should be deallocated.
// Only send program change if needed.

void Scheduler::Play(MidiOut const& midi_out, int voice_num, ParamBlock param_block) const
{
	// Rhythm gesture drives the output.
	// Run through rhythm gesture one time.
	// Other gestures may loop around or not get completely used.

	Gesture rhythm_gesture = param_block.GetRhythmGesture();
	Gesture pitch_gesture = param_block.GetPitchGesture();

	int rhythm_index = 0;	// updated by Next() through reference
	int pitch_index = 0;	// updated by Next() through reference
	int const max_dur = param_block.GetDuration();
	int total_dur = 0;
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
			int pitch = pitch_gesture.Next(pitch_index);
			//cout << dur << "<" << pitch << ">" << endl;
			// todo: get velocity from gesture
			int const velocity = 24;
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

void Scheduler::Play(MidiOut const& midi_out, Voice voice) const
{
	auto param_blocks = voice.GetParamBlocks();
	//int i = 0;
	for (auto pb : param_blocks)
	{
		//cout << "Starting param block " << i++ << endl;
		Play(midi_out, voice.GetVoiceNumber(), pb);
	}
}

int Scheduler::Play(MidiOut const& midi_out, Piece piece) const
{
	cout << "Scheduler: running" << endl;

	AllocateVoices(piece);
	InitializeVoices(midi_out, piece);

	vector<thread> voice_threads{};

	// Start a thread for each voice.
	int i = 0;
	for (auto v : piece)
	{
		cout << "Starting voice " << i++ << endl;

		// Get address of overloaded const member function Play(midi_out, voice)
		void (Scheduler:: *fpv)(MidiOut const&, Voice) const = &Scheduler::Play;

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
	// todo: Possible improvement: if no more channels, go back and find a channel
	// with the same instrument number and use that channel (multi-timbral synth).
	
	int chan = 1;
	int j = 0;
	for (Voice & v : voices)
	{
		if (chan == kPercussionChannel)
		{
			++chan;
		}
		if (chan > kMaxChannelNumber)
		{
			cout << "Warning: out of channels for voice " << j << endl;
		}
		
		cout << "Setting voice " << j << " to chan " << chan << endl;
		v.SetVoiceNumber(chan++);
		++j;
	}

	// Possible future dynamic voice allocation algo.
	// If voice has a fixed channel, use it (e.g. chan 10 for percussion).
	// Search for an unused channel. If found change the program and use the channel.
	// If not found, search for a channel with the same program and use it.
	// When not in use (rest), channel should be deallocated.
	// Only send program change if needed.
}

void Scheduler::InitializeVoices(MidiOut const& midi_out, std::vector<Voice> const & voices) const
{
	// Send a midi program change to each channel.
	for (auto v : voices)
	{
		cout << "Setting program " << v.GetInstrumentNumber() << " on chan " << v.GetVoiceNumber() << endl;
		midi_out.ProgramChange(v.GetVoiceNumber(), v.GetInstrumentNumber());
	}
}
