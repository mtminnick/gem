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

#include "piece.h"
#include "Gesture.h"
#include "Scheduler.h"
#include "generalmidi.h"

int piece1(MidiOut &midi_out)
{
	Gesture rhythm = make_gesture(1000, -500, 500);
	Gesture pitch = make_gesture(c4, b4, a4);
	int const pb_total_time = 10000;
	ParamBlock pb = make_param_block(pb_total_time, rhythm, pitch);
	Voice v = make_voice(acoustic_grand_piano, pb);
	Piece p = make_piece(v);

	Scheduler s;
	int ret = s.Play(midi_out, p);
	return ret;
}

int piece2(MidiOut& midi_out)
{
	int const pb_total_time = 10000;

	Gesture rhythm = make_gesture(nQ, -n8, n8);
	Gesture pitch = make_gesture(c4, b4, a4);
	ParamBlock pb = make_param_block(pb_total_time, rhythm, pitch);
	Voice v1 = make_voice(acoustic_grand_piano, pb);

	Gesture rhythm2 = make_gesture(-n8, nQ, n8);
	Gesture pitch2 = make_gesture(d3, cs4, b3);
	ParamBlock pb2 = make_param_block(pb_total_time, rhythm2, pitch2);
	Voice v2 = make_voice(choir_aahs, pb2);

	Piece p = make_piece(v1, v2);

	Scheduler s;
	int ret = s.Play(midi_out, p);
	return ret;
}

int piece3(MidiOut& midi_out)
{
	int const pb_total_time = 10000;

	Gesture rhy_slow = make_gesture(nWd, nQd, nHd);
	Gesture rhy_fast = make_gesture(-nH, n32, n32, n32, -n8, n16, n32, -n8d);
	Gesture pitch_low = make_gesture(g2, a2, gs2, b2, as2);
	Gesture pitch_high = make_gesture(c4, cs4, d4, ds4, fs4, f4, e4);
	Gesture vel_soft = make_gesture(40); // for piano
	Gesture vel_loud = make_gesture(20); // for voice

	ParamBlock pb = make_param_block(pb_total_time, rhy_fast, pitch_high, vel_soft);
	Voice v1 = make_voice(acoustic_grand_piano, pb);

	ParamBlock pb2 = make_param_block(pb_total_time, rhy_slow, pitch_low, vel_loud);
	Voice v2 = make_voice(choir_aahs, pb2);

	Piece p = make_piece(v1, v2);

	Scheduler s;
	int ret = s.Play(midi_out, p);
	return ret;
}