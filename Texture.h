#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include  "Scene.h"
// shuffle algorithm example

#include <algorithm>    // std::shuffle
#include <array>        // std::array
#include <random>       // std::default_random_engine
#include <functional>

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
				size_t index = 3 * (i * height + j);
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
	int PCount[16] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

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
		if(texture_type == Perlin)
		{
			shuffleG();
		}
	}

	glm::vec3 getTextureColor(glm::vec2 textCoord, glm::vec3 hitPoint =glm::vec3(0))
	{
		if (textureType == Perlin)
			return getPerlinColor(textCoord,hitPoint);
		return getStandardTextColor(textCoord);
	}
	float perlinColor(glm::vec3 iP)
	{
		float value = 0;


		iP *= noiseScale;
		glm::vec3 p0 = glm::vec3(floor(iP.x), floor(iP.y), floor(iP.z));
		glm::vec3 p1 = glm::vec3(floor(iP.x + 1), floor(iP.y), floor(iP.z));
		glm::vec3 p2 = glm::vec3(floor(iP.x), floor(iP.y + 1), floor(iP.z));
		glm::vec3 p3 = glm::vec3(floor(iP.x + 1), floor(iP.y + 1), floor(iP.z));
		glm::vec3 p4 = glm::vec3(floor(iP.x), floor(iP.y), floor(iP.z + 1));
		glm::vec3 p5 = glm::vec3(floor(iP.x + 1), floor(iP.y), floor(iP.z + 1));
		glm::vec3 p6 = glm::vec3(floor(iP.x), floor(iP.y + 1), floor(iP.z + 1));
		glm::vec3 p7 = glm::vec3(floor(iP.x + 1), floor(iP.y + 1), floor(iP.z + 1));
		glm::vec3 v0 = iP - p0;
		glm::vec3 v1 = iP - p1;
		glm::vec3 v2 = iP - p2;
		glm::vec3 v3 = iP - p3;
		glm::vec3 v4 = iP - p4;
		glm::vec3 v5 = iP - p5;
		glm::vec3 v6 = iP - p6;
		glm::vec3 v7 = iP - p7;


		value = glm::dot(v0, randomVectors(p0)) * W(v0.x) * W(v0.y) * W(v0.z) +
			glm::dot(v1, randomVectors(p1)) * W(v1.x) * W(v1.y) * W(v1.z) +
			glm::dot(v2, randomVectors(p2)) * W(v2.x) * W(v2.y) * W(v2.z) +
			glm::dot(v3, randomVectors(p3)) * W(v3.x) * W(v3.y) * W(v3.z) +
			glm::dot(v4, randomVectors(p4)) * W(v4.x) * W(v4.y) * W(v4.z) +
			glm::dot(v5, randomVectors(p5)) * W(v5.x) * W(v5.y) * W(v5.z) +
			glm::dot(v6, randomVectors(p6)) * W(v6.x) * W(v6.y) * W(v6.z) +
			glm::dot(v7, randomVectors(p7)) * W(v7.x) * W(v7.y) * W(v7.z);

		return value;
	}
	float perlinColorConverted(glm::vec3 iP)
	{
		float value = 0;


		iP *= noiseScale;
		glm::vec3 p0 = glm::vec3(floor(iP.x), floor(iP.y), floor(iP.z));
		glm::vec3 p1 = glm::vec3(floor(iP.x + 1), floor(iP.y), floor(iP.z));
		glm::vec3 p2 = glm::vec3(floor(iP.x), floor(iP.y + 1), floor(iP.z));
		glm::vec3 p3 = glm::vec3(floor(iP.x + 1), floor(iP.y + 1), floor(iP.z));
		glm::vec3 p4 = glm::vec3(floor(iP.x), floor(iP.y), floor(iP.z + 1));
		glm::vec3 p5 = glm::vec3(floor(iP.x + 1), floor(iP.y), floor(iP.z + 1));
		glm::vec3 p6 = glm::vec3(floor(iP.x), floor(iP.y + 1), floor(iP.z + 1));
		glm::vec3 p7 = glm::vec3(floor(iP.x + 1), floor(iP.y + 1), floor(iP.z + 1));
		glm::vec3 v0 = iP - p0;
		glm::vec3 v1 = iP - p1;
		glm::vec3 v2 = iP - p2;
		glm::vec3 v3 = iP - p3;
		glm::vec3 v4 = iP - p4;
		glm::vec3 v5 = iP - p5;
		glm::vec3 v6 = iP - p6;
		glm::vec3 v7 = iP - p7;


		value = glm::dot(v0, randomVectors(p0)) * W(v0.x) * W(v0.y) * W(v0.z) +
			glm::dot(v1, randomVectors(p1)) * W(v1.x) * W(v1.y) * W(v1.z) +
			glm::dot(v2, randomVectors(p2)) * W(v2.x) * W(v2.y) * W(v2.z) +
			glm::dot(v3, randomVectors(p3)) * W(v3.x) * W(v3.y) * W(v3.z) +
			glm::dot(v4, randomVectors(p4)) * W(v4.x) * W(v4.y) * W(v4.z) +
			glm::dot(v5, randomVectors(p5)) * W(v5.x) * W(v5.y) * W(v5.z) +
			glm::dot(v6, randomVectors(p6)) * W(v6.x) * W(v6.y) * W(v6.z) +
			glm::dot(v7, randomVectors(p7)) * W(v7.x) * W(v7.y) * W(v7.z);
		if (noiseConversion == Absval)
		{
			value = abs(value);
			
		}
		else
		{
			value = (value + 1.0) / 2.0;
		}
		return value;
	}

private:

	vector<glm::vec3> G = { glm::vec3(1,1,0),glm::vec3(-1,1,0),
			            glm::vec3(1,-1,0),glm::vec3(-1,-1,0),glm::vec3(1,0,1),glm::vec3(-1,0,1),
			            glm::vec3(1,0,-1),glm::vec3(-1,0,-1),glm::vec3(0,1,1),glm::vec3(0,-1,1),
			            glm::vec3(0,1,-1),glm::vec3(0,-1,-1),glm::vec3(1,1,0),glm::vec3(-1,1,0),
			            glm::vec3(0,-1,1),glm::vec3(0,-1,-1) };
	int P[16] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15 };
	
	void shuffleG()
	{
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(std::begin(P), std::end(P), default_random_engine(g));
	}
	static float W(float x)
	{
		x = abs(x);
		return -6 * pow(x, 5) + 15 * pow(x, 4) - 10 * pow(x, 3) + 1;
	}
	static void printVector(glm::vec3 vect, const char* optional = nullptr)
	{
		if (optional != nullptr)
			cout << optional;
		cout << "x: " << vect.x << " y: " << vect.y << " z: " << vect.z << endl;
	}
	
	
	glm::vec3 randomVectors(glm::vec3 hitpoint)
	{
		int x = hitpoint.x;
		int y = hitpoint.y;
		int z = hitpoint.z;
	
		int index = P[ abs((x + P[ abs(y+P[abs(z %16)]) %16]) %16) ];
		PCount[index]++;
		return G[index];
	}

	
	glm::vec3 bilinearInterpolation(const glm::vec2& textCoord)
	{
		
		glm::vec2 pixelPos = glm::vec2((textCoord.x) * texture->width, textCoord.y * texture->height);
	
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

	glm::vec3 getPerlinColor(const glm::vec2& textCoord,glm::vec3 hitPoint)
	{
		
		float color = perlinColor(hitPoint);
		//if(color!= 0)
		//	cout << color << endl;
		
		return glm::vec3(color);
	}

	
};



#endif
