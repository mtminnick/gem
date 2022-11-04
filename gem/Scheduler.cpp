#include <iostream>
#include "Scheduler.h"

using std::cout;
using std::endl;

// Play(piece) - send prog change for each voice, start thread for each voice
// Play(voice) - call play() on each paramblock
// Play(paramblock) - Run through rhythm gesture, send note on (if not rest), delay, send not off (if not rest)
// 
// Consider sending all prog changes first (first element of first gesture vector in each voice)

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

int Scheduler::Play(MidiOut const& midi_out, Piece piece) const
{
	cout << "Scheduler: running" << endl;

	AllocateVoices(piece);
	InitializeVoices(midi_out, piece);

	// todo: start a thread for each voice
	int i = 0;
	for (auto v : piece)
	{
		cout << "Starting voice " << i++ << endl;
		Play(midi_out, v);
	}

	cout << "Scheduler: done" << endl;
    return 0;
}

void Scheduler::Play(MidiOut const& midi_out, Voice voice) const
{
	auto param_blocks = voice.GetParamBlocks();
	int i = 0;
	for (auto pb : param_blocks)
	{
		cout << "Starting param block " << i++ << endl;
		Play(midi_out, pb);
	}
}

void Scheduler::Play(MidiOut const& midi_out, ParamBlock param_block) const
{
	// Rhythm gesture drives the output.
	// Run through rhythm gesture one time.
	// Other gestures may loop around or not get completely used.

	Gesture rhythm_gesture = param_block.GetRhythmGesture();
	Gesture pitch_gesture = param_block.GetPitchGesture();

	int rhythm_index = 0;	// updated by Next() through reference
	int pitch_index = 0;	// updated by Next() through reference
	for (size_t i = 0; i < rhythm_gesture.Size(); i++)
	{
		int dur = rhythm_gesture.Next(rhythm_index);
		if (dur <= 0)
		{
			// Negative value for duration is a rest - no other gestures are consumed.
			cout << dur << "<rest>" << endl;
		}
		else
		{
			cout << dur << "<" << pitch_gesture.Next(pitch_index) << ">" << endl;
		}
	}
}

void Scheduler::AllocateVoices(std::vector<Voice>& voices) const
{
	// Static voice allocation algorithm.
	// For each voice, set channel number to the next available channel number.
	// Skip channel 10 (percussion).
	// Stop allocating when run out of channels.
	// todo: Possible improvement: if no more channels, go back and find a channel
	// with the same instrument number and use that channel (multi-timbral synth).
	
	int const max_chan = 16;	    // todo: get from generalmidi.h
	int const percussion_chan = 10;	// todo: get from generalmidi.h
	int chan = 1;
	int j = 0;
	for (Voice & v : voices)
	{
		if (chan == percussion_chan)
		{
			++chan;
		}
		if (chan >= max_chan)
		{
			cout << "Warning: out of channels for voice " << j << endl;
			break;
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
