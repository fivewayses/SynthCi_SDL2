#pragma once

#include <cstdint>
#include "extra/compile_time_math.h"

inline constexpr int WAVEFORM_SAMPLE_SIZE = 2048;
inline constexpr int WAVEFORM_SAMPLE_HALF_SIZE = WAVEFORM_SAMPLE_SIZE / 2;
inline constexpr int WAVEFORM_SAMPLE_QUAD_SIZE = WAVEFORM_SAMPLE_HALF_SIZE / 2;
inline constexpr int WAVEFORM_SAMPLE_3_QUAD_SIZE = 3 * WAVEFORM_SAMPLE_QUAD_SIZE;
inline constexpr int WAVEFORM_SAMPLE_DIV = 65536 / WAVEFORM_SAMPLE_SIZE;

struct waveform_sample_t {
	int8_t data[WAVEFORM_SAMPLE_SIZE];
};

constexpr waveform_sample_t GenerateSquareSample()
{
	waveform_sample_t sample {};
	
	for (int32_t i = 0; i < WAVEFORM_SAMPLE_HALF_SIZE; ++i) {
		sample.data[i] = 112;
	}
	for (int32_t i = WAVEFORM_SAMPLE_HALF_SIZE; i < WAVEFORM_SAMPLE_SIZE; ++i) {
		sample.data[i] = -112;
	}
	
	return sample;
}

constexpr waveform_sample_t GenerateSawtoothSample()
{
	waveform_sample_t sample {};
	
	for (int32_t i = 0; i < WAVEFORM_SAMPLE_SIZE; ++i) {
		sample.data[i] = (i - WAVEFORM_SAMPLE_HALF_SIZE) / 288;
	}
	
	return sample;
}

constexpr waveform_sample_t GenerateTriangleSample()
{
	waveform_sample_t sample {};
	
	for (int32_t i = 0; i < WAVEFORM_SAMPLE_HALF_SIZE; ++i) {
		sample.data[i] = (i - WAVEFORM_SAMPLE_QUAD_SIZE) / 144;
	}
	for (int32_t i = WAVEFORM_SAMPLE_HALF_SIZE; i < WAVEFORM_SAMPLE_SIZE; ++i) {
		sample.data[i] = (WAVEFORM_SAMPLE_3_QUAD_SIZE - i) / 144;
	}
	
	return sample;
}

constexpr waveform_sample_t GenerateSineSample()
{
	waveform_sample_t sample {};
	
	for (int32_t i = 0; i < WAVEFORM_SAMPLE_HALF_SIZE; ++i) {
		int8_t res = 112.0f * CompileTimeSin(i * PI / (float)WAVEFORM_SAMPLE_HALF_SIZE);
		sample.data[i] = res;
		sample.data[i + WAVEFORM_SAMPLE_HALF_SIZE] = -res;
	}
	
	return sample;
}

inline constexpr waveform_sample_t SQUARE_SAMPLE   = GenerateSquareSample();
inline constexpr waveform_sample_t TRIANGLE_SAMPLE = GenerateTriangleSample();
inline constexpr waveform_sample_t SAWTOOTH_SAMPLE = GenerateSawtoothSample();
inline constexpr waveform_sample_t SINE_SAMPLE     = GenerateSineSample();
