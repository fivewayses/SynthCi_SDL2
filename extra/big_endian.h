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
#include <cstring>

inline constexpr bool IsLittleEndian()
{
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
	return __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
#else
	// Fallback compile-time check
	union { uint32_t i; char c[4]; } b = { 0x01020304 };
	return b.c[0] == 4;
#endif
}

template <typename T>
inline constexpr T SwapBytes(T value)
{
	T result = 0;
	constexpr size_t num_bytes = sizeof(T);

	for (size_t i = 0; i < num_bytes; ++i) {
		// Pull the lowest byte out and shift it into its mirrored destination position
		result |= ((value >> (i * 8)) & 0xFF) << ((num_bytes - 1 - i) * 8);
	}
	return result;
}

template <typename T>
inline constexpr T ReadBigEndian(const uint8_t *data)
{
	T raw_value;
	memcpy(&raw_value, data, sizeof(T));
	
	if constexpr (IsLittleEndian()) {
		return SwapBytes(raw_value);
	} else {
		return raw_value;
	};
}
