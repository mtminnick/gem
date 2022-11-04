#include <iostream>
#include "Gesture.h"

using std::cout;
using std::endl;

// todo: templatize these and make them variadic

Gesture make_gesture(int v1, int v2, int v3)
{
	Gesture g;
	g += v1;
	g += v2;
	g += v3;
	return g;
}

ParamBlock make_param_block(Gesture rhythm, Gesture pitches)
{
	ParamBlock pb;
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

void Gesture::Dump() const
{
	for (double val : m_values)
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
