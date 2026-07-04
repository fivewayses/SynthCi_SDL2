#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <SDL2/SDL_error.h>

#define ERROR_STACK_SIZE 16

struct error_t {
	const char *msg;
	int64_t p1, p2, p3, p4;
	bool valid = false;
};

static error_t error_stack[ERROR_STACK_SIZE];

std::string GetError(uint32_t index = 0)
{
	if (index >= ERROR_STACK_SIZE) return "";
	
	error_t &err = error_stack[index];
	if (!err.valid) return "";
	
	char err_buffer[256];
	snprintf(
		err_buffer, sizeof(err_buffer), err.msg,
		err.p1, err.p2, err.p3, err.p4
	);
	
	return err_buffer;
}

void PushError(error_t err)
{
	err.valid = true;
	
	// Shift the error stack.
	for (int i = ERROR_STACK_SIZE - 1; i > 0; --i) {
		error_stack[i] = error_stack[i-1];
	}
	
	error_stack[0] = err;
}

inline void RedirectSDLError()
{
	PushError({ SDL_GetError() });
}
