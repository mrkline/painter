#pragma once

#include <vector>

#include "SeperableKernel.hpp"

//! Provides a Gaussian kernel, where pixels are weighted using a
//! standard normal distribution curve.
class GaussianKernel : public SeperableKernel
{
public:
	GaussianKernel(int radius);

	float getRowElement(int x) const { return values[rx + x]; }
	float getColumnElement(int y) const { return values[ry + y]; }

private:
	std::vector<float> values;
};
