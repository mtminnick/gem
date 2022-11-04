#pragma once

#include <vector>
#include "Scheduler.h"
#include "Gesture.h"
#include "MidiOut.h"

class Scheduler
{
private:
	void AllocateVoices(std::vector<Voice>& voices) const;
	void InitializeVoices(MidiOut const& midi_out, std::vector<Voice> const & voices) const;
	void Play(MidiOut const & midi_out, Voice voice) const;
	void Play(MidiOut const & midi_out, ParamBlock param_block) const;

public:
	int Play(MidiOut const & midi_out, Piece piece) const;
};