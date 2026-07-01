#pragma once

#include "Note.h"
#include "waveforms.h"
#include <SDL2/SDL_error.h>
#include <cstddef>
#include <cstdint>
#include <climits>
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>

#define MAX_VOICES 256
#define SAMPLE_RATE 24000

struct voice_t {
	uint32_t phase_inc = 0;
	uint16_t phase = 0;
	bool is_active = false;
};

typedef enum {
	SQUARE = 1
} waveform_e;

static voice_t voices[MAX_VOICES];

static void AudioCallback(void *userdata, uint8_t *stream, int len) {
	for (int i = 0; i < len; ++i) {
		int16_t audio_sample = 0;
		int32_t num_active_voices = 0;
		
		for (int j = 0; j < MAX_VOICES; ++j) {
			voice_t &v = voices[j];
			
			if (v.is_active) {
				v.phase += v.phase_inc;
				audio_sample += TRIANGLE_SAMPLE[v.phase];
				++num_active_voices;
			}
		}
		
		stream[i] = num_active_voices ? (uint8_t)(audio_sample / num_active_voices + 128) : 0;
	}
}

class Synthesizer {
	
public:
	Synthesizer()
	{
		Open();
	}
	
	~Synthesizer()
	{
		Close();
	}
	
	void Play()
	{
		SDL_PauseAudioDevice(device, 0);
	}
	
	void Stop() {
		SDL_PauseAudioDevice(device, 1);
	}
	
	bool Open() {
		if (is_open) return false;
		
		SDL_AudioSpec spec {
			.freq = SAMPLE_RATE,
			.format = AUDIO_U8,
			.channels = 1,
			.samples = 512,
			.callback = AudioCallback
		};
		
		device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
		is_open = device > 0;
		
		return is_open;
	}
	
	bool Close() {
		if (!is_open) return false;
		
		SDL_PauseAudioDevice(device, 1);
		SDL_CloseAudioDevice(device);
			
		device = 0;
		is_open = false;
		
		return true;
	}
	
	inline bool IsOpen() const {
		return is_open;
	}
	
	uint32_t AddVoice(float freq) {
		// Find free voice slot
		for (int i = 0; i < MAX_VOICES; ++i) {
			voice_t &v = voices[i];
			if (v.is_active) continue;
			
			v.phase_inc = (uint32_t)(freq * 65536.0f / (float)SAMPLE_RATE);
			v.phase = 0;
			v.is_active = true;
		
			return i + 1;
		}
		
		return 0;
	}
	
	uint32_t AddVoice(uint8_t midi_note) {
		return AddVoice(Note(midi_note));
	}
	
	uint32_t AddVoice(note_e type, int32_t octave = 4) {
		return AddVoice(Note(type, octave));
	}
	
	bool RemoveVoice(uint32_t index) {
		if (index <= 0 || index > MAX_VOICES) return false;
		
		voice_t &v = voices[index-1];
		if (!v.is_active) return false;
		
		v.is_active = false;
		
		return true;
	}
	
	uint8_t *TestAudio(uint32_t sample_size = 1024) {
		uint8_t *sample = new uint8_t[sample_size];
		AudioCallback(NULL, sample, sample_size);
		
		return sample;
	}

private:
	SDL_AudioDeviceID device;
	
	bool is_open = false;
};
