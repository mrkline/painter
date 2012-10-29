#pragma once

#include <cassert>

//! Provides a brush interface which returns an alpha mask value at a given
//! x-y coordinate
class Brush
{
public:
	Brush(int radius)
		: r(radius)
	{ assert(r <= 15); }

	virtual ~Brush() { }

	int getRadius() const { return r; }

	virtual unsigned char getValue(int x, int y) const = 0;

protected:
	const int r;
};
