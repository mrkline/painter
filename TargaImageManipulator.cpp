#include "TargaImageManipulator.hpp"

#include <algorithm>
#include <cassert>
#include <memory> // for auto_ptr

#include "TargaImage.h"
#include "SeperableKernel.hpp"
#include "CircleBrush.hpp"
#include "LineBrush.hpp"

//! clamps a value to the [0..1] range
static inline float clamp(float x)
{
	return std::max(0.0f, std::min(1.0f, x));
}

void TargaImageManipulator::convolve(TargaImage* image,
                                     const SeperableKernel& k)
{
	// Get total kernel weight (used later for renormalization)
	float kernelWeight = 0.0f;
	for (int y = -k.getColumnRadius(); y <= k.getColumnRadius(); ++y) {
		for (int x = -k.getRowRadius(); x <= k.getRowRadius(); ++x) {
			kernelWeight += k.getRowElement(x) * k.getColumnElement(y);
		}
	}

	int signedHeight = (int)image->height();
	int signedWidth = (int)image->width();

	std::auto_ptr<TargaImage> temp(TargaImage::blankImage(image->width(),
	                               image->height()));

	// In the first pass, convolve horizontally
	for (int y = 0; y < signedHeight; ++y) {
		for (int x = 0; x < signedWidth; ++x) {
			float usedWeight = kernelWeight;
			float sum[kPixelWidth] = {0.0f};

			for (int r = -k.getRowRadius(); r <= k.getRowRadius(); ++r) {
				// If the pixel is off the image, remove its weight from
				// usedWeight
				if (x + r < 0 || x + r >= signedWidth) {
					usedWeight -= k.getRowElement(-r);
				}
				// Otherwise, sample the pixel based on the kernel's weight
				else {
					unsigned char* pix = getPixel(image, x + r, y);
					float weight = k.getRowElement(-r);
					for (size_t c = 0; c < kPixelWidth; ++c)
						sum[c] += ((float)pix[c] / 255.0f) * weight;
				}
			}

			// Renormalize the sum and write it back out.
			unsigned char* pixOut = getPixel(temp.get(), x, y);
			for (size_t c = 0; c < kPixelWidth; ++c) {
				sum[c] /= usedWeight;
				// Ensure the color is within the valid [0..1] space
				sum[c] = clamp(sum[c]);
				pixOut[c] = (unsigned char)(sum[c] * 255.0f);
			}
		}
	}

	// Copy the temporary back into the main image
	size_t imageLen = image->width() * image->height() * kPixelWidth;
	memcpy(image->pixels(), temp->pixels(), imageLen);

	// In the second pass, convolve vertically
	for (int y = 0; y < signedHeight; ++y) {
		for (int x = 0; x < signedWidth; ++x) {
			float usedWeight = kernelWeight;
			float sum[kPixelWidth] = {0.0f};

			for (int r = -k.getColumnRadius(); r <= k.getColumnRadius(); ++r) {
				// If the pixel is off the image, remove its weight from
				// usedWeight
				if (y + r < 0 || y + r >= signedHeight) {
					usedWeight -= k.getRowElement(-r);
				}
				// Otherwise, sample the pixel based on the kernel's weight
				else {
					unsigned char* pix = getPixel(image, x, y + r);
					float weight = k.getColumnElement(-r);
					for (size_t c = 0; c < kPixelWidth; ++c)
						sum[c] += ((float)pix[c] / 255.0f) * weight;
				}
			}

			// Renormalize the sum and write it back out.
			unsigned char* pixOut = getPixel(temp.get(), x, y);
			for (size_t c = 0; c < kPixelWidth; ++c) {
				sum[c] /= usedWeight;
				// Ensure the color is within the valid [0..1] space
				sum[c] = clamp(sum[c]);
				pixOut[c] = (unsigned char)(sum[c] * 255.0f);
			}
		}
	}

	// Copy the temporary back into the main image
	imageLen = image->width() * image->height() * kPixelWidth;
	memcpy(image->pixels(), temp->pixels(), imageLen);
}


void TargaImageManipulator::paint(TargaImage* image,
		TargaImageManipulator::BrushType brush,
		int bRadius)
{
	LineBrush(2);
}
