#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>
#include <SDL2/SDL.h>
#include "../../error/error.h"
#include "../../extra/big_endian.h"
#include "../../extra/vlq.h"

#define MIDI_HEADER_CHUNK_SIZE 14
#define MIDI_TRACK_CHUNK_SIZE 8

const char MIDI_MTHD[] = "MThd";
const char MIDI_MTRK[] = "MTrk";

struct midi_event_t {
	uint32_t abs_time;
	uint32_t dt;
	uint8_t type;
	uint8_t data1;
	uint8_t data2;
};

struct midi_track_t {
	std::vector<midi_event_t> events;
};

struct midi_data_t {
	uint32_t num_tracks;
	std::vector<midi_track_t> tracks;
};

static bool ParseMIDI(const uint8_t *raw_midi, uint32_t size, midi_data_t &out)
{
	if (!raw_midi) {
		PushError({ "MIDI data is NULL" });
		return false;
	}
	if (size == 0) {
		PushError({ "MIDI size is 0" });
		return false;
	}
	
	uint32_t required_size = MIDI_HEADER_CHUNK_SIZE;
	uint32_t offset = 0;
	
	/* MIDI Header */
	if (size < required_size) {
		PushError({ "MIDI not big enough for header" });
		return false;
	}
	if (memcmp(raw_midi, MIDI_MTHD, 4) != 0) {
		PushError({ "MIDI header does not start with MThd" });
		return false;
	}
	
	// TODO: Add MIDI format checking
	
	uint32_t num_tracks = ReadBigEndian<uint16_t>(&raw_midi[6]);
	int16_t div = ReadBigEndian<int16_t>(&raw_midi[8]);
	double ms_per_tick = 500000.0 / div;
	
	offset += MIDI_HEADER_CHUNK_SIZE;
	
	std::vector<midi_track_t> tracks {};
	tracks.reserve(num_tracks);
	
	/* MIDI Tracks */
	for (int i = 0; i < num_tracks; ++i) {
		required_size += MIDI_TRACK_CHUNK_SIZE;
	
		if (size < required_size) break;
		if (memcmp(&raw_midi[offset], MIDI_MTRK, 4) != 0) {
			PushError({ "MIDI track #%d does not start with MTrk", i });
			return false;
		}
	
		uint32_t track_size = ReadBigEndian<uint32_t>(&raw_midi[offset+4]);
		offset += MIDI_TRACK_CHUNK_SIZE;
		required_size += track_size;
	
		if (size < required_size) {
			PushError({ "MIDI is not big enough (%d < %d, Track=%d)", size, required_size, i });
			return false;
		}
		
		std::vector<midi_event_t> events {};
		events.reserve(track_size / 2);
		
		uint32_t track_end = offset + track_size;
		uint8_t running_status = 0;
		uint32_t abs_time = 0;
		
		/* MIDI Events */
		while (offset < track_end) {
			uint32_t dt = ReadVLQ<uint32_t>(raw_midi, offset);
			abs_time += dt;
			
			uint8_t status = raw_midi[offset];

			// Running Status logic: If highest bit is 0, it's data, not a status byte.
			// We reuse the last seen status byte and DO NOT advance the offset.
			if ((status & 0x80) == 0) {
				status = running_status;
			} else {
				running_status = status;
				offset++; 
			}
			
			// Check the event category
			if (status == 0xFF) {
				// META EVENT (e.g., Tempo, Track Name, End of Track)
				uint8_t meta_type = raw_midi[offset++];
				uint32_t len = ReadVLQ<uint32_t>(raw_midi, offset);
				
				// You can parse Tempo (meta_type == 0x51) here if needed.
				// For now, we just skip the data payload.
				offset += len;
				
			} else if (status == 0xF0 || status == 0xF7) {
				// SYSEX EVENT
				uint32_t len = ReadVLQ<uint32_t>(raw_midi, offset);
				offset += len;
				
			} else {
				// MIDI CHANNEL EVENT
				uint8_t event_type = status & 0xF0; // Strip the channel (lower 4 bits)
				
				// Extract Data Bytes depending on the event type
				if (event_type == 0xC0 || event_type == 0xD0) {
					// Program Change & Channel Pressure only have 1 data byte
					offset += 1; 
				} else {
					// Note On, Note Off, Pitch Bend, Control Change have 2 data bytes
					uint8_t data1 = raw_midi[offset++];
					uint8_t data2 = raw_midi[offset++];
					
					// We only care about saving Note On and Note Off for your synth
					if (event_type == 0x90 || event_type == 0x80) {
						// In MIDI, a Note On (0x90) with 0 velocity is actually a Note Off
						if (event_type == 0x90 && data2 == 0) {
							event_type = 0x80;
						}
						
						uint32_t abs_time_ms = (double)abs_time * ms_per_tick;
						
						events.push_back({
							abs_time_ms,
							dt,
							event_type,
							data1, // Note
							data2  // Velocity
						});
					}
				}
			}
		}
		
		tracks.push_back({
			std::move(events)
		});
		offset = track_end;
	}
	
	out.num_tracks = num_tracks;
	out.tracks = std::move(tracks);
	
	return true;
}
