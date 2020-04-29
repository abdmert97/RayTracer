#include "Shape.h"
#include <stack>
#define PI 3.14159265

extern Scene *pScene;
Shape::Shape(void)
{

}

void Shape::initTransformatrix()
{
	inverseTransformMatrix = glm::mat4x4(0);
	inverseTransformMatrix[0][0] = 1;
	inverseTransformMatrix[1][1] = 1;
	inverseTransformMatrix[2][2] = 1;
	inverseTransformMatrix[3][3] = 1;
	
	normalTransformMatrix = glm::mat4x4(0);
	normalTransformMatrix[0][0] = 1;
	normalTransformMatrix[1][1] = 1;
	normalTransformMatrix[2][2] = 1;
	normalTransformMatrix[3][3] = 1;
	stack<std::pair<char, int>> stk;
	for (std::pair<char, int> transform : transformations)
	{
		stk.push(transform);
	}
	while (stk.empty() != true)
	{
		std::pair<char, int> transform = stk.top();
		stk.pop();
		if (transform.first == 't')
		{
			inverseTransformMatrix = multiplyMatrixWithMatrix(*pScene->translationMatrices[transform.second - 1], inverseTransformMatrix);

		}
		else if (transform.first == 's')
		{
			inverseTransformMatrix = multiplyMatrixWithMatrix(*pScene->scalingMatrices[transform.second - 1], inverseTransformMatrix);
		}
		else if (transform.first == 'r')
		{
			inverseTransformMatrix = multiplyMatrixWithMatrix(*pScene->rotationMatrices[transform.second - 1], inverseTransformMatrix);
		}
	}
	for (std::pair<char, int> transform : transformations)
	{

		if (transform.first == 't')
		{
			normalTransformMatrix = multiplyMatrixWithMatrix(*pScene->normtranslationMatrices[transform.second - 1], normalTransformMatrix);

		}
		else if (transform.first == 's')
		{
			normalTransformMatrix = multiplyMatrixWithMatrix(*pScene->normscalingMatrices[transform.second - 1], normalTransformMatrix);
		}
		else if (transform.first == 'r')
		{
			normalTransformMatrix = multiplyMatrixWithMatrix(*pScene->normrotationMatrices[transform.second - 1], normalTransformMatrix);
		}
	}

	transformMatrixTranspose = glm::mat4x4(transposeMatrix(inverseTransformMatrix));
}

Shape::Shape(int id, int matIndex, int textureIndex, int textureIndex2,Material * material, ShapeType type, vector<std::pair<char, int>> transformations, glm::vec3 motionBlur, ShadingMode shadingMode)
    : id(id), matIndex(matIndex),textureIndex(textureIndex),textureIndex2(textureIndex2) ,shapeType(type),material(material),transformations(transformations),motionBlur(motionBlur),shadingMode(shadingMode)
{
	bounds = nullptr;
	isTransformed = glm::length(motionBlur) || transformations.size() != 0 ? true : false;
	cout << textureIndex << " " << textureIndex2 << endl;
}

Sphere::Sphere(void)
{}

/* Constructor for sphere. You will implement this. */
Sphere::Sphere(int id, int matIndex, int textureIndex, int textureIndex2, Material* material, int cIndex, float R, vector<glm::vec3> *pVertices, ShapeType type, vector<std::pair<char, int>> transformations, glm::vec3 motionBlur, ShadingMode shadingMode)
    : Shape(id, matIndex,textureIndex,textureIndex2,material,type,transformations,motionBlur,shadingMode)
{
this->radius = R;
this->center = (*pVertices)[cIndex-1];
//getBounds();
}

Ray Shape::applyTransform(Ray rayTransformed) const
{
	glm::vec4 origin =  glm::vec4(rayTransformed.origin, 1);
	rayTransformed.origin = transformVector(origin);

	
	glm::vec4 direction = glm::vec4(rayTransformed.direction, 0);
	rayTransformed.direction = transformVector(direction);

	return rayTransformed;
}


/* Sphere-ray intersection routine. You will implement this.
Note that IntersectionInfo structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc.
You should to declare the variables in IntersectionInfo structure you think you will need. It is in defs.h file. */

