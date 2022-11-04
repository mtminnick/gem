#pragma once

#include "MidiOut.h"

void test_note_on_off(MidiOut const & mout);
void test_channels(MidiOut const& mout);
void test_program_change(MidiOut const& mout);
void test_polyphony(MidiOut const& mout);
void test_parameters(MidiOut const& mout);