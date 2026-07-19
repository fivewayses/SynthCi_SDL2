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

#include <cstdint>
#include "extra/compile_time_math.h"

#define TWELFTH_ROOT_OF_2 1.0594630943592953f
#define CONCERT_A_MIDI_NOTE 69
#define CONCERT_A_PITCH 440.0

struct pitch_table_t {
	float data[128];
};

struct velocity_table_t {
	uint8_t data[256];
};

constexpr pitch_table_t GeneratePitchTable()
{
	pitch_table_t table {};
	
	for (int i = 0; i < 128; ++i) {
		int16_t semitone_offset = (int16_t)(i - CONCERT_A_MIDI_NOTE);
		
		table.data[i] = CONCERT_A_PITCH * CompileTimePow(TWELFTH_ROOT_OF_2, semitone_offset);
	}
	
	return table;
}

constexpr velocity_table_t GenerateVelocityTable()
{
	velocity_table_t table {};
	
	for (int i = 0; i < 256; ++i) {
		table.data[i] = (uint8_t)((uint16_t)i * i / 256);
	}
	
	return table;
}

inline constexpr pitch_table_t MIDI_PITCH_TABLE  = GeneratePitchTable();
inline constexpr velocity_table_t VELOCITY_TABLE = GenerateVelocityTable();
