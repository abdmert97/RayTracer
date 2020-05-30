#include "Light.h"
#define PI 3.14159265
/* Constructor. Implemented for you. */
PointLight::PointLight(const glm::vec3 & position, const glm::vec3 & intensity)
    : position(position), intensity(intensity)
{
}


// Compute the contribution of light at point p using the
// inverse square law formula
glm::vec3 PointLight::computeLightContribution(const glm::vec3& p)
{
	

	glm::vec3 distance = p-position;
    float distanceSquare =  distance.x*distance.x+
                            distance.y*distance.y+
                            distance.z*distance.z;

    return intensity*(1.0f/distanceSquare);
}

glm::vec3 DirectionalLight::computeLightContribution(const glm::vec3& p)
{
	return radiance;
}

glm::vec3 SpotLight::computeLightContribution(const glm::vec3& p)
{
	glm::vec3 objectVector = glm::normalize(p - position);
	float cost =glm::dot(direction, objectVector);
	float theta = acos(cost) * 57.2957795;

	if (theta > coverageAngle)
	{
		return glm::vec3(0);
	}
	if(theta< fallofAngle)
	{
		glm::vec3 distance = p - position;
		float distanceSquare = distance.x * distance.x +
			distance.y * distance.y +
			distance.z * distance.z;

		return intensity * (1.0f / distanceSquare);
	}
	
	glm::vec3 distance = p - position;
	float distanceSquare = distance.x * distance.x +
		distance.y * distance.y +
		distance.z * distance.z;

	float f = (cost- cos(coverageAngle* 0.0174532925))/ (cos(fallofAngle* 0.0174532925) - cos(coverageAngle* 0.0174532925));

	
	f = pow(f, 6);

	return intensity * (1.0f / distanceSquare)*f;
	
	
}
float randomTime()
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	uniform_real_distribution<> dis(0, 1);
	return dis(gen);
}
glm::vec3 AreaLight::computeLightContribution(const glm::vec3& p)
{
	
	float r1 = randomTime() * size -0.5;
	float r2 = randomTime() * size -0.5;
	
	glm::vec3 randomPos = defaultpos+u*r1+v*r2;
	position = randomPos;
	glm::vec3 distance = p - randomPos;
	glm::vec3 objectVector = glm::normalize(distance);
	float cost =abs(glm::dot(normal, objectVector));
	float distanceSquare = distance.x * distance.x +
		distance.y * distance.y +
		distance.z * distance.z;
	glm::vec3 mult = ((size * size * cost) / distanceSquare)* radiance;

	return mult;
}
float randomEnv()
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	uniform_real_distribution<> dis(-1, 1);
	return dis(gen);
}
glm::vec3 EnviromentLight::computeLightContribution(const glm::vec3& p)
{

	glm::vec3 l = glm::vec3(randomEnv(), randomEnv(), randomEnv());
	while(glm::length(l) >1 || glm::dot(l, normal) <= 0)
		l = glm::vec3(randomEnv(), randomEnv(), randomEnv());
	if (glm::length(l) <= 1 && glm::dot(l, normal) > 0)
	{
		glm::vec3 u = glm::vec3(0, -normal.z, normal.y);
		glm::vec3 w = glm::vec3(normal.y * normal.y + normal.z * normal.z, -1 * normal.x * normal.y, -1 * normal.x * normal.z);
		float theta = acos(glm::dot(l, w));
		float fi = atan(glm::dot(l, w) / glm::dot(l, u));
		glm::vec2 textCoord = glm::vec2((fi + PI) / (2 * PI), theta / PI);
		float pi2 = 2 * PI;
		float x = 200;
		position = p + l * x;
		glm::vec2 textCoordClamped = glm::vec2(textCoord.x - (int)textCoord.x, textCoord.y - (int)textCoord.y);

		glm::vec2 pixelPos = glm::vec2(textCoordClamped.x * texture->width, textCoordClamped.y * texture->height);
		int p = (int)pixelPos.x;
		int q = (int)pixelPos.y;
		glm::vec3 color = texture->getTextureColor(p, q) * pi2;

		return	color;


	}
	
	
}
