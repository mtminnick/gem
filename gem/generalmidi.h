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

inline unsigned char constexpr kNoteOn = 0x90;
inline unsigned char constexpr kProgramChange = 0xc0;
inline unsigned char constexpr kControlChange = 0xb0;

inline unsigned char constexpr kControllerModWheelMSB = 0x01;
inline unsigned char constexpr kControllerPanMSB = 0x0a;

inline unsigned char constexpr kPercussionChannel = 10;
inline unsigned char constexpr kMaxChannelNumber = 16;

enum PatchMap : int
{
	acoustic_grand_piano	= 1,
	//...
	church_organ		    = 20,
	//...
	choir_aahs				= 53,
	//...
	flute					= 74,
	//...
};

enum PercussionMap : int
{
	acoustic_bass_drum = 35,
	//...
	acoustic_snare = 38,
	//...
	low_mid_tom = 47,
	//...
	crash_cymbal_1 = 49,
	high_tom = 50,
	//...
	open_triangle = 81
};

enum NoteNumber: int
{
	cm1		= 0,
	csm1	= 1,
	dm1		= 2,
	dsm1	= 3,
	em1		= 4,
	fm1		= 5,
	fsm1	= 6,
	gm1		= 7,
	gsm1	= 8,
	am1		= 9,
	asm1	= 10,
	bm1		= 11,
	//...
	c2		= 36,
	cs2		= 37,
	d2		= 38,
	ds2		= 39,
	e2		= 40,
	f2		= 41,
	fs2		= 42,
	g2		= 43,
	gs2		= 44,
	a2		= 45,
	as2		= 46,
	b2		= 47,
	//
	c3		= 48,
	cs3		= 49,
	d3		= 50,
	ds3		= 51,
	e3		= 52,
	f3		= 53,
	fs3		= 54,
	g3		= 55,
	gs3		= 56,
	a3		= 57,
	as3		= 58,
	b3		= 59,
	//
	c4		= 60,
	cs4		= 61,
	d4		= 62,
	ds4		= 63,
	e4		= 64,
	f4		= 65,
	fs4		= 66,
	g4		= 67,
	gs4		= 68,
	a4		= 69,
	as4		= 70,
	b4		= 71,
	//
	c5		= 72,
	cs5		= 73,
	d5		= 74,
	ds5		= 75,
	e5		= 76,
	f5		= 77,
	fs5		= 78,
	g5		= 79,
	gs5		= 80,
	a5		= 81,
	as5		= 82,
	b5		= 83,
	//...
};

// Not General Midi, but a convenient place to specify.
// Durations in milliseconds at quarter note = 60 bpm.
inline int constexpr nW = 1000 * 4;
inline int constexpr nH = 1000 * 2;
inline int constexpr nQ = 1000 * 1;
inline int constexpr n8 = 1000 / 2;
inline int constexpr n16 = 1000 / 4;
inline int constexpr n32 = 1000 / 8;
// Dots
inline int constexpr nWd = nW + nW / 2;
inline int constexpr nHd = nH + nH / 2;
inline int constexpr nQd = nQ + nQ / 2;
inline int constexpr n8d = n8 + n8 / 2;
inline int constexpr n16d = n16 + n16 / 2;
inline int constexpr n32d = n32 + n32 / 2;
// Triplets
inline int constexpr nWt = nW / 3;
inline int constexpr nHt = nH / 3;
inline int constexpr nQt = nQ / 3;
inline int constexpr n8t = n8 / 3;
inline int constexpr n16t = n16 / 3;
inline int constexpr n32t = n32 / 3;