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
	const uint32_t size = 1024;
	const char *test = reinterpret_cast<char *>(synth.TestAudio(size));
	
	std::fstream stream("testing.bin", std::ios::out | std::ios::binary);
	CheckError(stream.is_open(), "Can't open file [ERROR: %s]\n", strerror(errno));
	
	stream.write(test, size);
	
	return 0;
}

int main(int argc, char **argv)
{
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
	SDL_Delay(1000);
	
	synth.RemoveVoice(1);
	SDL_Delay(1000);
	
	SDL_Log("Playing MIDI...\n");
	midi.Play(synth);
	
	synth.Close();
	
	SDL_Log("Finished!\n");
	
	return 0;
}
