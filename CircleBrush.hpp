#pragma once

#include "Brush.hpp"

//! Loads a 31x31 cirle brush from CircleBrushMap.hpp and scales it to the
//! desired width
class CircleBrush : public Brush
{
public:
	CircleBrush(int radius);
	~CircleBrush();

	//! Inlined for speed
	unsigned char getValue(int x, int y) const
	{
		int index = ((r + y) * width) + (r + x);
		assert(index >= 0 && index < width * width);
		return weights[index];
	}

private:
	unsigned char* weights;
	int width;
};
