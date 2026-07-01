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
