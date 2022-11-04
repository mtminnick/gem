#pragma once
#include <vector>

// todo: these classes are similar enough they could probably be templatized.
// todo: could make refs to these return ref to vector, then don't need Size() or vector getters,
// and/or possibly return an iterator instead of the whole vector
// and/or possibly implement a Next() like Gesture

class Gesture
{
private:
	std::vector<int> m_values;

public:
	void AddValue(int val) { m_values.push_back(val); }
	// todo: consider returning an iterator
	int Next(int& idx) const;
	size_t Size() const { return m_values.size(); }
	void Dump() const;
	Gesture& operator+=(int val) { AddValue(val); return *this; }
	Gesture operator+(int val) const { return Gesture(*this) += val; }
};

class ParamBlock
{
private:
	std::vector<Gesture> m_gestures;

public:
	void AddGesture(Gesture g) { m_gestures.push_back(g); }
	size_t Size() const { return m_gestures.size(); }
	// todo: validate index in getters
	// todo: use enum class not magic numbers
	Gesture GetRhythmGesture() const { return m_gestures[0]; }
	Gesture GetPitchGesture() const { return m_gestures[1]; }
	ParamBlock& operator+=(Gesture g) { AddGesture(g); return *this; }
	ParamBlock operator+(Gesture g) const { return ParamBlock(*this) += g; }
};

class Voice
{
private:
	std::vector<ParamBlock> m_param_blocks;
	int m_voice_number;
	int m_instrument_number;

public:
	Voice(int inst_num) : m_voice_number(0), m_instrument_number(inst_num) {}
	void AddParamBlock(ParamBlock pb) { m_param_blocks.push_back(pb); }
	size_t Size() const { return m_param_blocks.size(); }
	void SetVoiceNumber(int num) { m_voice_number = num; }
	int GetVoiceNumber() const { return m_voice_number; }
	int GetInstrumentNumber() const { return m_instrument_number;  }
	std::vector<ParamBlock> GetParamBlocks() const { return m_param_blocks; };
	Voice& operator+=(ParamBlock pb) { AddParamBlock(pb); return *this; }
	Voice operator+(ParamBlock pb) const { return Voice(*this) += pb; }
};

typedef std::vector<Voice> Piece;			// A Piece is a vertical collection of Voices

// todo: templatize these and make them variadic
Gesture make_gesture(int v1, int v2, int v3);
ParamBlock make_param_block(Gesture rhythm, Gesture pitches);
Voice make_voice(int instrument, ParamBlock pb1);
Piece make_piece(Voice v1);
