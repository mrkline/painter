#pragma once

#include "Brush.hpp"

class CircleBrush : public Brush
{
public:
	CircleBrush(int radius);
	~CircleBrush();

	unsigned char getValue(int x, int y) const
	{ return weights[((r + y) * width) + r + x]; }

private:
	unsigned char* weights;
	unsigned int width;
};
