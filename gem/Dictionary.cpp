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

#include <cstdlib>
#include <iostream>
#include "Dictionary.h"
#include "generalmidi.h"

using std::vector;
using std::cerr;
using std::endl;

Dictionary build_dictionary()
{
	Dictionary dict{};

	// Pitch intervals.
	dict["unison"] = vector<Gesture>{ make_gesture(c4) };
	dict["minor_second"] = vector<Gesture>{ make_gesture(c4, cs4) };
	dict["major-second"] = vector<Gesture>{ make_gesture(c4, d4) };
	dict["minor-third"] = vector<Gesture>{ make_gesture(c4, ds4) };
	dict["major-third"] = vector<Gesture>{ make_gesture(c4, e4) };
	dict["forth"] = vector<Gesture>{ make_gesture(c4, f4) };
	dict["tritone"] = vector<Gesture>{ make_gesture(c4, fs4) };
	dict["fifth"] = vector<Gesture>{ make_gesture(c4, g4) };
	dict["minor-sixth"] = vector<Gesture>{ make_gesture(c4, gs4) };
	dict["sixth"] = vector<Gesture>{ make_gesture(c4, a4) };
	dict["flat-seventh"] = vector<Gesture>{ make_gesture(c4, as4) };
	dict["seventh"] = vector<Gesture>{ make_gesture(c4, b4) };

	return dict;
}

// add get_dict(string, dict) returns random gesture inside "string" vector
Gesture get_gesture(Dictionary const& dict, std::string const& key)
{
	// Pull out the gesture array that matches the key.
	auto it = dict.find(key);
	if (it == dict.end())
	{
		cerr << "Warning: no gesture array for key '" << key << "'" << endl;
		// Such an empty gesture.
		return make_gesture();
	}
	vector<Gesture> vect = it->second;

	// Return a random gesture from the gesture array.
	size_t top = vect.size();
	if (top == 0)
	{
		cerr << "Warning: empty gesture array for key '" << key << "'" << endl;
		return make_gesture();
	}
	else
	{
		int idx = rand() % top;
		return vect.at(idx);
	}
}
