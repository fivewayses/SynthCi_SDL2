#pragma once

#include <cstdint>

template <typename T>
static T ReadVLQ(const uint8_t *data, uint32_t &offset)
{
	T value = 0;
	uint8_t byte;
	
	do {
		byte = data[offset++];
		value = (value << 7) | (byte & 0x7f);
	} while (byte & 0x80);
	
	return value;
}
