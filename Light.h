#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "Scene.h"

class Texture;

using namespace std;

// Class for point lights
class Light
{
public:
	virtual glm::vec3 computeLightContribution(const glm::vec3& p) = 0;
};
class PointLight:Light
{
public:	
	glm::vec3 position;	// Position of the point light

    PointLight(const glm::vec3& position, const glm::vec3& intensity);	// Constructor
	glm::vec3 computeLightContribution(const glm::vec3& p); // Compute the contribution of light at point p

private:

	glm::vec3 intensity;	// Intensity of the point light
};
class DirectionalLight :Light
{
public:
	glm::vec3 direction;
	glm::vec3 radiance;

	DirectionalLight(const glm::vec3& direction, const glm::vec3& radiance)
		:
		  radiance(radiance)
	{
		this->direction = normalize(direction);
	}
	glm::vec3 computeLightContribution(const glm::vec3& p);

private:
};
class SpotLight:  Light
{
public:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 intensity;
	float coverageAngle;
	float fallofAngle;

	SpotLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& intensity,
		const float coverage_angle, const float fallof_angle)
		: position(position),
		  
		  intensity(intensity),
		  coverageAngle(coverage_angle),
		  fallofAngle(fallof_angle)
	{
		this->direction = normalize(direction);
	}
	glm::vec3 computeLightContribution(const glm::vec3& p);
};
class AreaLight: Light
{
public:
	glm::vec3 position;
	glm::vec3 defaultpos;
	glm::vec3 normal;
	glm::vec3 radiance;
	glm::vec3 u ;
	glm::vec3 v ;
	float size;

	AreaLight(const glm::vec3& position, const glm::vec3& normal, const glm::vec3& radiance, const float size)
		: position(position),
		  
		  radiance(radiance),
		  size(size)
	{
		this->normal = glm::normalize(normal);
		defaultpos = position;
		u = glm::vec3(0, -normal.z, normal.y);
		
		v = glm::vec3(normal.y * normal.y + normal.z * normal.z, -1*normal.x * normal.y, -1*normal.x * normal.z);
	
	}

	glm::vec3 computeLightContribution(const glm::vec3& p);

	glm::vec3 glossyReflection(float size, glm::vec3 vec);
};

class EnviromentLight:Light
{
public:
	int imageID;
	Texture *texture;
	glm::vec3 position;
	glm::vec3 normal;
	EnviromentLight(int id,Texture *text)
	{
		imageID = id;
		position = glm::vec3(0);
		texture = text;
	}
	glm::vec3 computeLightContribution(const glm::vec3& p);
};

#endif
