#pragma once

#include <SDL2/SDL_log.h>
#include <SDL2/SDL_timer.h>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "lookup_tables.h"
#include "midi_parser.h"
#include "Synthesizer.h"

constexpr char INVALID_MIDI_DATA_ERR[] = "MIDI data is not valid";

class MIDIData {
	
public:
	MIDIData(const uint8_t *data, uint32_t size)
	{
		valid = ParseMIDI(data, size, midi);
		
		// Un-comment this line to inspect MIDI notes.
		//DebugNotes();
	}
	
	bool Play(Synthesizer &synth)
	{
		if (!valid) {
			PushError(INVALID_MIDI_DATA_ERR);
			return false;
		}
		if (!synth.IsOpen()) {
			PushError("Synthesizer is not open");
			return false;
		}
		
		uint32_t t = SDL_GetTicks();
		uint32_t start = t;
		uint32_t current_t = 0;
		uint32_t step = 0;
		
		static uint32_t notes[256];
		memset(notes, 0, sizeof(notes));
		
		while (true) {
			// Wait until time updates.
			uint32_t new_t;
			while (t >= (new_t = SDL_GetTicks())) SDL_Delay(1);
			t = new_t;
			
			uint32_t t_rel = t - start;
			
			// Make sure we check events on all concurent values of t,
			// even when t skips a value.
			while (current_t <= t_rel) {
				midi_event_t &e = midi.tracks[0].events[step];
				if (e.abs_time != current_t) {
					++current_t;
					continue;
				}
				
				++step;
				if (step >= midi.tracks[0].events.size()) return true;
				
				uint8_t channel = e.type & 0x0f;
				
				switch (e.type & 0xf0) {
				
				case 0x90:
					if (!notes[e.data1])
						notes[e.data1] = synth.AddVoice(MIDI_PITCH_TABLE.data[e.data1]);
					
					break;
					
				case 0x80:
					if (notes[e.data1]) {
						synth.RemoveVoice(notes[e.data1]);
						notes[e.data1] = 0;
					}
				
					break;
				}
				
				++current_t;
			}
		}
		
		return true;
	}
	
	inline bool IsValid() const
	{
		return valid;
	}
	
	bool DebugNotes()
	{
		if (!valid) {
			PushError(INVALID_MIDI_DATA_ERR);
			return false;
		}
		
		for (const midi_event_t &e : midi.tracks[0].events) {
			SDL_Log(
				"abs=%d\ndt=%d\ntype=%x\ndata1=%d\ndata2=%d\n",
				e.abs_time,
				e.dt,
				e.type,
				e.data1,
				e.data2
			);
		}
		
		return true;
	}

private:
	bool valid = false;
	midi_data_t midi;
};
