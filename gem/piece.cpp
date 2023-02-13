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
#include "piece.h"
#include "Gesture.h"
#include "Scheduler.h"
#include "generalmidi.h"
#include "Dictionary.h"

using std::cout;
using std::endl;

void piece1(MidiOut &midi_out)
{
	Gesture rhythm = make_gesture(1000, -500, 500);
	Gesture pitch = make_gesture(c4, b4, a4);
	int const pb_total_time = 10000;
	ParamBlock pb = make_param_block(pb_total_time, rhythm, pitch);
	Voice v = make_voice(pb);
	Piece p = make_piece(v);

	cout << "** Piece 1 **" << endl;

	Scheduler s;
	s.Play(midi_out, p);
}

void piece2(MidiOut& midi_out)
{
	int const pb_total_time = 10000;

	Gesture rhythm = make_gesture(nQ, -n8, n8);
	Gesture pitch = make_gesture(c4, b4, a4);
	ParamBlock pb = make_param_block(pb_total_time, rhythm, pitch);
	Voice v1 = make_voice(pb);

	Gesture rhythm2 = make_gesture(-n8, nQ, n8);
	Gesture pitch2 = make_gesture(d3, cs4, b3);
	ParamBlock pb2 = make_param_block(pb_total_time, rhythm2, pitch2);
	Voice v2 = make_voice(pb2);

	Piece p = make_piece(v1, v2);

	cout << "** Piece 2 **" << endl;

	Scheduler s;
	s.Play(midi_out, p);
}

void piece3(MidiOut& midi_out)
{
	int const pb_total_time = 10000;

	Gesture rhy_slow	= make_gesture(nWd, nQd, nHd);
	Gesture rhy_fast	= make_gesture(-nH, n32, n32, n32, -n8, n16, n32, -n8d);
	Gesture pitch_low	= make_gesture(g2, a2, gs2, b2, as2);
	Gesture pitch_high	= make_gesture(c4, cs4, d4, ds4, fs4, f4, e4);
	Gesture vel_soft	= make_gesture(40); // for piano
	Gesture vel_loud	= make_gesture(20); // for choir
	Gesture ins_piano	= make_gesture(acoustic_grand_piano);
	Gesture ins_choir	= make_gesture(choir_aahs);

	ParamBlock pb = make_param_block(pb_total_time, rhy_fast, pitch_high, vel_soft, ins_piano);
	Voice v1 = make_voice(pb);

	ParamBlock pb2 = make_param_block(pb_total_time, rhy_slow, pitch_low, vel_loud, ins_choir);
	Voice v2 = make_voice(pb2);

	Piece p = make_piece(v1, v2);

	cout << "** Piece 3 **" << endl;

	Scheduler s;
	s.Play(midi_out, p);
}

void piece4(MidiOut& midi_out)
{
	int const pb_total_time = 15000;

	Gesture rhy1 = make_gesture(nQd, -n8, n32, n32, -nQ, n16, -n8d, n16d, n16d, -nQ);
	Gesture perc_instr1 = make_gesture(acoustic_bass_drum, high_tom, open_triangle, acoustic_snare, crash_cymbal_1);
	Gesture vel1 = make_gesture(85, 75, 60);

	ParamBlock pb1 = make_param_block(pb_total_time, rhy1, perc_instr1, vel1);
	Voice v1 = make_voice(pb1);

	const int roll = n32 / 2;
	Gesture rhy2 = make_gesture(-nW, roll, roll, roll, roll, roll, -nWd, roll, roll, roll);
	Gesture perc_instr2 = make_gesture(acoustic_snare);
	Gesture vel2 = make_gesture(65, 55, 55, 55);

	ParamBlock pb2 = make_param_block(pb_total_time, rhy2, perc_instr2, vel2);
	Voice v2 = make_voice(pb2);

	// Force voices to percussion channel
	v1.SetVoiceNumberOnce(kPercussionChannel);
	v2.SetVoiceNumberOnce(kPercussionChannel);

	Piece p = make_piece(v1, v2);

	cout << "** Piece 4 **" << endl;

	Scheduler s;
	s.Play(midi_out, p);
}

void piece5(MidiOut& midi_out)
{
	int const pb_total_time = 20000;
	const auto dict = build_dictionary();

	Gesture vel = make_gesture(60);
	Gesture ins = make_gesture(church_organ);

	// Each call to dict pulls a random gesture.
	Gesture pitch1 = get_gesture(dict, "all-pitches");
	Gesture pitch2 = get_gesture(dict, "all-pitches");
	Gesture pitch3 = get_gesture(dict, "all-pitches");
	//Gesture pitch4 = get_gesture(dict, "all-pitches");

	Gesture rhy1 = get_gesture(dict, "slow-drama");
	Gesture rhy2 = get_gesture(dict, "slow-drama");
	Gesture rhy3 = get_gesture(dict, "slow-drama");
	//Gesture rhy4 = get_gesture(dict, "slow-drama");

	ParamBlock pb1 = make_param_block(pb_total_time, rhy1, pitch1, vel, ins);
	ParamBlock pb2 = make_param_block(pb_total_time, rhy2, pitch2, vel, ins);
	ParamBlock pb3 = make_param_block(pb_total_time, rhy3, pitch3, vel, ins);
	//ParamBlock pb4 = make_param_block(pb_total_time, rhy4, pitch4, vel, ins);

	Voice v1 = make_voice(pb1);
	Voice v2 = make_voice(pb2);
	Voice v3 = make_voice(pb3);
	//Voice v4 = make_voice(pb4);

	//Piece p = make_piece(v1, v2, v3, v4);
	Piece p = make_piece(v1, v2, v3);

	cout << "** Piece 5 **" << endl;

	Scheduler s;
	s.Play(midi_out, p);
}