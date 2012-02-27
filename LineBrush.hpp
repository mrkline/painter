#pragma once

#include "Brush.hpp"

class LineBrush : public Brush
{
public:
	LineBrush(int radius);
	~LineBrush();

	float getValue(int x, int y) const
	{ return weights[((r + y) * width) + r + x]; }

private:
	float* weights;
	unsigned int width;
};
