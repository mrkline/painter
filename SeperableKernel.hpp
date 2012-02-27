#pragma once

//! An interface for a floating point kernel for performing image convolutions
class SeperableKernel
{
public:
	/*!
	 * \brief Constructs a kernel with a given radius
	 * \param radiusX the X-axis radius of the kernel (width is radiusX*2 + 1)
	 * \param radiusY the Y-axis radius of the kernel (height is radiusY*2 + 1)
	 */
	SeperableKernel(int radiusX, int radiusY)
			: rx(radiusX), ry(radiusY) { }

	int getRowRadius() const { return rx; }

	int getColumnRadius() const { return ry; }

	virtual float getRowElement(int x) const = 0;
	virtual float getColumnElement(int y) const = 0;

protected:
	const int rx;
	const int ry;
};

