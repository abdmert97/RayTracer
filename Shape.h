#ifndef _SHAPE_H_
#define _SHAPE_H_
#include "BoundingBox.h"
#include "BoundingVolume.h"
#include "Scene.h"
#include "IntersectionInfo.h"
#include "Ray.h"
#include "Node.h"
using namespace std;
enum ShapeType
{
	TriangleShape,
	SphereType,
	MeshType
	
};
enum ShadingMode
{
	FlatShading,
	SmoothShading
};
class BoundingVolume;
class Node;
class BoundingBox;
class Ray;
class Material;
class Scene;
class TextureMap;
// Base class for any shape object
class Shape
{
public:

	int id;	        // Id of the shape
	int matIndex;	// Material index of the shape
	int textureIndex;
	int textureIndex2;
	bool isTransformed;
	
	ShapeType shapeType;
	BoundingBox* bounds;
	Material* material;
	Scene* scene;
	glm::mat4x4 inverseTransformMatrix;
	glm::mat4x4 normalTransformMatrix;
	glm::mat4x4 transformMatrixTranspose;
	glm::vec3 motionBlur;
	vector<std::pair<char,int>> transformations;
	Ray applyTransform(Ray rayTransformed) const;
	glm::vec3 transformVector(glm::vec4& vect)const;
	glm::vec3 transformNormal(glm::vec3& vect)const;
	ShadingMode shadingMode;
	virtual IntersectionInfo intersect(const Ray & ray, Ray *rayTransformed = nullptr)  = 0; // Pure virtual method for intersection test. You must implement this for sphere, triangle, and mesh.
	virtual BoundingBox* getBounds()= 0;
    Shape(void);
	void initTransformatrix();
	Shape(int id, int matIndex,int textureIndex, int textureIndex2, Material* material, ShapeType type, vector<std::pair<char, int>> transformations, glm::vec3 motionBlur, ShadingMode shadingMode); // Constructor
	static float determinant(float matrix[3][3])
	{
		float a = matrix[0][0];
		float b = matrix[1][0];
		float c = matrix[2][0];
		float d = matrix[0][1];
		float e = matrix[1][1];
		float f = matrix[2][1];
		float g = matrix[0][2];
		float h = matrix[1][2];
		float i = matrix[2][2];


		return a * (e * i - h * f) + b * (g * f - d * i) + c * (d * h - e * g);
	}
	glm::vec4  static multiplyMatrixWithVec4(glm::mat4x4 m, glm::vec4 v)
	{
		double values[4];
		double total;

		for (int i = 0; i < 4; i++)
		{
			total = 0;
			for (int j = 0; j < 4; j++)
			{
				total += m[i][j] * v[j];
			}
			values[i] = total;
		}

		return glm::vec4(values[0], values[1], values[2], values[3]);
	}
	
	glm::mat4x4 static multiplyMatrixWithMatrix(glm::mat4x4 m, glm::mat4x4 m2)
	{
		double values[4];
		double total;
		glm::mat4x4 result = glm::mat4x4(0);
		for (int i = 0; i < 4; i++)
		{
			total = 0;
			for (int j = 0; j < 4; j++)
			{
				for (int k = 0; k < 4; k++)
				{
					result[i][j] += m[i][k] * m2[k][j];
				}


			}

		}

		return result;
	}
	glm::mat4x4 static transposeMatrix(glm::mat4x4 m)
	{
		glm::mat4x4 transpos;
		for (int i = 0; i < 4; i++)
		{

			for (int j = 0; j < 4; j++)
			{
				transpos[j][i] = m[i][j];
			}
		}
		return transpos;
	}
private:

};

// Class for sphere
class Sphere: public Shape
{
public:
	
	Sphere(void);	// Constructor
	Sphere(int id, int matIndex, int textureIndex, int textureIndex2, Material* material, int cIndex, float R, vector<glm::vec3> *vertices,ShapeType type, vector<std::pair<char, int>> transformations, glm::vec3 motionBlur, ShadingMode shadingMode);	// Constructor
	
	IntersectionInfo intersect(const Ray & ray, Ray* rayTransformed = nullptr) ;	// Will take a ray and return a structure related to the intersection information. You will implement this.
	BoundingBox* getBounds();
private:
	glm::vec3 center;
	float radius;
	
};

// Class for triangle
class Triangle: public Shape
{
public:
	Triangle(void);	// Constructor
	Triangle(int id, int matIndex, int textureIndex, int textureIndex2, Material* material, int p1Index, int p2Index, int p3Index, vector<glm::vec3> *vertices, ShapeType type, vector<std::pair<char, int>> transformations, glm::vec3 motionBlur, ShadingMode shadingMode);	// Constructor
	IntersectionInfo intersect(const Ray & ray, Ray* rayTransformed = nullptr) ; // Will take a ray and return a structure related to the intersection information. You will implement this.
	BoundingBox* getBounds();

    int point1;
    int point2;
    int point3;
	int textPoint1;
	int textPoint2;
	int textPoint3;

	glm::vec3 faceNormal;
	glm::vec3 normal1;
	glm::vec3 normal2;
	glm::vec3 normal3;
	glm::vec3 getNormal();
	bool first;

};

// Class for mesh
class Mesh: public Shape
{
public:
	BoundingVolume* boundingVolume;
	Mesh(void);	// Constructor
	Mesh(int id, int matIndex, int textureIndex, int textureIndex2, Material* material, const vector<Triangle*>& faces, vector<int> *pIndices, vector<glm::vec3> *vertices, ShapeType type, vector<std::pair<char, int>> transformations, glm::vec3 motionBlur, ShadingMode shadingMode);	// Constructor
	void MeshVolumeIntersection(const Ray& ray, Node* node, IntersectionInfo* intersecion_info,Ray * rayTransformed) ;
	IntersectionInfo intersect(const Ray & ray, Ray* rayTransformed = nullptr) ; // Will take a ray and return a structure related to the intersection information. You will implement this.
	BoundingBox* getBounds();
	void face(int start, int end);
	const vector<Triangle*> faces;
	void calculateFaceNormals();
	
};

#endif
