#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include  "Scene.h"
class Texture
{
public:
	int id;
	int height;
	int width;
	std::vector<glm::vec3> color;


	Texture(int id,const int height, const int width, unsigned char* data)
		: id(id),height(height),
		  width(width)
	{

		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				// read rgb
				size_t index = 3 * (j * width + i);
				glm::vec3 indexColor = {
					static_cast<int>(data[index + 0]),
					static_cast<int>(data[index + 1]),
					static_cast<int>(data[index + 2])
				};
				color.push_back(indexColor);
			}
		}
	}
	glm::vec3 getTextureColor(int x,int y)
	{
		size_t index = 3 * (y * width + x);
		return color[index];
	}
};




#endif
