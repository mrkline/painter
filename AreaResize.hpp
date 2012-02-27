#pragma once

enum Dimension {
	DIM_WIDTH,
	DIM_HEIGHT
};

class TargaImage;

TargaImage* areaResize(TargaImage* original,
                       unsigned int newSize,
                       Dimension dim);
