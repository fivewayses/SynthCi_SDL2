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
#include <cstdio>
#include <string>
#include <SDL2/SDL_error.h>

#define ERROR_STACK_SIZE 16

struct error_info_t {
	const char *msg;
	int64_t p1, p2, p3, p4;
	bool valid = false;
};

struct error_globals_t {
	error_info_t err_stack[ERROR_STACK_SIZE];
	uint32_t num_errors = 0;
};

inline error_globals_t *GetErrorGlobals() {
	// This makes sure error_globals shares
	// the same reference to all .cpp files.
	static error_globals_t error_globals {};
	return &error_globals;
}

inline std::string GetError(int32_t index = -1)
{
	error_globals_t *errg = GetErrorGlobals();
	error_info_t *error_stack = errg->err_stack;
	uint32_t num_errors = errg->num_errors;
	uint32_t true_index = 0;

	if (index > 0 && index <= num_errors) {
		true_index = index - 1;
	} else if (index < 0 && index >= -(int32_t)num_errors) {
		true_index = num_errors + index;
	} else {
		return "";
	}

	error_info_t &err = error_stack[true_index];
	if (!err.valid) return "";

	// String size including NULL-terminator.
	size_t required_size = 1 + snprintf(
		NULL, 0, err.msg,
		err.p1, err.p2, err.p3, err.p4
	);
	
	std::string err_buffer(required_size, '\0');
	sprintf(
		err_buffer.data(), err.msg,
		err.p1, err.p2, err.p3, err.p4
	);

	return err_buffer;
}

inline void PushError(
	const char *msg,
	int64_t p1 = 0, int64_t p2 = 0,
	int64_t p3 = 0, int64_t p4 = 0
)
{
	error_globals_t *errg = GetErrorGlobals();
	error_info_t *error_stack = errg->err_stack;
	error_info_t err = { msg, p1, p2, p3, p4, true };
	
	if (errg->num_errors >= ERROR_STACK_SIZE) {
		// Shift the error stack to get space.
		for (int i = 0; i < ERROR_STACK_SIZE - 1; ++i) {
			error_stack[i] = error_stack[i+1];
		}
	} else {
		++errg->num_errors;
	}
	
	error_stack[errg->num_errors - 1] = err;
}

inline void ClearError()
{
	error_globals_t *errg = GetErrorGlobals();
	// Due to how arenas work, we just need
	// to set the size to 0.
	errg->num_errors = 0;
}

inline uint32_t GetNumErrors()
{
	error_globals_t *errg = GetErrorGlobals();
	return errg->num_errors;
}

inline void RedirectSDLError()
{
	PushError(SDL_GetError());
}
