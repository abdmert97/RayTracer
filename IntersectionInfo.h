#include "Scene.h"
typedef struct IntersectionInfo
{
	bool isIntersect;
	glm::vec3 intersectionPoint;
	float t;
	glm::vec3 hitNormal;
	int objectID;
	glm::vec2 textCoord;
	glm::mat3x3 TBN;
	IntersectionInfo()
	{
		isIntersect = false;
		objectID = -1;
		intersectionPoint = { 0,0,0 };
		t = -1;
		hitNormal = { 0,0,0 };
		textCoord = { -1,-1 };
		TBN = glm::mat3x3(0);
		
	}
	IntersectionInfo operator=(IntersectionInfo rhs)
	{
		isIntersect = rhs.isIntersect;
		intersectionPoint = rhs.intersectionPoint;
		t = rhs.t;
		hitNormal = rhs.hitNormal;
		objectID = rhs.objectID;
		textCoord = rhs.textCoord;
		TBN = rhs.TBN;
		return *this;
	}
} IntersectionInfo;