#include "Scene.h"
typedef struct IntersectionInfo
{
	bool isIntersect;
	glm::vec3 intersectionPoint;
	float t;
	glm::vec3 hitNormal;
	int objectID;
	glm::vec2 textCoord;

	IntersectionInfo()
	{
		isIntersect = false;
		objectID = -1;
		intersectionPoint = { 0,0,0 };
		t = -1;
		hitNormal = { 0,0,0 };
		textCoord = { -1,-1 };
	}
	IntersectionInfo operator=(IntersectionInfo rhs)
	{
		isIntersect = rhs.isIntersect;
		intersectionPoint = rhs.intersectionPoint;
		t = rhs.t;
		hitNormal = rhs.hitNormal;
		objectID = rhs.objectID;
		textCoord = rhs.textCoord;
		return *this;
	}
} IntersectionInfo;