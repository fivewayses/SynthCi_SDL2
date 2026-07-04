#pragma once

#include <SDL2/SDL_timer.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "pitch_table.h"
#include "midi_parser.h"
#include "Synthesizer.h"

class MIDIData {
	
public:
	MIDIData(const uint8_t *data, uint32_t size)
	{
		valid = ParseMIDI(data, size, midi);
	}
	
	bool Play(Synthesizer &synth)
	{
		if (!valid || !synth.IsOpen()) return false;
		
		uint32_t t = SDL_GetTicks();
		uint32_t last_callback_time = t;
		uint32_t step = 0;
		
		static uint32_t notes[256];
		memset(notes, 0, sizeof(notes));
		
		while (true) {
			// Wait until time updates.
			uint32_t new_t;
			do {
				new_t = SDL_GetTicks();
			} while (t <= new_t);
			t = new_t;
			
			while (true) {
				midi_event_t &e = midi.tracks[0].events[step];
				if (e.abs_time != t) break;
				
				++step;
				if (step >= midi.tracks[0].events.size()) return true;
				
				uint8_t channel = e.type & 0x0f;
				
				switch (e.type & 0xf0) {
				
				case 0x90:
					if (!notes[e.data1])
						notes[e.data1] = synth.AddVoice(MIDI_PITCH_TABLE[e.data1]);
					
					break;
					
				case 0x80:
					if (notes[e.data1]) {
						synth.RemoveVoice(notes[e.data1]);
						notes[e.data1] = 0;
					}
				
					break;
				}
			}
		}
		
		return true;
	}
	
	inline bool IsValid() const
	{
		return valid;
	}

private:
	bool valid = false;
	midi_data_t midi;
};
