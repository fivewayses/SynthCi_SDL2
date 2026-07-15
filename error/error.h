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

#include <cstdint>
#include <cstdio>
#include <string>
#include <SDL2/SDL_error.h>

#define ERROR_STACK_SIZE 16

struct error_info_t {
	const char *msg;
	int64_t p1, p2, p3, p4;
	bool valid = false;
};

static error_info_t error_stack[ERROR_STACK_SIZE];

static std::string GetError(uint32_t index = 0)
{
	if (index >= ERROR_STACK_SIZE) return "";
	
	error_info_t &err = error_stack[index];
	if (!err.valid) return "";
	
	char err_buffer[256];
	snprintf(
		err_buffer, sizeof(err_buffer), err.msg,
		err.p1, err.p2, err.p3, err.p4
	);
	
	return err_buffer;
}

static void PushError(
	const char *msg,
	int64_t p1 = 0, int64_t p2 = 0,
	int64_t p3 = 0, int64_t p4 = 0
)
{
	error_info_t err = { msg, p1, p2, p3, p4, true };
	
	// Shift the error stack.
	for (int i = ERROR_STACK_SIZE - 1; i > 0; --i) {
		error_info_t prev = error_stack[i-1];
		if (!prev.valid) return;
		
		error_stack[i] = prev;
	}
	
	error_stack[0] = err;
}

static void ClearError()
{
	error_info_t &err = error_stack[0];
	int32_t i = 0;
	
	while (err.valid) {
		err.valid = false;
		err = error_stack[++i];
	}
}

static uint32_t GetNumErrors()
{
	int32_t i = 0;
	while (error_stack[i].valid) ++i;
	
	return i;
}

inline void RedirectSDLError()
{
	PushError(SDL_GetError());
}
