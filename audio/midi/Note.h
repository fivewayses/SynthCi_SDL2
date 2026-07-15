/**
 * SynthCi_SDL2 - Fivewayses (TM)
 *
 * ===============================
 *
 * MIT License
 * 
 * Copyright (C) Fivewayses (2026).
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files 
 * (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 * 
 *  [*] The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 * 
 *  [*] THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *      MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 *      FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *      CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include <cmath>
#include <cstdint>
#include "lookup_tables.h"

enum class note_e {
	A,
	A_SHARP,
	B,
	C,
	C_SHARP,
	D,
	D_SHARP,
	E,
	F,
	F_SHARP,
	G,
	G_SHARP
};

class Note {
	
public:
	int32_t note;
	
	inline Note(uint8_t midi_note) :
		note(midi_note)
	{
	}

	inline Note(note_e type, int32_t octave = 4) :
		note(12 * octave + (int32_t)type + 9)
	{
	}
	
	inline float CalculatePitch() const {
		if (note >= 0 && note < 128) return MIDI_PITCH_TABLE.data[note];
		
		return CONCERT_A_PITCH * powf(TWELFTH_ROOT_OF_2, note - CONCERT_A_MIDI_NOTE);
	};
	
	inline operator float() const {
		return CalculatePitch();
	}
};
