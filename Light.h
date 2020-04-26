#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "Scene.h"


using namespace std;

// Class for point lights
class PointLight
{
public:	
	glm::vec3 position;	// Position of the point light

    PointLight(const glm::vec3& position, const glm::vec3& intensity);	// Constructor
	glm::vec3 computeLightContribution(const glm::vec3& p); // Compute the contribution of light at point p

private:

	glm::vec3 intensity;	// Intensity of the point light
};


#endif
