#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <cstdio>
#include <cstdlib>
#include "glm/vec3.hpp"


//
// Access the color using either individual component names
// or the channel array.
//
typedef union Color
{
    struct
    {
        unsigned int red;
        unsigned int grn;
        unsigned int blu;
    };
	Color operator+(Color c)
	{
		Color retVal = { red + c.red,
						grn + c.grn,
						blu + c.blu };
		return retVal;
	}
	Color operator+(glm::vec3 colorVector)
	{
		Color color = { red,grn,blu };
			color.red += (unsigned int)colorVector.x;
			color.grn += (unsigned int)colorVector.y;
			color.blu += (unsigned int)colorVector.z;
		return color;
	}
	Color operator*(float c)
	{

		return Color{ (unsigned int)(red * c),(unsigned int)(grn * c),(unsigned int)(blu * c) };
	}

    unsigned int channel[3];
} Color;

/* This class is provided to you for defining an image as a variable, manipulate it easily, and save it as a ppm file. */
class Image
{
public:
    Color** data;                   // Image data
	int width;						// Image width
	int height;						// Image height

	Image(int width, int height);	// Constructor
	void setPixelValue(int col, int row, const Color& color); // Sets the value of the pixel at the given column and row
	void saveImage(const char *imageName) const;	          // Takes the image name as a file and saves it as a ppm file. 
};

#endif