glm::vec3 Shape::transformVector(glm::vec4 & vect)const
{
	
	return multiplyMatrixWithVec4(inverseTransformMatrix,vect);
}

glm::vec3 Shape::transformNormal(glm::vec3& vect)const
{
	return glm::normalize(multiplyMatrixWithVec4(transformMatrixTranspose,glm::vec4(vect,0)));
}
BoundingBox* Sphere::getBounds()
{
	
	if(bounds == nullptr)
	{
		
		bounds = new BoundingBox();
		glm::vec3 radiusVec = { radius,radius,radius };
		bounds->min = center - radiusVec;
		bounds->max = center + radiusVec;
		//bounds->min = transformVector(bounds->min);
		//bounds->max = transformVector(bounds->max);
	}
	return bounds;
}
BoundingBox* Triangle::getBounds()
{
	if (bounds == nullptr)
	{
		bounds = new BoundingBox();
		bounds->min = BoundingBox::getMin(pScene->vertices[point1], BoundingBox::getMin(pScene->vertices[point2], pScene->vertices[point3]));
		bounds->max = BoundingBox::getMax(pScene->vertices[point1], BoundingBox::getMax(pScene->vertices[point2], pScene->vertices[point3]));
		//bounds->min = transformVector(bounds->min);
		//bounds->max = transformVector(bounds->max);
	}
	return bounds;
}
BoundingBox* Mesh::getBounds()
{
	if (bounds == nullptr)
	{
		bounds = new BoundingBox();
		for (Triangle* triangle : faces)
		{
			BoundingBox* triangleBounds = triangle->getBounds();
			bounds->min = BoundingBox::getMin(bounds->min, triangleBounds->min);
			bounds->max = BoundingBox::getMax(bounds->max, triangleBounds->max);
			//bounds->min = transformVector(bounds->min);
			//bounds->max = transformVector(bounds->max);
		}
		boundingVolume = new BoundingVolume(faces);
	}

	return bounds;
}

Triangle::Triangle(void)
{}

/* Constructor for triangle. You will implement this. */
Triangle::Triangle(int id, int matIndex, int textureIndex, int textureIndex2, Material* material, int p1Index, int p2Index, int p3Index, vector<glm::vec3> *pVertices, ShapeType type, vector<std::pair<char, int>> transformations, glm::vec3 motionBlur, ShadingMode shadingMode)
    : Shape(id, matIndex,textureIndex,textureIndex2,material,type,transformations,motionBlur, shadingMode)
{
    point1 = p1Index-1;
    point2 = p2Index-1;
    point3 = p3Index-1;
	//getBounds();
}

/* Triangle-ray intersection routine. You will implement this.
Note that IntersectionInfo structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc.
You should to declare the variables  in IntersectionInfo structure you think you will need. It is in defs.h file. */

Mesh::Mesh()
{}

/* Constructor for mesh. You will implement this. */
Mesh::Mesh(int id, int matIndex, int textureIndex, int textureIndex2, Material* material, const vector<Triangle*>& faces, vector<int> *pIndices, vector<glm::vec3> *pVertices, ShapeType type, vector<std::pair<char, int>> transformations, glm::vec3 motionBlur, ShadingMode shadingMode)
    : Shape(id, matIndex,textureIndex,textureIndex2,material,type,transformations,motionBlur, shadingMode),faces(faces)
{
	//for (Triangle triangle : faces)
	//	triangle.getBounds();
	//getBounds();
}



