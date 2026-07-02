#include "audio/midi/Note.h"
#include "audio/midi/Synthesizer.h"
#include "audio/midi/MIDIData.h"
#include <SDL2/SDL_error.h>
#include <SDL2/SDL_log.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>

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
	
	CheckError(
		synth.Open(),
		"Could not initialize Synthesizer: %s\n", SDL_GetError()
	);
	
	synth.Play();
	
	synth.AddVoice(note_e::C);
	SDL_Delay(250);
	
	synth.AddVoice(note_e::E);
	SDL_Delay(250);
	
	synth.AddVoice(note_e::G);
	SDL_Delay(250);
	
	synth.AddVoice(note_e::C, 5);
	SDL_Delay(250);
	
	synth.RemoveVoice(4);
	SDL_Delay(250);
	
	synth.RemoveVoice(3);
	SDL_Delay(250);
	
	synth.RemoveVoice(2);
	SDL_Delay(250);
	
	synth.RemoveVoice(1);
	SDL_Delay(250);
	
	synth.Close();
	
	SDL_Log("Finished!\n");
	
	return 0;
}
