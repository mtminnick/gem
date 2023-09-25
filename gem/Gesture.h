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

#include <vector>

class Gesture
{
private:
	std::vector<int> m_values{};

public:
	Gesture(std::vector<int> values) : m_values{ values } {}
	Gesture() {};
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
	std::vector<Gesture> m_gestures{};
	int m_duration{};

public:
	ParamBlock(int duration, std::vector<Gesture> gestures) : m_duration{ duration }, m_gestures{ gestures } {}
	ParamBlock(int duration) : m_duration{ duration } {}
	ParamBlock() : m_duration{ 0 } {};
	void AddGesture(Gesture g) { m_gestures.push_back(g); }
	Gesture GetRhythmGesture() const;
	Gesture GetPitchGesture() const;
	Gesture GetVelocityGesture() const;
	Gesture GetInstrumentGesture() const;
	auto GetDuration() const { return m_duration; }
	ParamBlock& operator+=(Gesture g) { AddGesture(g); return *this; }
	ParamBlock operator+(Gesture g) const { return ParamBlock(*this) += g; }
};

class Voice
{
private:
	std::vector<ParamBlock> m_param_blocks{};
	int m_voice_number{};

public:
	static inline unsigned char constexpr kUnallocated = 0;

	Voice(std::vector<ParamBlock> param_blocks) : m_voice_number{ kUnallocated }, m_param_blocks{ param_blocks } {}
	Voice() : m_voice_number{ kUnallocated } {}
	void AddParamBlock(ParamBlock pb) { m_param_blocks.push_back(pb); }
	void SetVoiceNumberOnce(int num);
	auto GetVoiceNumber() const { return m_voice_number; }
	auto GetParamBlocks() const { return m_param_blocks; };
	Voice& operator+=(ParamBlock pb) { AddParamBlock(pb); return *this; }
	Voice operator+(ParamBlock pb) const { return Voice(*this) += pb; }
};

// A Piece is a vertical collection of Voices
typedef std::vector<Voice> Piece;

//
// Global variadic template functions to make gestures, parameter blocks, voices and pieces.
//

template<typename ... Ts>
auto make_gesture(Ts... params)
{
    return Gesture{ std::vector<int>{ params... } };
}

template<typename ... Ts>
auto make_param_block(int duration, Ts... params)
{
	return ParamBlock{ duration, std::vector<Gesture>{ params... } };
}

template<typename ... Ts>
auto make_voice(Ts... params)
{
	return Voice{ std::vector<ParamBlock>{ params... } };
}

template<typename ... Ts>
auto make_piece(Ts... params)
{
	return std::vector<Voice>{ params... };
}

