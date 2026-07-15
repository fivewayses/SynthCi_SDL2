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

#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <SDL2/SDL_log.h>
#include "audio/midi/Note.h"
#include "audio/midi/Synthesizer.h"
#include "audio/midi/MIDIData.h"
#include "error/error.h"
#include "example/note_c.h"

#define CheckError(status, ...) if (!(status)) { \
	SDL_Log(__VA_ARGS__); \
	return 1; \
}

static Synthesizer synth;

int Test()
{
	const uint32_t size = 63356;
	const char *test = reinterpret_cast<char *>(synth.TestAudio(size));
	
	std::fstream stream("testing.bin", std::ios::out | std::ios::binary);
	CheckError(stream.is_open(), "Can't open file [ERROR: %s]\n", strerror(errno));
	
	stream.write(test, size);
	
	return 0;
}

bool CheckArg(const char* str, int argc, char **argv)
{
	if (!str || argc <= 0 || !argv) return false;
	
	for (int i = 0; i < argc; ++i) {
		if (strcmp(str, argv[i]) == 0) return true;
	}
	
	return false;
}

int main(int argc, char **argv)
{
	const bool do_midi_test = CheckArg("--testmidi", argc, argv);
	const bool do_audio_sample_test = CheckArg("--testaudio", argc, argv);
	const bool do_time_logging = CheckArg("--logtime", argc, argv);
	
	CheckError(
		SDL_Init(SDL_INIT_AUDIO) == 0,
		"Failed to initialize SDL: %s\n", SDL_GetError()
	);
	
	MIDIData midi(note_c, note_c_len);
	CheckError(
		midi.IsValid(),
		"MIDI parsing failed: %s", GetError().c_str()
	);
	
	CheckError(
		synth.Open(),
		"Could not initialize Synthesizer: %s\n", GetError().c_str()
	);
	
	synth.Play();
	
	SDL_Log("Playing concert A at 440Hz...\n");
	synth.AddVoice(note_e::A, 5);
	
	if (do_audio_sample_test) {
		SDL_Log("Testing audio sample...\n");
		Test();
	}
	
	if (do_time_logging) {
		for (int i = 1; i <= 20; ++i) {
			SDL_Delay(100);
			SDL_Log("%dms\n", i * 100);
		}
	} else {
		SDL_Delay(2000);
	}
	
	synth.RemoveVoice(1);
	SDL_Delay(1000);
	
	if (do_midi_test) {
		SDL_Log("Playing MIDI...\n");
		midi.Play(synth);
	}
	
	synth.Close();
	
	SDL_Log("Finished!\n");
	
	return 0;
}
