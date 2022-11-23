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
#include <cstdlib>
#include "Gesture.h"
#include "generalmidi.h"

using std::cout;
using std::cerr;
using std::endl;
using std::abs;

//
// Gesture implementation.
//

int Gesture::AbsSum() const
{
	int total = 0;
	for (auto val : m_values)
	{
		total += abs(val);
	}
	return total;
}

void Gesture::Dump() const
{
	for (auto val : m_values)
	{
		cout << val << " ";
	}
	cout << endl;
}

int Gesture::Next(int& idx) const
{
	// todo: use iterator
	if (idx >= m_values.size())
	{
		idx = 0;
	}

	return m_values[idx++];
}

//
// ParamBlock implementation.
//

inline int constexpr kRhythmIndex = 0;
inline int constexpr kPitchIndex = 1;
inline int constexpr kVelocityIndex = 2;

Gesture ParamBlock::GetRhythmGesture() const
{
	if (m_gestures.size() > kRhythmIndex)
	{
		return m_gestures[kRhythmIndex];
	}
	else
	{
		cerr << "Error: missing rhythm gesture" << endl;
		throw std::runtime_error("");
	}
}

Gesture ParamBlock::GetPitchGesture() const
{ 
	if (m_gestures.size() > kPitchIndex)
	{
		return m_gestures[kPitchIndex];
	}
	else
	{
		cerr << "Error: missing pitch gesture" << endl;
		throw std::runtime_error("");
	}
}

Gesture ParamBlock::GetVelocityGesture() const
{
	if (m_gestures.size() > kVelocityIndex)
	{
		return m_gestures[kVelocityIndex];
	}
	else
	{
		cerr << "Warning: missing velocity gesture" << endl;
		return make_gesture(24);
	}
}
