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

#include <cstddef>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_error.h>
#include "Note.h"
#include "lookup_tables.h"
#include "waveforms.h"
#include "error/error.h"

#define MAX_VOICES 256
#define SAMPLE_RATE 24000
#define LTICKS_PER_MS 65536
#define ITERS_PER_ENVELOPE_CALC 16

struct sound_envelope_t {
	uint32_t attack = 50;
	uint32_t decay = 100;
	uint32_t release = 300;
	uint8_t sustain = 192;
};

struct voice_t {
	int64_t time_lticks = 0;
	uint32_t phase_inc = 0;
	uint16_t phase = 0;
	uint8_t envelope = 0;
	bool is_active = false;
};

typedef enum {
	SQUARE = 1,
	SAWTOOTH = 2,
	TRIANGLE = 3,
	SINE = 4
} waveform_e;

const uint32_t SAMPLE_TIME_INC = 1000 * LTICKS_PER_MS / SAMPLE_RATE;

const int8_t *waveform_sample_table = SQUARE_SAMPLE.data;
static sound_envelope_t global_envelope;
static voice_t voices[MAX_VOICES];
static uint16_t envelope_phase = 0;

/* The Audio Callback Which Generates Sound */
static void AudioCallback(void *userdata, uint8_t *stream, int len) {
    for (int i = 0; i < len; ++i) {
        int32_t audio_sample = 0; // Use int32_t to accumulate without overflowing early
        int32_t num_active_voices = 0;
        
        // Bitwise AND is vastly faster than modulo for powers of 2.
        // 15 in binary is 00001111. This triggers every 16 iterations.
        if ((envelope_phase++ & 15) == 0) {
            sound_envelope_t env = global_envelope;
            
            for (int j = 0; j < MAX_VOICES; ++j) {
                voice_t &v = voices[j]; // Just use the reference, copying doesn't make it thread-safe

                if (!v.is_active && v.time_lticks >= 0) continue;
                
                int32_t t = env.attack;
                
                // Pre-calculate this division so we only do it once per envelope update
                int64_t time_ms = v.time_lticks / LTICKS_PER_MS; 
                
                if (v.time_lticks < 0) {
                    // Releasing
                    v.envelope = (env.sustain * -time_ms) / env.release;
                } else if (time_ms < t) {
                    // Attacking: Bitshift instead of * 256
                    v.envelope = (time_ms << 8) / env.attack;
                } else {
                    t += env.decay;
                    if (time_ms < t) {
                        // Decaying
                        v.envelope = env.sustain + ((t - time_ms) * (256 - env.sustain)) / env.decay;
                    } else {
                        // Sustaining
                        v.envelope = env.sustain;
                    }
                }
            }
        }
        
        for (int j = 0; j < MAX_VOICES; ++j) {
            voice_t &v = voices[j];
            
            if (!v.is_active && v.time_lticks >= 0) continue;
            
            v.time_lticks += SAMPLE_TIME_INC;
            v.phase += v.phase_inc;
            
            uint8_t vel = VELOCITY_TABLE.data[v.envelope];
            
            // Assuming WAVEFORM_SAMPLE_DIV is a power of 2 (like 256), use a bitshift here too
            int32_t wsample = waveform_sample_table[v.phase / WAVEFORM_SAMPLE_DIV];
            
            // Bitshift right by 8 is identical to dividing by 256, but much faster
            audio_sample += (wsample * vel) >> 8; 
            
            ++num_active_voices;
        }
        
        // Master volume attenuation
        audio_sample /= 8;
        
        // Clamp to valid 8-bit audio range (-127 to 127)
        if (audio_sample > 127) audio_sample = 127;
        else if (audio_sample < -127) audio_sample = -127;
        
        stream[i] = (uint8_t)(audio_sample + 128);
    }
}

/*
static void AudioCallback(void *userdata, uint8_t *stream, int len) {
	for (int i = 0; i < len; ++i) {
		int16_t audio_sample = 0;
		int32_t num_active_voices = 0;
		
		// The envelope calculation will run every
		// ITERS_PER_ENVELOPE_CALC iterations.
		if (envelope_phase++ % ITERS_PER_ENVELOPE_CALC == 0) {
			sound_envelope_t env = global_envelope;
			
			for (int j = 0; j < MAX_VOICES; ++j) {
				// Copy the voice to the stack to prevent
				// race condition.
				voice_t v = voices[j];

				// If the voice is not active, check first
				// whether time is negative, representing
				// the release envelope.
				if (!v.is_active && v.time_lticks >= 0) continue;
				
				int32_t t = env.attack;
				int64_t time_ms = v.time_lticks / LTICKS_PER_MS;
				
				if (v.time_lticks < 0) {
					// Releasing: sound fades out
					voices[j].envelope = env.sustain * -time_ms / env.release;
				} else if (time_ms < t) {
					// Attacking: sound increases sharply
					voices[j].envelope = 256 * time_ms / env.attack;
				} else {
					t += env.decay;
					if (time_ms < t) {
						// Decaying: sound levels out
						voices[j].envelope = env.sustain + (t - time_ms) * (256 - env.sustain) / env.decay;
					} else {
						// Sustaining: sound stays the same
						voices[j].envelope = env.sustain;
					}
				}
			}
		}
		
		for (int j = 0; j < MAX_VOICES; ++j) {
			voice_t &v = voices[j];
			
			// If the voice is not active, check first
			// whether time is negative, representing
			// the release envelope.
			if (!v.is_active && v.time_lticks >= 0) continue;
			
			v.time_lticks += SAMPLE_TIME_INC;
			v.phase += v.phase_inc;
			
			uint8_t vel = VELOCITY_TABLE.data[v.envelope];
			int32_t wsample = waveform_sample_table[v.phase / WAVEFORM_SAMPLE_DIV];
			audio_sample += (int16_t)(wsample * vel / 256);
			
			++num_active_voices;
		}
		
		audio_sample /= 8;
		
		// Clamp the audio sample.
		if (audio_sample >  127) audio_sample =  127;
		if (audio_sample < -127) audio_sample = -127;
		
		stream[i] = (uint8_t)(audio_sample + 128);
	}
}*/

