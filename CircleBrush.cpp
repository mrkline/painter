#include "CircleBrushMap.hpp"

#include <cstdlib>

#include "AreaResize.hpp"
#include "CircleBrush.hpp"
#include "TargaImageManipulator.hpp"
#include "TargaImage.h"

CircleBrush::CircleBrush(int radius)
	: Brush(radius), width(radius * 2 + 1)
{
	unsigned int area = width * width;
	weights = (unsigned char*)malloc(area);
	// Load up our map into a Targa so we can scale it using the scaling code
	// we've already written.
	TargaImage* map = TargaImage::blankImage(CircleBrushMap.width,
	                  CircleBrushMap.height);
	memcpy(map->pixels(),
	       CircleBrushMap.pixel_data,
	       CircleBrushMap.width * CircleBrushMap.height
	       * CircleBrushMap.bytes_per_pixel);
	// Scale the map
	TargaImage* xScaledMap = areaResize(map, width, DIM_WIDTH);
	TargaImage* scaledMap = areaResize(xScaledMap, width, DIM_HEIGHT);
	delete map;
	delete xScaledMap;
	// We only care about the alpha component
	unsigned char* scaledMapPixel = scaledMap->pixels();
	for (unsigned int y = 0; y < width; ++y)
		for (unsigned int x = 0; x < width; ++x, scaledMapPixel += kPixelWidth)
			weights[y * width + x] = scaledMapPixel[3];

	delete scaledMap;
}

CircleBrush::~CircleBrush()
{
	free(weights);
}
