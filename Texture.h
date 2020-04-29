#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include  "Scene.h"

class Scene;
extern Scene* pScene;
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
		
		size_t index = (y * width + x);

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
	
	Texture* texture;
	TextureType textureType;
	InterpolationType interpolationType;
	DecalMode decalMode;
	int normalizer;
	float bumpFactor;
	NoiseConversion noiseConversion;
	float noiseScale;


	TextureMap(const int id, Texture* texture, const TextureType texture_type,
		const InterpolationType interpolation_type, const DecalMode decal_mode, const int normalizer,
		const float bump_factor, const NoiseConversion noise_conversion, const float noise_scale)
		: id(id),
		  texture(texture),
		  textureType(texture_type),
		  interpolationType(interpolation_type),
		  decalMode(decal_mode),
		  normalizer(normalizer),
		  bumpFactor(bump_factor),
		  noiseConversion(noise_conversion),
		  noiseScale(noise_scale)
	{
	}

	glm::vec3 getTextureColor(glm::vec2 textCoord)
	{
		if (textureType == Perlin)
			return getPerlinColor(textCoord);
		return getStandardTextColor(textCoord);
	}
private:

	

	glm::vec3 bilinearInterpolation(const glm::vec2& textCoord)
	{
		
		glm::vec2 pixelPos = glm::vec2(textCoord.y * texture->width, textCoord.x * texture->height);
	
		int p = (int)pixelPos.x;
		int q = (int)pixelPos.y;
		
		float dx = pixelPos.x - p;
		float dy = pixelPos.y - q;
		return texture->getTextureColor(p, q)*(1 - dx)*(1 - dy) +
			   texture->getTextureColor(p + 1, q)*(dx)*(1 - dy) +
			   texture->getTextureColor(p, q + 1)*(1 - dx)*(dy)+
			   texture->getTextureColor(p + 1, q + 1)*(dx)*(dy);
	}

	glm::vec3 nearestInterpolation(const glm::vec2& textCoord)
	{
		glm::vec2 pixelPos = glm::vec2(textCoord.x * texture->width, textCoord.y * texture->height);
		int p = (int)pixelPos.x;
		int q = (int)pixelPos.y;
		return texture->getTextureColor(p, q);
	}

	glm::vec3 getStandardTextColor(const glm::vec2& textCoord)
	{
		switch (interpolationType)
		{
		case Bilinear:
			return bilinearInterpolation(textCoord);
			break;
		case Nearest:
			return nearestInterpolation(textCoord);
			break;
		}
	}

	glm::vec3 getPerlinColor(const glm::vec2& vec)
	{
		return glm::vec3(0);
	}

	
};



#endif
