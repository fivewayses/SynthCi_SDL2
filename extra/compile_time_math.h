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

#define PI 3.14159265358979323846f

constexpr float CompileTimePow(float base, int16_t exp)
{
	if (exp == 0) return 1.0f;
	if (exp < 0) return 1.0f / CompileTimePow(base, -exp);
	
	float result = 1.0f;
	float current_product = base;
	
	while (exp > 0) {
		if (exp % 2 == 1) result *= current_product;
		current_product *= current_product;
		exp /= 2;
	}
	
	return result;
}

constexpr float CompileTimeSin(float x) {
    // Normalize x to [-pi, pi]
    while (x > PI)  x -= 2 * PI;
    while (x < -PI) x += 2 * PI;

    // Taylor series expansion
    float term = x;
    float sum = x;
    float x2 = x * x;

    term *= -x2 / (2 * 3);
    sum += term;
    term *= -x2 / (4 * 5);
    sum += term;
    term *= -x2 / (6 * 7);
    sum += term;
    term *= -x2 / (8 * 9);
    sum += term;

    return sum;
}
