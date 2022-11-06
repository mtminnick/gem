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
	int AbsSum() const;
	void Dump() const;
	Gesture& operator+=(int val) { AddValue(val); return *this; }
	Gesture operator+(int val) const { return Gesture(*this) += val; }
};

class ParamBlock
{
private:
	std::vector<Gesture> m_gestures;
	int m_duration;

public:
	ParamBlock(int duration) : m_duration(duration) {}
	void AddGesture(Gesture g) { m_gestures.push_back(g); }
	// todo: validate index in getters
	// todo: use enum class not magic numbers
	Gesture GetRhythmGesture() const { return m_gestures[0]; }
	Gesture GetPitchGesture() const { return m_gestures[1]; }
	int GetDuration() const { return m_duration; }
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
ParamBlock make_param_block(int duration, Gesture rhythm, Gesture pitches);
Voice make_voice(int instrument, ParamBlock pb1);
Piece make_piece(Voice v1);