IntersectionInfo Sphere::intersect(const Ray& ray, Ray* rayTransformed) 
{

	IntersectionInfo returnValue = {};
	returnValue.isIntersect = false;

	glm::vec3 direction =ray.direction;
	glm::vec3 origin    =ray.origin;
	center = isTransformed ? multiplyMatrixWithVec4(normalTransformMatrix, glm::vec4(center, 1)) : center;
	radius = isTransformed ? glm::length(multiplyMatrixWithVec4(normalTransformMatrix, glm::vec4(radius,0,0, 0))):radius;
	isTransformed = false;
	float t = dot(direction * glm::vec3(-1), (origin - center));
	//dummy variables
	float x = glm::dot(direction, (origin - center));
	float y = glm::dot(direction, direction);
	float z = glm::dot((origin - center), (origin - center));
	float delta = x * x - y * (z - radius * radius);

	if (delta < 0 || t + sqrt(delta) <= 0)
		return returnValue;


	
	float intersectionPoint1 = t + sqrt(delta);
	float intersectionPoint2 = t - sqrt(delta);
	float intersectiont = intersectionPoint2 > 0 ? intersectionPoint2 : intersectionPoint1;
	//float intersectionPoint = intersectionPoint1 < intersectionPoint2 ? intersectionPoint1 : intersectionPoint2;
	returnValue.isIntersect = true;
	returnValue.intersectionPoint =  ray.getPoint(intersectiont);
	returnValue.t = intersectiont;
	returnValue.objectID = id;
	returnValue.hitNormal = glm::normalize ((ray.getPoint(intersectiont) - center));

	if(textureIndex != -1)
	{
		glm::vec3 hitPoint = (returnValue.intersectionPoint - center);
	
		x = hitPoint.x;
		y = hitPoint.y;
		z = hitPoint.z;
		float cos = -y/radius;

		float theta = acosf(cos);
		float fi = atan2(-z, x);
		
		returnValue.textCoord.x = (fi + PI) / (2 * PI);
		returnValue.textCoord.y = theta/PI;


	
	}
	
	//if(transformations.size() != 0)
	//{
	//	returnValue.hitNormal = transformNormal(returnValue.hitNormal);
	//}

	//    cout <<ray.getPoint(intersectionPoint1)<<" " << ray.getPoint(intersectionPoint2)<<endl ;
	return returnValue;
}
IntersectionInfo Triangle::intersect(const Ray& ray, Ray* rayTransformed) 
{
	IntersectionInfo returnValue = {};
	returnValue.isIntersect = false;
	returnValue.objectID = -1;
	glm::vec3 point1vec = pScene->vertices[point1];
	glm::vec3 point2vec = pScene->vertices[point2];
	glm::vec3 point3vec = pScene->vertices[point3];


	glm::vec3 rayDirection = rayTransformed->direction;
	glm::vec3 rayOrigin	   = rayTransformed->origin;

	float AMatrix[3][3] = {

			{point1vec.x - point2vec.x, point1vec.x - point3vec.x, rayDirection.x},
			{point1vec.y - point2vec.y, point1vec.y - point3vec.y, rayDirection.y},
			{point1vec.z - point2vec.z, point1vec.z - point3vec.z, rayDirection.z}
	};
	float determinantA = determinant(AMatrix);
	if (determinantA == 0)
	{
		returnValue.isIntersect = false; // maybe wrong;
		return returnValue;
	}
	float betaMatrix[3][3] = {
			{point1vec.x - rayOrigin.x, point1vec.x - point3vec.x, rayDirection.x},
			{point1vec.y - rayOrigin.y, point1vec.y - point3vec.y, rayDirection.y},
			{point1vec.z - rayOrigin.z, point1vec.z - point3vec.z, rayDirection.z}

	};
	float beta = determinant(betaMatrix) / determinantA;
	if (beta < 0) return returnValue;
	float gammaMatrix[3][3] = {
			{point1vec.x - point2vec.x, point1vec.x - rayOrigin.x, rayDirection.x},
			{point1vec.y - point2vec.y, point1vec.y - rayOrigin.y, rayDirection.y},
			{point1vec.z - point2vec.z, point1vec.z - rayOrigin.z, rayDirection.z}

	};
	float gamma = determinant(gammaMatrix) / determinantA;
	if (gamma < 0) return returnValue;
	float tMatrix[3][3] = {
			{point1vec.x - point2vec.x, point1vec.x - point3vec.x, point1vec.x - rayOrigin.x},
			{point1vec.y - point2vec.y, point1vec.y - point3vec.y, point1vec.y - rayOrigin.y},
			{point1vec.z - point2vec.z, point1vec.z - point3vec.z, point1vec.z - rayOrigin.z}

	};

	float t = determinant(tMatrix) / determinantA;
	if (beta + gamma <= 1 && t > 0.001)
	{
		returnValue.objectID = id;
		returnValue.isIntersect = true;
		returnValue.t = t;
		returnValue.intersectionPoint = ray.getPoint(t);
		if(textureIndex != -1)
		{
			glm::vec2 textCoord1 = pScene->textCoord[point1];
			glm::vec2 textCoord2 = pScene->textCoord[point2];
			glm::vec2 textCoord3 = pScene->textCoord[point3];
			returnValue.textCoord = (1 - beta - gamma) * textCoord1 + beta * textCoord2 + gamma * textCoord3;
		}
		
		if (shadingMode == SmoothShading)
		{
			glm::vec3 crossProduct = glm::cross((point2vec - point1vec), (point3vec - point1vec));
			returnValue.hitNormal = glm::normalize(crossProduct);
		}
		else
		{
			glm::vec3 crossProduct = glm::cross((point2vec - point1vec), (point3vec - point1vec));
			returnValue.hitNormal = glm::normalize(crossProduct);
		}
	
	
		if (transformations.size() != 0)
			returnValue.hitNormal = glm::normalize(transformNormal(returnValue.hitNormal));
	
	}

	return returnValue;
}


