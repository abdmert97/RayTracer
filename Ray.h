#ifndef _RAY_H_
#define _RAY_H_
#include "Scene.h"


// Class for Ray object. You will implement the methods
class Ray
{
public:
	glm::vec3 origin;	// Origin of the ray
	glm::vec3 direction;	// Direction of the ray
	glm::vec3 p;
	Ray();	// Constuctor
	Ray(const glm::vec3& origin, const glm::vec3& direction);	// Constuctor

	glm::vec3 getPoint(float t) const;       // Return the point along the ray at ray parameter t
	float gett(const glm::vec3& p) const;   // Return the t parameter at point p along the ray

private:
	// Write any other stuff here. 
};

#endif

