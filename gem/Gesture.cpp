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

using std::cout;
using std::endl;
using std::abs;

// todo: templatize these and make them variadic

Gesture make_gesture(int v1, int v2, int v3)
{
	Gesture g;
	g += v1;
	g += v2;
	g += v3;
	return g;
}

ParamBlock make_param_block(int duration, Gesture rhythm, Gesture pitches)
{
	ParamBlock pb{duration};
	pb += rhythm;
	pb += pitches;
	return pb;
}

Voice make_voice(int instrument, ParamBlock pb1)
{
	Voice v{instrument};
	v += pb1;
	return v;
}

Piece make_piece(Voice v1)
{
	Piece p;
	p.push_back(v1);
	return p;
}

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
