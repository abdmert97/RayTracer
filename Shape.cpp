#include "Shape.h"
#include <stack>
#define PI 3.14159265

extern Scene *pScene;
Shape::Shape(void)
{

}

void Shape::initTransformatrix()
{
	if (transformations.size() == 0)return;
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


void Mesh::face(int start ,int end)
{
	for (int i = start; i < end; i++)
	{
		Triangle* tris = faces[i];
		tris->normal1 = glm::vec3(0);
		tris->normal2 = glm::vec3(0);
		tris->normal3 = glm::vec3(0);
		int count1 = 0;
		int count2 = 0;
		int count3 = 0;


		for (int j = 0; j < faces.size(); j++)
		{
			Triangle* trisCheck = faces[j];
			if (tris->point1 == trisCheck->point1 || tris->point1 == trisCheck->point2 || tris->point1 == trisCheck->point3)
			{
				tris->normal1 += trisCheck->getNormal();
				count1++;
			}
			if (tris->point2 == trisCheck->point1 || tris->point2 == trisCheck->point2 || tris->point2 == trisCheck->point3)
			{
				tris->normal2 += trisCheck->getNormal();
				count2++;
			}
			if (tris->point3 == trisCheck->point1 || tris->point3 == trisCheck->point2 || tris->point3 == trisCheck->point3)
			{
				tris->normal3 += trisCheck->getNormal();
				count3++;
			}

		}

		tris->normal1 /= count1;
		tris->normal2 /= count2;
		tris->normal3 /= count3;

	}
}
void Mesh::calculateFaceNormals()
{
	cout << faces.size() << endl;
	int size = faces.size();
	std::thread td0(&Mesh::face, this,0,size/12 );
	std::thread td1(&Mesh::face, this,(size)/12, (size*2) / 12);
	std::thread td2(&Mesh::face, this, (size*2) / 12, (size*3) / 12);
	std::thread td3(&Mesh::face, this, (size*3) / 12, (size*4) / 12);
	std::thread td4(&Mesh::face, this, (size*4) / 12, (size*5) / 12);
	std::thread td5(&Mesh::face, this, (size*5) / 12, (size*6) / 12);
	std::thread td6(&Mesh::face, this, (size*6) / 12, (size*7) / 12);
	std::thread td7(&Mesh::face, this, (size*7) / 12, (size*8) / 12);
	std::thread td8(&Mesh::face, this, (size*8) / 12, (size*9) / 12);
	std::thread td9(&Mesh::face, this, (size*9) / 12, (size*10) / 12);
	std::thread tda(&Mesh::face, this, (size*10) / 12, (size*11) / 12);
	std::thread tdb(&Mesh::face, this, (size*11) / 12, (size*12) /12 );
	td0.join();

	td1.join();

	td2.join();

	td3.join();

	td4.join();

	td5.join();

	td6.join();

	td7.join();

	td8.join();

	td9.join();

	tda.join();

	tdb.join();
	
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
	textPoint1 = 0;
	textPoint2 = 0;
	textPoint3 = 0;
	first = false;
}

/* Triangle-ray intersection routine. You will implement this.
Note that IntersectionInfo structure should hold the information related to the intersection point, e.g., coordinate of that point, normal at that point etc.
You should to declare the variables  in IntersectionInfo structure you think you will need. It is in defs.h file. */

glm::vec3 Triangle::getNormal()
{
	if(first)
	{
		return faceNormal;
	}
	glm::vec3 point1vec = pScene->vertices[point1];
	glm::vec3 point2vec = pScene->vertices[point2];
	glm::vec3 point3vec = pScene->vertices[point3];
	glm::vec3 crossProduct = glm::cross((point2vec - point1vec), (point3vec - point1vec));
	glm::vec3 normal = glm::normalize(crossProduct);
	faceNormal = normal;
	first = true;
	return normal;
}

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
		float cosi = y/radius;

		float theta = acosf(cosi);
		float fi = atan2(-z, x);
		
		returnValue.textCoord.x = (fi + PI) / (2 * PI);
		returnValue.textCoord.y = theta/PI;
		
		if (pScene->textureMaps[textureIndex]->decalMode == ReplaceNormal || pScene->textureMaps[textureIndex]->decalMode == BumpNormal ||
			(textureIndex2 != -1 && (pScene->textureMaps[textureIndex2]->decalMode == ReplaceNormal || pScene->textureMaps[textureIndex2]->decalMode == BumpNormal)))
		{
			glm::vec3 tangent, bitangent;
			tangent = glm::vec3(-radius*cos(theta)*sin(fi), 0, radius * cos(theta)*cos(fi));
	
			bitangent = glm::cross(returnValue.hitNormal, tangent);
			if(pScene->textureMaps[textureIndex]->decalMode == ReplaceNormal)
			{
				tangent = glm::normalize(tangent);
				bitangent = glm::normalize(bitangent);
			}

			returnValue.TBN = glm::mat3x3(tangent.x, bitangent.x, returnValue.hitNormal.x,
				tangent.y, bitangent.y, returnValue.hitNormal.y,
				tangent.z, bitangent.z, returnValue.hitNormal.z
			);
			
		}
		if ((textureIndex != -1 && pScene->textureMaps[textureIndex]->textureType == Perlin) ||
			textureIndex2 != -1 && pScene->textureMaps[textureIndex2]->textureType == Perlin)
		{
			TextureMap* map = pScene->textureMaps[textureIndex];
			if (textureIndex2 != -1 && pScene->textureMaps[textureIndex2]->textureType == Perlin)
				map = pScene->textureMaps[textureIndex2];

			float eps = 0.001;
			float x = map->perlinColorConverted(returnValue.intersectionPoint + glm::vec3(eps, 0, 0)) - map->perlinColorConverted(returnValue.intersectionPoint);
			float y = map->perlinColorConverted(returnValue.intersectionPoint + glm::vec3(0, eps, 0)) - map->perlinColorConverted(returnValue.intersectionPoint);
			float z = map->perlinColorConverted(returnValue.intersectionPoint + glm::vec3(0, 0, eps)) - map->perlinColorConverted(returnValue.intersectionPoint);


			glm::vec3 gradient = glm::vec3(x / eps, y / eps, z / eps);

			float gP = glm::dot(gradient, returnValue.hitNormal);

			glm::vec3 newGradient = gradient - glm::dot(gradient, returnValue.hitNormal) * returnValue.hitNormal;

			returnValue.TBN = glm::mat3x3(newGradient.x, newGradient.x, returnValue.hitNormal.x,
				newGradient.y, newGradient.y, returnValue.hitNormal.y,
				newGradient.z, newGradient.z, returnValue.hitNormal.z
			);
		}
		

	
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
		if (shadingMode == SmoothShading)
		{
			
			returnValue.hitNormal = glm::normalize((1-beta-gamma)*normal1+ beta*normal2+gamma*normal3);
		}
		else
		{
			glm::vec3 crossProduct = getNormal();
			returnValue.hitNormal = glm::normalize(crossProduct);
		}


		if (transformations.size() != 0)
			returnValue.hitNormal = glm::normalize(transformNormal(returnValue.hitNormal));

		
		if(textureIndex != -1&& pScene->textureMaps[textureIndex]->textureType == StandardTexture)
		{
	
			glm::vec2 textCoord1 = pScene->textCoord[textPoint1];
			glm::vec2 textCoord2 = pScene->textCoord[textPoint2];
			glm::vec2 textCoord3 = pScene->textCoord[textPoint3];
			
			returnValue.textCoord = (1 - beta - gamma) * textCoord1 + beta * textCoord2 + gamma * textCoord3;
			
			if (pScene->textureMaps[textureIndex]->decalMode == ReplaceNormal || pScene->textureMaps[textureIndex]->decalMode == BumpNormal||
				(textureIndex2 != -1 &&(pScene->textureMaps[textureIndex2]->decalMode == ReplaceNormal || pScene->textureMaps[textureIndex2]->decalMode == BumpNormal)))
			{
				
				glm::vec3 edge1 = point2vec - point1vec;
				glm::vec3 edge2 = point3vec - point1vec;
				glm::vec2 deltaUV1 = textCoord2 - textCoord1;
				glm::vec2 deltaUV2 = textCoord3 - textCoord1;
				float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
				glm::vec3 tangent, bitangent;

				
				tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
				tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
				tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
			

				bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
				bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
				bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
			
			
				if (pScene->textureMaps[textureIndex]->decalMode == ReplaceNormal)
				{
					tangent = glm::normalize(tangent);
					bitangent = glm::normalize(bitangent);
				}
			
				returnValue.TBN = glm::mat3x3( tangent.x,bitangent.x,returnValue.hitNormal.x ,
									tangent.y,bitangent.y,returnValue.hitNormal.y ,
									tangent.z,bitangent.z,returnValue.hitNormal.z
				);
			
			}
			
		}
		if ((textureIndex != -1 && pScene->textureMaps[textureIndex]->textureType == Perlin) ||
			textureIndex2 != -1 && pScene->textureMaps[textureIndex2]->textureType == Perlin)
		{
			TextureMap* map = pScene->textureMaps[textureIndex];
			if(textureIndex2 != -1 && pScene->textureMaps[textureIndex2]->textureType == Perlin)
				 map = pScene->textureMaps[textureIndex2];
		
			float eps = 0.001;
			float x = map->perlinColorConverted(returnValue.intersectionPoint + glm::vec3(eps, 0, 0)) - map->perlinColorConverted(returnValue.intersectionPoint);
			float y = map->perlinColorConverted(returnValue.intersectionPoint + glm::vec3(0, eps, 0)) - map->perlinColorConverted(returnValue.intersectionPoint);
			float z = map->perlinColorConverted(returnValue.intersectionPoint + glm::vec3(0, 0, eps)) - map->perlinColorConverted(returnValue.intersectionPoint);


			glm::vec3 gradient = glm::vec3(x/eps,y/eps,z/eps);
		
			float gP = glm::dot(gradient, returnValue.hitNormal);
			
			glm::vec3 newGradient =gradient - glm::dot(gradient, returnValue.hitNormal)* returnValue.hitNormal;

			returnValue.TBN = glm::mat3x3(newGradient.x, newGradient.x, returnValue.hitNormal.x,
				newGradient.y, newGradient.y, returnValue.hitNormal.y,
				newGradient.z, newGradient.z, returnValue.hitNormal.z
			);
		}
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


