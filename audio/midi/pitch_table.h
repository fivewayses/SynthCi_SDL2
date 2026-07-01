#pragma once

#include <array>
#include <cstdint>
#include "../../extra/compile_time_math.h"

#define TWELFTH_ROOT_OF_2 1.0594630943592953f
#define CONCERT_A_MIDI_NOTE 69
#define CONCERT_A_PITCH 440.0

constexpr std::array<float, 128> GeneratePitchTable()
{
	std::array<float, 128> table {};
	
	for (int i = 0; i < 128; ++i) {
		int16_t semitone_offset = (int16_t)(i - CONCERT_A_MIDI_NOTE);
		
		table[i] = CONCERT_A_PITCH * CompileTimePow(TWELFTH_ROOT_OF_2, semitone_offset);
	}
	
	return table;
}

inline constexpr std::array<float, 128> MIDI_PITCH_TABLE = GeneratePitchTable();
