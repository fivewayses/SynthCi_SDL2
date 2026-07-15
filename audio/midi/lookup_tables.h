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

inline constexpr pitch_table_t MIDI_PITCH_TABLE = GeneratePitchTable();
inline constexpr velocity_table_t VELOCITY_TABLE = GenerateVelocityTable();