class Synthesizer {
	
public:
	Synthesizer() {}
	
	~Synthesizer()
	{
		Close();
	}
	
	void Play()
	{
		SDL_PauseAudioDevice(device, 0);
	}
	
	void Stop()
	{
		SDL_PauseAudioDevice(device, 1);
	}
	
	bool Open()
	{
		if (is_open) {
			PushError("Audio device is already open");
			return false;
		}
		
		SDL_AudioSpec spec {
			.freq = SAMPLE_RATE,
			.format = AUDIO_U8,
			.channels = 1,
			.samples = 512,
			.callback = AudioCallback
		};
		
		device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
		is_open = device > 0;
		if (!is_open) RedirectSDLError();
		
		return is_open;
	}
	
	bool Close()
	{
		if (!is_open) {
			PushError("Audio device is already closed");
			return false;
		}
		
		SDL_PauseAudioDevice(device, 1);
		SDL_CloseAudioDevice(device);
			
		device = 0;
		is_open = false;
		
		return true;
	}
	
	inline bool IsOpen() const
	{
		return is_open;
	}
	
	bool SetWaveformType(waveform_e wave_type)
	{
		switch (wave_type) {
			
		case waveform_e::SQUARE:
			waveform_sample_table = SQUARE_SAMPLE.data;
			break;
			
		case waveform_e::SAWTOOTH:
			waveform_sample_table = SAWTOOTH_SAMPLE.data;
			break;
			
		case waveform_e::TRIANGLE:
			waveform_sample_table = TRIANGLE_SAMPLE.data;
			break;
			
		case waveform_e::SINE:
			waveform_sample_table = SINE_SAMPLE.data;
			break;
			
		default:
			PushError("Invalid waveform mode");
			return false;
		}
		
		return true;
	}
	
	uint32_t AddVoice(float freq)
	{
		// Find free voice slot.
		for (int i = 0; i < MAX_VOICES; ++i) {
			voice_t &v = voices[i];
			if (v.is_active || v.time_lticks < 0) continue;
			
			v.time_lticks = 0;
			v.phase_inc = (uint32_t)(freq * 65536.0f / (float)SAMPLE_RATE);
			v.phase = 0;
			v.envelope = 0;
			v.is_active = true;
		
			return i + 1;
		}
		
		// Try to find releasing voice slot
		// if no fully free slot is avalible.
		for (int i = 0; i < MAX_VOICES; ++i) {
			voice_t &v = voices[i];
			if (v.is_active) continue;
			
			v.time_lticks = 0;
			v.phase_inc = (uint32_t)(freq * 65536.0f / (float)SAMPLE_RATE);
			v.phase = 0;
			v.envelope = 0;
			v.is_active = true;
		
			return i + 1;
		}
		
		return 0;
	}
	
	uint32_t AddVoice(uint8_t midi_note)
	{
		return AddVoice(Note(midi_note));
	}
	
	uint32_t AddVoice(note_e type, int32_t octave = 4)
	{
		return AddVoice(Note(type, octave));
	}
	
	bool RemoveVoice(uint32_t index)
	{
		if (index <= 0 || index > MAX_VOICES) {
			PushError("Voice index (%d) out of range of 1 - %d", index, MAX_VOICES);
			return false;
		}
		
		voice_t &v = voices[index-1];
		if (!v.is_active) {
			PushError("Voice #%d is not active", index);
			return false;
		}
		
		v.time_lticks = -(int64_t)global_envelope.release * LTICKS_PER_MS;
		v.is_active = false;
		
		return true;
	}
	
	uint8_t *TestAudio(uint32_t sample_size = 1024)
	{
		uint8_t *sample = new uint8_t[sample_size];
		AudioCallback(NULL, sample, sample_size);
		
		return sample;
	}

private:
	SDL_AudioDeviceID device;
	
	bool is_open = false;
};
