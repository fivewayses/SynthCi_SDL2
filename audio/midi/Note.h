#pragma once

#include "pitch_table.h"
#include <cmath>
#include <cstdint>

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
		if (note >= 0 && note < 128) return MIDI_PITCH_TABLE[note];
		
		return CONCERT_A_PITCH * powf(TWELFTH_ROOT_OF_2, note - CONCERT_A_MIDI_NOTE);
	};
	
	inline operator float() const {
		return CalculatePitch();
	}
};
