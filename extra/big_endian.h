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
