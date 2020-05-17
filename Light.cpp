#include "Light.h"

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
