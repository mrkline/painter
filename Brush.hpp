#pragma once

#include <cassert>

class Brush
{
public:
	Brush(int radius)
		: r(radius)
	{ assert(r <= 30); }
	int getRadius() const { return r; }

	virtual unsigned char getValue(int x, int y) const = 0;

protected:
	const int r;
};
