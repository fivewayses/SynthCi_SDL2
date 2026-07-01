#pragma once

#include <array>
#include <cstdint>
#include "../../extra/compile_time_math.h"

constexpr std::array<int8_t, 65536> GenerateSquareSample()
{
	std::array<int8_t, 65536> sample {};
	
	for (int32_t i = 0; i < 32768; ++i) {
		sample[i] = 112;
	}
	for (int32_t i = 32768; i < 65536; ++i) {
		sample[i] = -112;
	}
	
	return sample;
}

constexpr std::array<int8_t, 65536> GenerateSawtoothSample()
{
	std::array<int8_t, 65536> sample {};
	
	for (int32_t i = 0; i < 65536; ++i) {
		sample[i] = (i - 32768) / 288;
	}
	
	return sample;
}

constexpr std::array<int8_t, 65536> GenerateTriangleSample()
{
	std::array<int8_t, 65536> sample {};
	
	for (int32_t i = 0; i < 32768; ++i) {
		sample[i] = (i - 16384) / 144;
	}
	for (int32_t i = 32768; i < 65536; ++i) {
		sample[i] = (49152 - i) / 144;
	}
	
	return sample;
}

constexpr std::array<int8_t, 65536> GenerateSineSample()
{
	std::array<int8_t, 65536> sample {};
	
	for (int32_t i = 0; i < 32768; ++i) {
		int8_t res = 112.0f * CompileTimeSin(i * PI / 32768.0f);
		sample[i] = res;
		sample[i+32768] = -res;
	}
	
	return sample;
}

inline constexpr std::array<int8_t, 65536> SQUARE_SAMPLE   = GenerateSquareSample();
inline constexpr std::array<int8_t, 65536> TRIANGLE_SAMPLE = GenerateTriangleSample();
inline constexpr std::array<int8_t, 65536> SAWTOOTH_SAMPLE = GenerateSawtoothSample();
inline constexpr std::array<int8_t, 65536> SINE_SAMPLE     = GenerateSineSample();
