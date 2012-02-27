#pragma once

#include "Brush.hpp"

class CircleBrush : public Brush
{
public:
	CircleBrush(int radius);
	~CircleBrush();

	float getValue(int x, int y) const
	{ return weights[((r + y) * width) + r + x]; }

private:
	float* weights;
	unsigned int width;
};
