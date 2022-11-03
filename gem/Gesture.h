#pragma once
#include <vector>

// todo: it would be nice to make Gesture a type of vector that has wraparound "next" access

typedef std::vector<int> Gesture;			// A Gesture is a horizontal collection of ints
typedef std::vector<Gesture> ParamBlock;	// A ParamBlock is a vertical collection of Gestures
typedef std::vector<ParamBlock> Voice;		// A Voice is a horizontal collection of ParamBlocks
typedef std::vector<Voice> Piece;			// A Piece is a vertical collection of Voices

// todo: todo: templetize these and make them variadic
Gesture make_gesture(int v1, int v2);
ParamBlock make_param_block(Gesture rhythm, Gesture pitches);
Voice make_voice(ParamBlock pb1, ParamBlock pb2);
Piece make_piece(Voice v1, Voice v2);

#if 0

// is Gesture just a vector by another name?
// or is adding modifiers as methods useful?
// Gesture::Randomize() or f = randomize_gesture(g)

class Gesture
{
private:
	std::vector<double> m_values;// consider using ints and ms for rhythm with enums for quarter note, etc.
	int m_next_index = 0;

public:
	void AddValue(double val) { m_values.push_back(val); }
	// consider returning an iterator
	double Next();
	void Reset() { m_next_index = 0;  }
	size_t Size() const { return m_values.size(); }
	void Dump() const;
	Gesture& operator+=(double val) { AddValue(val); return *this; }
	Gesture operator+(double val) const { return Gesture(*this) += val; }
};

Gesture make_gesture(double v1, double v2);

class ParamBlock
{
private:
	// add a single voice number (i.e. midi channel) - no, instrument number belongs with voice.
	Gesture m_rhythm_gesture;
	Gesture m_pitch_gesture;

public:
	void SetRhythmGesture(Gesture rhythm) { m_rhythm_gesture = rhythm; }
	void SetPitchGesture(Gesture pitches) { m_pitch_gesture = pitches; }
	void Dump() const
	{
		// todo: print midi channel
		m_rhythm_gesture.Dump();
		m_pitch_gesture.Dump();
	}
	void Play();
};

ParamBlock make_param_block(Gesture rhythm, Gesture pitches);

class Phrase
{
private:
	ParamBlock m_param_block;

public:
	void SetParamBlock(ParamBlock pb) { m_param_block = pb; }
	void Dump() const { m_param_block.Dump(); }
	void Play() { m_param_block.Play(); }
};

Phrase make_phrase(ParamBlock pb);
#endif