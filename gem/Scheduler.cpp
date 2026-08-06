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
#define NOMINMAX
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <climits>

#include "Scheduler.h"
#include "generalmidi.h"

using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::abs;
using std::cout;
using std::cerr;
using std::thread;
using std::vector;

static vector<int> MakeChord(int root, int cardinality)
{
	vector<int> pitches;
	const int interval = 2; // todo: get this from a table
	for (int i = 0; i < cardinality; i++)
	{
		pitches.push_back(root + i * interval);
	}
	return pitches;
}

void Scheduler::Play(int channel_num, ParamBlock param_block) const
{
	// Rhythm gesture drives the output.
	// Run through rhythm gesture one time.
	// Other gestures may loop around or not get completely used.

	// todo: apply order and value modulators to rhythm and value gestures
	// todo: start separate threads to play async controllers like pitch wheel and mod wheel. They
	//       will get their own rhythm gestures.

	auto& midi_out = MidiOut::Instance();

	auto rhythm_gesture = param_block.GetRhythmGesture();
	auto pitch_gesture = param_block.GetPitchGesture();
	auto velocity_gesture = param_block.GetVelocityGesture();
	auto instrument_gesture = param_block.GetInstrumentGesture();
	auto chord_gesture = param_block.GetChordGesture();

	// Indicies are updated by Next() through a reference.
	int rhythm_index{ 0 };
	int pitch_index{ 0 };
	int velocity_index{ 0 };
	int instrument_index{ 0 };
	int chord_index{ 0 };

	int max_dur = param_block.GetDuration();
	if (max_dur == 0) {
		max_dur = INT_MAX;
	}
	int total_dur{ 0 };
	int last_instrument{ 0 };

	auto sleep_or_stop = [this](int duration) {
        int remaining = duration;
        while (remaining > 0) {
            int sleep_time = std::min(remaining, 500);
            sleep_for(milliseconds(sleep_time));
            remaining -= sleep_time;
            if (GetStop()) {
                return true;
            }
        }
		return false;
	};
	bool stop = false;

	while (total_dur < max_dur && !stop)
	{
		auto dur = rhythm_gesture.Next(rhythm_index);
		auto absdur = abs(dur);
		if (dur <= 0)
		{
			// Negative value for duration is a rest - no other gestures are consumed.
			//cout << dur << "<rest>\n";
			stop = sleep_or_stop(absdur);
		}
		else
		{
			// Apply the instrument change first, but only if it has changed.
			// Don't send program change on percussion channel.
			if (channel_num != kPercussionChannel)
			{
				auto ins = instrument_gesture.Next(instrument_index);
				if (ins != last_instrument)
				{
					midi_out.ProgramChange(channel_num, ins);
					//cout << "new ins " << ins << "\n";
					last_instrument = ins;
				}
			}

			// Genearate the note-on with velocity
			auto pitch = pitch_gesture.Next(pitch_index);
			auto velocity = velocity_gesture.Next(velocity_index);
			//cout << dur << "<p:" << pitch << ">" << "[v:" << velocity << "]" << "\n";

			auto pitches = MakeChord(pitch, chord_gesture.Next(chord_index));

			for (auto p : pitches)
			{
				midi_out.NoteOn(channel_num, p, velocity);
			}
			stop = sleep_or_stop(dur);
			for (auto p : pitches)
			{
				midi_out.NoteOff(channel_num, p);
			}
		}
		total_dur += absdur;
		//cout << "Total dur = " << total_dur << "\n";
	}

	// Let things settle
	//sleep_for(milliseconds(1000));

	//cout << "Channel " << channel_num << " done\n";
}

// Voice thread.
void Scheduler::Play(Voice voice) const
{
	auto param_blocks = voice.GetParamBlocks();
	[[maybe_unused]] int i{ 0 };
	for (auto pb : param_blocks)
	{
		//cout << "Starting param block " << i++ << "\n";
		Play(voice.GetChannelNumber(), pb);
	}
}

void Scheduler::Play(Piece piece) const
{
	//cout << "Scheduler: running\n";

	AllocateVoices(piece);

	vector<thread> voice_threads{};

	// Start a thread for each voice.
	[[maybe_unused]] int i{ 0 };
	for (auto v : piece)
	{
		if (v.IsMuted()) {
			continue;
		}

		//cout << "Starting voice " << i++ << "\n";

		// Get address of overloaded const member function Play(voice)
		void (Scheduler:: *fpv)(Voice) const = &Scheduler::Play;

		// First arg to thread constructor is pointer to member function, second arg is ref to valid object
		// with this function, rest are args to the function.
		voice_threads.push_back(thread(fpv, std::ref(*this), v));
	}

	for (auto & t : voice_threads)
	{
		t.join();
	}

	cout << "Scheduler: done\n";
}

void Scheduler::AllocateVoices(std::vector<Voice>& voices) const
{
	// Static voice allocation algorithm.
	// For each voice, set channel number to the next available channel number.
    // Skip channel 9 (percussion), must be explicitly assigned to a voice.
	// When out of channels, assign to channel 15.

    int chan{ 1 };	// channel numbers are 1 - 16, 0 means unallocated
	int i{ 0 }; // running count of allocated voices for logging
	for (Voice & v : voices)
	{
		// Don't set chanel if already set.
        if (v.GetChannelNumber() != Voice::kUnallocated) {
            cout << "Voice " << i << " is pre-assigned to channel " << v.GetChannelNumber() << "\n";
            ++i;
            continue;
        }
		// Don't automatically assign the percussion channel.
		if (chan == kPercussionChannel)
		{
			++chan;
		}
		if (chan > kMaxChannelNumber)
		{
			cerr << "Warning: out of channels for voice " << i << ", assigning to channel " << kMaxChannelNumber << "\n";
			v.SetChannelNumber(kMaxChannelNumber);
		}
		else
		{
			cout << "Assigning voice " << i << " to chan " << chan << "\n";
			v.SetChannelNumber(chan++);
		}
		++i;
	}
}