void Mesh::MeshVolumeIntersection(const Ray& ray, Node* node, IntersectionInfo* intersecion_info, Ray* rayTransformed)
{
	
	if(pScene->transformationEnabled)
	{
		
		float t_int = node->boundingBox->isIntersect(*rayTransformed);
		if (t_int != -1 && t_int <= intersecion_info->t)
		{

			if (node->left == nullptr && node->right == nullptr)
			{
				Triangle *shape = faces[node->ObjectIDs[0]];
				if (shape->bounds == nullptr)
					shape->getBounds();

				if (shape->bounds->isIntersect(*rayTransformed) <= intersecion_info->t)
				{
					float t_min = intersecion_info->t;


					IntersectionInfo intesectionInfo = shape->intersect(ray, rayTransformed);
					if (intesectionInfo.isIntersect == true)
					{
						if (intesectionInfo.t <= t_min)
						{
							t_min = intesectionInfo.t;
							intesectionInfo.objectID = node->ObjectIDs[0];
							*intersecion_info = intesectionInfo;
						}
					}
				}
			}
			if (node->left != nullptr)
			{
				MeshVolumeIntersection(ray, node->left, intersecion_info, rayTransformed);
			}
			if (node->right != nullptr)
			{
				MeshVolumeIntersection(ray, node->right, intersecion_info, rayTransformed);
			}
		}
	}
	else
	{
		float t_int = node->boundingBox->isIntersect(*rayTransformed);
		if (t_int != -1 && t_int <= intersecion_info->t)
		{

			if (node->left == nullptr && node->right == nullptr)
			{
				Triangle *shape = faces[node->ObjectIDs[0]];
				if (shape->bounds == nullptr)
					shape->getBounds();

				if (shape->bounds->isIntersect(*rayTransformed) <= intersecion_info->t)
				{
					float t_min = intersecion_info->t;


					IntersectionInfo intesectionInfo = shape->intersect(ray,rayTransformed);
					if (intesectionInfo.isIntersect == true)
					{
						if (intesectionInfo.t <= t_min)
						{
							t_min = intesectionInfo.t;
							intesectionInfo.objectID = node->ObjectIDs[0];
							*intersecion_info = intesectionInfo;
						}
					}
				}
			}
			if (node->left != nullptr)
			{
				MeshVolumeIntersection(ray, node->left, intersecion_info, rayTransformed);
			}
			if (node->right != nullptr)
			{
				MeshVolumeIntersection(ray, node->right, intersecion_info, rayTransformed);
			}
		}
	}
	

}
/* Mesh-ray intersection routine. You will implement this.
Note that IntersectionInfo structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc.
You should to declare the variables in IntersectionInfo structure you think you will need. It is in defs.h file. */
IntersectionInfo Mesh::intersect(const Ray & ray, Ray* rayTransformed) 
{

	IntersectionInfo returnValue{};

	returnValue.t = INT16_MAX;
	Node* node = boundingVolume->root;
	
	MeshVolumeIntersection(ray, node, &returnValue,rayTransformed);
	return  returnValue;

	

}


