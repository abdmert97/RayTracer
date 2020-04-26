#include "BoundingBox.h"

void BoundingBox::extend(BoundingBox* boundingBox)
{
	min = getMin(min, boundingBox->min);
	max = getMax(max, boundingBox->max);
}

void BoundingBox::extend(Shape* shape)
{
	BoundingBox* boundingBox = shape->getBounds();
	min = getMin(min, boundingBox->min);
	max = getMax(max, boundingBox->max);
}

glm::vec3  BoundingBox::getMin(glm::vec3 v1, glm::vec3 v2)
{
	return glm::vec3(
		v1.x < v2.x ? v1.x : v2.x,
		v1.y < v2.y ? v1.y : v2.y,
		v1.z < v2.z ? v1.z : v2.z
	);
}
glm::vec3 BoundingBox::getMax(glm::vec3 v1, glm::vec3 v2)
{
	return glm::vec3(
		v1.x < v2.x ? v2.x : v1.x,
		v1 .y < v2.y ? v2.y : v1.y,
		v1.z < v2.z ? v2.z : v1.z
	);
}

template <typename T>
void SetMax(T& val0, const T& val1) { val0 = val0 < val1 ? val1 : val0; }

template<typename T>
void SetMin(T& val0, const T& val1) { val0 = val0 > val1 ? val1 : val0; }

float BoundingBox::isIntersect(Ray r)
{
	
	// Initialize with min and max values
	float tn = 0; // Initialize with 0 since rays go only forward
	float tf = std::numeric_limits<float>::max(); // Initialize with inf 
	// --

	// Check for x 
	float tnn = (min.x - r.origin.x) / r.direction.x;

	float tff = (max.x - r.origin.x) / r.direction.x;

	if (tnn > tff)
		std::swap(tnn, tff);

	SetMax(tn, tnn);
	SetMin(tf, tff);

	if (tf < tn)
		return -1;
	// --

	// Check for y
	tnn = (min.y - r.origin.y) / r.direction.y;
	tff = (max.y - r.origin.y) / r.direction.y;

	if (tnn > tff)
		std::swap(tnn, tff);

	SetMax(tn, tnn);
	SetMin(tf, tff);

	if (tf < tn)
		return -1;
	// --

	// Check for z
	tnn = (min.z - r.origin.z) / r.direction.z;
	tff = (max.z - r.origin.z) / r.direction.z;

	if (tnn > tff)
		std::swap(tnn, tff);

	SetMax(tn, tnn);
	SetMin(tf, tff);

	if (tf < tn)
		return -1;
	// -- 

	return tn;
}


void BoundingBox::printBounds()
{
	//cout << "min: " << min << endl;
	//cout << "max: " << max << endl;
}

glm::vec3 BoundingBox::midPoint()
{
	glm::vec3 midp = glm::vec3(min.x / 2 + max.x / 2, min.y / 2 + max.y / 2, min.z / 2 + max.z / 2);
	return midp;
}


