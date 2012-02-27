#pragma once

#include "Brush.hpp"

class LineBrush : public Brush
{
public:
	LineBrush(int radius);
	~LineBrush();

	unsigned char getValue(int x, int y) const
	{ return weights[((r + y) * width) + r + x]; }

private:
	unsigned char* weights;
	unsigned int width;
};
