#include "Gesture.h"

// todo: templetize these and make them variadic

Gesture make_gesture(int v1, int v2)
{
	Gesture g;
	g.push_back(v1);
	g.push_back(v2);
	return g;
}

ParamBlock make_param_block(Gesture rhythm, Gesture pitches)
{
	ParamBlock pb;
	pb.push_back(rhythm);
	pb.push_back(pitches);
	return pb;
}

Voice make_voice(ParamBlock pb1, ParamBlock pb2)
{
	Voice v;
	v.push_back(pb1);
	v.push_back(pb2);
	return v;
}

Piece make_piece(Voice v1, Voice v2)
{
	Piece p;
	p.push_back(v1);
	p.push_back(v2);
	return p;
}

#if 0
Gesture make_gesture(double v1, double v2)
{
	// change to variadic
	Gesture g;
	g += v1;
	g += v2;
	return g;
}

ParamBlock make_param_block(Gesture rhythm, Gesture pitches)
{
	// change to take all gesture in constructor instead of Set
	ParamBlock pb;
	pb.SetRhythmGesture(rhythm);
	pb.SetPitchGesture(pitches);
	return pb;
}

Phrase make_phrase(ParamBlock pb)
{
	// Change to variadic and change Set to Add
	Phrase ph;
	ph.SetParamBlock(pb);
	return ph;
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

double Gesture::Next()
{
	// fixme: use iterator?
	if (m_next_index >= m_values.size())
	{
		Reset();
	}

	return m_values[m_next_index++];
}

//
// ParamBlock implementation.
//

void ParamBlock::Play()
{
	// Rhythm gesture drives the output.
	// Run through rhythm gesture one time.
	// Other gestures may loop around or not get completely used.
	for (size_t i = 0; i < m_rhythm_gesture.Size(); i++)
	{
		double dur = m_rhythm_gesture.Next();
		if (dur <= 0.0)
		{
			// Negative value for duration is a rest - no other gestures are consumed.
			cout << dur << endl;
		}
		else
		{
			cout << dur << "<" << m_pitch_gesture.Next() << ">" << endl;
		}
	}
	m_rhythm_gesture.Reset();
	m_pitch_gesture.Reset();
}
#endif