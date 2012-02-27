#include "AreaResize.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>

#include "TargaImage.h"
#include "TargaImageManipulator.hpp"

//! Calculates the per-pixel component of an area
static inline float areaInPixel(float start, float end, unsigned int pixel)
{
	// The left boundary is either the starting boundary of the region
	// or the pixel, whichever comes second
	float left = std::max(start, (float)pixel);
	// The right boundary is either the ending boundary of the region
	// or the pixel, whichever comes first
	float right = std::min(end, (float)(pixel + 1));
	return right - left;
}

TargaImage* areaResize(TargaImage* original,
                       unsigned int newSize,
                       Dimension dim)
{
	// Validate dim
	assert(dim == DIM_WIDTH || dim == DIM_HEIGHT);

	unsigned int oldSize = dim == DIM_WIDTH ?
	                       original->width() : original->height();

	// We only want to use this for downsscaling.
	// We have better algorithms for upscaling.
	assert(newSize < oldSize);

	// Ratio of the new size to the old size
	float oldToNew = (float)oldSize / (float)newSize;

	// Fill in the new image
	TargaImage* ret;
	if (dim == DIM_WIDTH) {
		ret = TargaImage::blankImage(newSize, original->height());
	}
	else {
		ret = TargaImage::blankImage(original->width(), newSize);
	}

	unsigned char* cp = ret->pixels(); // current pixel
	for(unsigned int y = 0; y < ret->height(); ++y) {
		for (unsigned int x = 0; x < ret->width(); ++x, cp += kPixelWidth) {
			// We want to sum everything in the region of the old image that the
			// new pixel is sampling. oldToNew is the width of said region.
			float regionStart =
			    (float)(dim == DIM_WIDTH ? x : y) * oldToNew;
			float regionEnd = regionStart + oldToNew;
			// Accumulate each channel as we move across
			// all pixels in the region
			float accumulators[kPixelWidth] = {0.0f};
			unsigned int lastPixelNum = (unsigned int)floor(regionEnd);
			for (unsigned int pixelNum = (unsigned int)floor(regionStart);
			        pixelNum <= lastPixelNum; ++pixelNum) {
				// Don't break out of image bounds
				if (dim == DIM_WIDTH) {
					if (pixelNum == original->width())
						break;
				}
				else {
					if (pixelNum == original->height())
						break;
				}

				// Sample the pixel from the original image
				unsigned char* pixel;
				if (dim == DIM_WIDTH) {
					pixel = TargaImageManipulator::getPixel(original,
							pixelNum,
							y);
				}
				else {
					pixel = TargaImageManipulator::getPixel(original,
							x,
							pixelNum);
				}
				float areaWidth = areaInPixel(regionStart, regionEnd, pixelNum);
				for (size_t c = 0; c < kPixelWidth; ++c)
					accumulators[c] += areaWidth * ((float)pixel[c] / 255.0f);
			}
			// Having accumulated all values, divide by the region width and
			// write out the result
			for (size_t c = 0; c < kPixelWidth; ++c)
				cp[c] = (unsigned char)(accumulators[c] / oldToNew * 255.0f);
		}
	}

	return ret;
}
