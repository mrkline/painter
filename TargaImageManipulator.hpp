#pragma once

#include <cassert>
#include <cstring> // For size_t

#include "TargaImage.h"

class SeperableKernel;


const size_t kPixelWidth = 4;

/*!
 * \brief This class allows for image manipulation on Targa images loaded via
 *        the TargaImage class.
 */
class TargaImageManipulator
{
public:
	enum BrushType
	{
		BR_CIRCLE,
		BR_LINE
	};

	//! Inlined since it will be used frequently
	static unsigned char* getPixel(TargaImage* image,
										  unsigned int x,
										  unsigned int y)
	{
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

	static void paint(TargaImage* image, BrushType brush, int brushRadius);
};
