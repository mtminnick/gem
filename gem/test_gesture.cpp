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
#include "Gesture.h"
#include "Scheduler.h"
#include "test_gesture.h"

using std::cout;
using std::endl;

void test_gesture_wrap()
{
	Gesture g = make_gesture(100, 200, 300);

	cout << "test_gesture_wrap (100, 200, 300)" << endl;
	int j = 0;
	for (int i = 0; i < 10; i++)
	{
		cout << g.Next(j) << " ";
	}
	cout << endl;
}

void test_param_block()
{
	Gesture r = make_gesture(1000, -2000, 3000);
	Gesture p = make_gesture(100, 200, 300);
	ParamBlock pb = make_param_block(r.AbsSum(), r, p);

	cout << "test_param_block (1000, -2000, 3000), (100, 200, 300)" << endl;
	Gesture rhythm = pb.GetRhythmGesture();
	Gesture pitch = pb.GetPitchGesture();
	rhythm.Dump();
	pitch.Dump();
}

void test_voice_alloc(MidiOut& midi_out)
{
	Gesture r = make_gesture(100, -200, 300);
	Gesture p = make_gesture(64, 65, 66);
	ParamBlock pb = make_param_block(r.AbsSum(), r, p);

	int const too_many_voices = 17;
	Piece piece;
	for (int i = 0; i < too_many_voices; i++)
	{
		Voice v = make_voice(1, pb);
		piece.push_back(v);
	}

	Scheduler s;
	s.Play(midi_out, piece);
}