#include "LineBrushMap.hpp"

#include <cstdlib>

#include "AreaResize.hpp"
#include "LineBrush.hpp"
#include "TargaImageManipulator.hpp"
#include "TargaImage.h"

LineBrush::LineBrush(int radius)
	: Brush(radius), width(radius * 2 + 1)
{
	unsigned int area = width * width;
	weights = (unsigned char*)malloc(area);
	// Load up our map into a Targa so we can scale it using the scaling code
	// we've already written.
	TargaImage* map = TargaImage::blankImage(LineBrushMap.width,
	                  LineBrushMap.height);
	memcpy(map->pixels(),
	       LineBrushMap.pixel_data,
	       LineBrushMap.width * LineBrushMap.height
	       * LineBrushMap.bytes_per_pixel);
	// Scale the map
	TargaImage* xScaledMap = areaResize(map, width, DIM_WIDTH);
	TargaImage* scaledMap = areaResize(xScaledMap, width, DIM_HEIGHT);
	delete map;
	delete xScaledMap;
	// We only care about the alpha component
	unsigned char* scaledMapPixel = scaledMap->pixels();
	for (int y = 0; y < width; ++y)
		for (int x = 0; x < width; ++x, scaledMapPixel += kPixelWidth)
			weights[y * width + x] = scaledMapPixel[3];

	delete scaledMap;
}

LineBrush::~LineBrush()
{
	free(weights);
}
