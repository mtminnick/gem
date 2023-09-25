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

Dictionary build_dictionary()
{
	Dictionary dict{};

	dict["all-pitches"] = vector<Gesture>{ make_gesture(c4, e4, fs4),
										   make_gesture(b4, g4, as4, d4),
		                                   make_gesture(cs4, ds4, g4, ds4),
			                               make_gesture(a4, f4) };
	dict["slow-drama"] = vector<Gesture>{  make_gesture(nW, -nQd, nQd, nH, -nH),
										   make_gesture(-nH, nW, -nQd, nQd, nH),
										   make_gesture(nH, -nH, nW, -nQd, nQd) };

	return dict;
}

// add get_dict(string, dict) returns random gesture inside "string" vector
Gesture get_gesture(Dictionary const& dict, std::string const& key)
{
	// Pull out the gesture array that matches the key.
	auto it{ dict.find(key) };
	if (it == dict.end())
	{
		cerr << "Warning: no gesture array for key '" << key << "'" << '\n';
		// Such an empty gesture.
		return make_gesture();
	}
	vector<Gesture> vect{ it->second };

	// Return a random gesture from the gesture array.
	size_t top{ vect.size() };
	if (top == 0)
	{
		cerr << "Warning: empty gesture array for key '" << key << "'" << '\n';
		return make_gesture();
	}
	else
	{
		size_t idx{ rand() % top };
		return vect.at(idx);
	}
}
