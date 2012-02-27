#pragma once

enum Dimension {
    DIM_WIDTH,
    DIM_HEIGHT
};

class TargaImage;

//! This function shrinks an image by summing the colors that fall within the
//! region of the source image that correlates to each pixel in the resulting
//! image.
TargaImage* areaResize(TargaImage* original,
                       unsigned int newSize,
                       Dimension dim);
