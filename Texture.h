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
enum NoiseConversion
{
	Absval,
	Linear
};
enum TextureType
{
	StandardTexture,
	Perlin
};
enum InterpolationType
{
	Bilinear,
	Nearest
};
enum DecalMode
{
	ReplaceKD,
	ReplaceNormal,
	ReplaceBG,
	ReplaceALL,
	BlendKD,
	BumpNormal
};
class TextureMap
{

public:
	int id;
	int TextureID;
	TextureType textureType;
	InterpolationType interpolationType;
	DecalMode decalMode;
	int normalizer;
	float bumpFactor;
	NoiseConversion noiseConversion;
	float noiseScale;


	TextureMap(const int id, const int texture_ýd, const TextureType texture_type,
		const InterpolationType interpolation_type, const DecalMode decal_mode, const int normalizer,
		const float bump_factor, const NoiseConversion noise_conversion, const float noise_scale)
		: id(id),
		  TextureID(texture_ýd),
		  textureType(texture_type),
		  interpolationType(interpolation_type),
		  decalMode(decal_mode),
		  normalizer(normalizer),
		  bumpFactor(bump_factor),
		  noiseConversion(noise_conversion),
		  noiseScale(noise_scale)
	{
	}
};



#endif
