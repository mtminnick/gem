#include <iostream>
#include "Scheduler.h"

using std::cout;
using std::endl;

int Scheduler::Play(Piece const& p)
{
	cout << "Scheduler: running" << endl;

	// Consider sending all prog changes first (first element of first gesture vector in each voice)
	
    // scheduler algo
    // start a thread for each voice
    // each thread: work through each param block
    // for each param block:
    // send program change from inst_gesture
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

	cout << "Scheduler: done" << endl;
    return 0;
}

#if 0
void ParamBlock::Play()
{
	// Rhythm gesture drives the output.
	// Run through rhythm gesture one time.
	// Other gestures may loop around or not get completely used.
	for (size_t i = 0; i < m_rhythm_gesture.Size(); i++)
	{
		double dur = m_rhythm_gesture.Next();
		if (dur <= 0.0)
		{
			// Negative value for duration is a rest - no other gestures are consumed.
			cout << dur << endl;
		}
		else
		{
			cout << dur << "<" << m_pitch_gesture.Next() << ">" << endl;
		}
	}
	m_rhythm_gesture.Reset();
	m_pitch_gesture.Reset();
}
#endif