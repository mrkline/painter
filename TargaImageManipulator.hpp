#pragma once

#include <cassert>
#include <cstring> // For size_t

#include "TargaImage.h"

class SeperableKernel;

//! Width of a Targa pixel. It is in 32-bit RGBA format.
const size_t kPixelWidth = 4;

//! This class allows for image manipulation on Targa images loaded via
//! the TargaImage class.
class TargaImageManipulator
{
public:
	enum BrushType {
	    BR_CIRCLE,
	    BR_LINE
	};

	//! Inlined since it will be used frequently
	static unsigned char* getPixel(TargaImage* image,
	                               unsigned int x,
	                               unsigned int y) {
		assert(x >= 0);
		assert(x < image->width());
		assert(y >= 0);
		assert(y < image->height());
		size_t pitch = image->width() * kPixelWidth;
		return image->pixels() + y * pitch + x * kPixelWidth;
	}

	//! Convolves a TargaImage with a given kernel. Kernel renormalization is
	//! performed when needed along image boundaries
	static void convolve(TargaImage* image, const SeperableKernel& k);

	//! Given a brush type and radius, apply a painting algorithm to an image
	static void paint(TargaImage* image, BrushType brush, int brushRadius);

private:
	//! Paints a single layer onto canvas using reference to determine desired
	//! brush colors for this layer.
	static void paintLayer(TargaImage* canvas, TargaImage* reference,
	                       BrushType brush, int radius);
};
