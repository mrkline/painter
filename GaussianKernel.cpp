#include "GaussianKernel.hpp"

#include <cmath>

static const float sqrt2 = sqrt(2.0f);

/*!
 * \brief An implementation of the error function, since MSVC++ 2010 doesn't
 *        have C99 support
 * Found from http://stackoverflow.com/a/6281146/713961
 */
float erf(float x)
{
    // constants
    float a1 =  0.254829592f;
    float a2 = -0.284496736f;
    float a3 =  1.421413741f;
    float a4 = -1.453152027f;
    float a5 =  1.061405429f;
    float p  =  0.3275911f;

    // Save the sign of x
    int sign = 1;
    if (x < 0)
        sign = -1;
    x = fabs(x);

    // A&S formula 7.1.26
    float t = 1.0f /(1.0f + p*x);
    float y = 1.0f - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*exp(-x*x);

    return sign*y;
}

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
