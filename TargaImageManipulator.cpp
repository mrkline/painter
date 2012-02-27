#include "TargaImageManipulator.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib> // for srand and rand
#include <ctime> // for seeding srand
#include <memory> // for auto_ptr
#include <vector>

#include "TargaImage.h"
#include "SeperableKernel.hpp"
#include "GaussianKernel.hpp"
#include "CircleBrush.hpp"
#include "LineBrush.hpp"

// An area must exceed this normalized error per pixel to get a stroke
static const float kErrorForStroke = 0.1;

//! clamps a value to the [0..1] range
static inline float clamp(float x)
{
	return std::max(0.0f, std::min(1.0f, x));
}

static inline bool fallsInImage(TargaImage* image,
                                unsigned int x,
                                unsigned int y)
{
	return x >= 0 && x < image->width() && y >= 0 && y < image->height();
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
                                  BrushType brush,
                                  int bRadius)
{
	// Seed the random number generator used in paintLayer
	srand(time(nullptr));
	TargaImage* output = TargaImage::blankImage(image->width(),
	                     image->height());

	// Minimum radius of 2
	for (int radius = bRadius; radius > 0; --radius) {
		// Create a blurred copy of the image
		TargaImage* blurred = TargaImage::blankImage(image->width(),
		                      image->height());
		memcpy(blurred->pixels(), image->pixels(),
		       kPixelWidth * image->width() * image->height());
		convolve(blurred, GaussianKernel(radius));
		paintLayer(output, blurred, brush, radius);
		delete blurred;
	}

	// Copy back to the source Targa
	memcpy(image->pixels(), output->pixels(),
	       kPixelWidth * image->width() * image->height());
	delete output;
}


void TargaImageManipulator::paintLayer(TargaImage* canvas,
                                       TargaImage* reference,
                                       BrushType brush,
                                       int radius)
{
	printf("\t Generating strokes using a brush with a radius of %d...\n",
	       radius);
	// A stroke location, sortable by z-order
	struct StrokeLocation {
		int x, y, z;
		StrokeLocation(int X, int Y, int Z) : x(X), y(Y), z(Z) { }
		bool operator<(const StrokeLocation& o) const { return z < o.z; }
	};
	std::vector<StrokeLocation> strokes;
	// Calculate the differences at each pixel
	unsigned char* canvasPixel = canvas->pixels();
	unsigned char* referencePixel = reference->pixels();
	float differences[canvas->height()][canvas->width()];
	for (unsigned int y = 0; y < canvas->height(); ++y) {
		for (unsigned int x = 0; x < canvas->width(); ++x) {
			differences[y][x] = 0;
			for (size_t c = 0; c < kPixelWidth; ++c) {
				float diffSq = fabs((float)referencePixel[c] / 255.0f
				                    - (float)canvasPixel[c] / 255.0f);
				diffSq *= diffSq;
				differences[y][x] += diffSq;
			}
			differences[y][x] = sqrt(differences[y][x]);
			canvasPixel += kPixelWidth;
			referencePixel += kPixelWidth;
		}
	}
	// Make the grid size slightly smaller than the radius so that we get some
	// overlap
	int gridSize = radius * 2 / 3;
	// Make sure grid size is at least 1 (it will be zero if radius is 1)
	if (gridSize < 1)
		gridSize = 1;
	for (unsigned int y = 0; y  < canvas->height() + gridSize; y += gridSize) {
		for (unsigned int x = 0; x < canvas->width() + gridSize;
		        x += gridSize) {
			// Sum the error in the region around the grid point, and find the
			// pixel with the highest error
			int halfGrid = gridSize / 2;
			int gridLeft = x - halfGrid;
			int gridRight = x + halfGrid;
			int gridTop = y - halfGrid;
			int gridBottom = y + halfGrid;
			float highestError = -1.0f;
			int highestErrorX;
			int highestErrorY;
			// Total error of the area
			float areaError = 0.0f;
			// Tracks the number of pixels in the grid
			// that fall within the image
			float pixelsSummed = 0.0f;
			for (int gy = gridTop; gy <= gridBottom; ++gy) {
				for (int gx = gridLeft; gx <= gridRight; ++gx) {
					if (fallsInImage(canvas, gx, gy)) {
						pixelsSummed += 1.0f;
						areaError += differences[gy][gx];
						if (differences[gy][gx] > highestError) {
							highestError = differences[gy][gx];
							highestErrorY = gy;
							highestErrorX = gx;
						}
					}
				}
			}
			// If our area exceeded the threshold of error per pixel, it gets
			// a stroke at its highest error point
			if (areaError / pixelsSummed >= kErrorForStroke) {
				strokes.push_back(StrokeLocation(highestErrorX,
				                                 highestErrorY,
				                                 rand()));
			}
		}
	}
	// Sort the strokes by randomized z-order
	std::sort(strokes.begin(), strokes.end());

	printf("\t\tDrawing %lu strokes.\n", strokes.size());

	Brush* b;
	if (brush == BR_LINE) {
		b = new LineBrush(radius);
	}
	else {
		b = new CircleBrush(radius);
	}
	// Draw strokes.
	for (auto it = strokes.begin(); it != strokes.end(); ++it) {
		int strokeLeft = it->x - b->getRadius();
		int strokeRight = it->x + b->getRadius();
		int strokeTop = it->y - b->getRadius();
		int strokeBottom = it->y + b->getRadius();
		unsigned char* referencePixel = getPixel(reference, it->x, it->y);
		float rr = (float)referencePixel[0] / 255.0f;
		float rg = (float)referencePixel[1] / 255.0f;
		float rb = (float)referencePixel[2] / 255.0f;
		// Normalize reference pixel values
		for (int y = strokeTop; y <= strokeBottom; ++y) {
			for (int x = strokeLeft; x <= strokeRight; ++x) {
				if (fallsInImage(canvas, x, y)) {
					// Ignore the reference pixel's alpha and use that of the
					// brush
					float ra = (float)b->getValue(x - it->x, y - it->y)
					           / 255.0f;
					unsigned char* canvasPixel = getPixel(canvas, x, y);
					// Normalize canvas values
					float cr = (float)canvasPixel[0] / 255.0f;
					float cg = (float)canvasPixel[1] / 255.0f;
					float cb = (float)canvasPixel[2] / 255.0f;
					float ca = (float)canvasPixel[3] / 255.0f;
					// Alpha blend the brush stroke
					float ba = ra + ca * (1.0f - ra);
					float br = (rr * ra + cr * ca * (1.0f - ra)) / ba;
					float bg = (rg * ra + cg * ca * (1.0f - ra)) / ba;
					float bb = (rb * ra + cb * ca * (1.0f - ra)) / ba;
					// Write the pixel out
					canvasPixel[0] = (unsigned char)(br * 255.0f);
					canvasPixel[1] = (unsigned char)(bg * 255.0f);
					canvasPixel[2] = (unsigned char)(bb * 255.0f);
					canvasPixel[3] = (unsigned char)(ba * 255.0f);
				}
			}
		}
	}
	delete b;
}
