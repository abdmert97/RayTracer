#ifndef _BOUNDINGBOX_H_
#define _BOUNDINGBOX_H_



#include "Ray.h"
class Shape;
class Ray;
class BoundingBox
{
	public:
		glm::vec3 min;
		glm::vec3 max;
		BoundingBox()
		{
			min = glm::vec3( INT16_MAX);
			max = glm::vec3( INT16_MIN);
		
		}
		BoundingBox(BoundingBox box1, BoundingBox box2)
		{
			min = glm::vec3(getMin(box1.min, box2.min));
			max = glm::vec3(getMin(box1.max, box2.max));
		}
		/*BoundingBox(Shape* shape1, Shape* shape2)
		{
			BoundingBox * firstBound  = shape1->getBounds();
			BoundingBox * secondBound = shape2->getBounds();
			min = getMin(firstBound->min, secondBound->min);
			max = getMax(firstBound->max, secondBound->max);
		}*/
		void extend(BoundingBox *boundingBox);
		void extend(Shape * shape);
		static glm::vec3 getMin(glm::vec3 v1, glm::vec3 v2);
		static glm::vec3 getMax(glm::vec3 v1, glm::vec3 v2);
		float isIntersect(Ray ray);
		void printBounds();
		glm::vec3 midPoint();
};
#endif

