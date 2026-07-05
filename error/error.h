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

void PushError(
	const char *msg,
	int64_t p1 = 0, int64_t p2 = 0,
	int64_t p3 = 0, int64_t p4 = 0
)
{
	error_t err = { msg, p1, p2, p3, p4, true };
	
	// Shift the error stack.
	for (int i = ERROR_STACK_SIZE - 1; i > 0; --i) {
		error_t prev = error_stack[i-1];
		if (!prev.valid) return;
		
		error_stack[i] = prev;
	}
	
	error_stack[0] = err;
}

void ClearError()
{
	error_t &err = error_stack[0];
	int32_t i = 0;
	
	while (err.valid) {
		err.valid = false;
		err = error_stack[++i];
	}
}

uint32_t GetNumErrors()
{
	int32_t i = 0;
	while (error_stack[i].valid) ++i;
	
	return i;
}

inline void RedirectSDLError()
{
	PushError(SDL_GetError());
}
