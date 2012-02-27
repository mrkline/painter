#include "GaussianKernel.hpp"

#include <cmath>

static const float sqrt2 = sqrt(2.0f);

/*!
 * \brief Calculates the integral of a standard normal distribution between
 *        two points.
 *
 * The integral of a standard normal distribution (a normalized Gaussian curve)
 * is 1/2 erf(x/sqrt(2)) + 1. We can simplify a bit to speed up the calculation.
 */
static inline float sndIntegral(float a, float b)
{
	return 0.5f * (erf(b / sqrt2) - erf(a / sqrt2));
}

GaussianKernel::GaussianKernel(int radius)
	: SeperableKernel(radius, radius)
{
	int width = radius * 2 + 1;

	// The function has values significantly greater from zero from about
	// -4 to 4. We want to map the pixels to that range
	float x = -4.0f;
	float dx = 8.0f / (float)(width);

	for (int c = 0; c < width; ++c, x += dx) {
		// The integral of the standard normal distribution from x to x + dx
		values.push_back(sndIntegral(x, x + dx));
	}
}
