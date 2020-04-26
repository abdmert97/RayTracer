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
