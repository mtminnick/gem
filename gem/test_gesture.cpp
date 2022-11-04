#include <iostream>
#include "Gesture.h"
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
	ParamBlock pb = make_param_block(r, p);

	cout << "test_param_block (1000, -2000, 3000), (100, 200, 300)" << endl;
	Gesture rhythm = pb.GetRhythmGesture();
	Gesture pitch = pb.GetPitchGesture();
	rhythm.Dump();
	pitch.Dump();
}