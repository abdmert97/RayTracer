#ifndef _SCENE_H_
#define _SCENE_H_


#include <string>
#include <vector>
#include <thread>
#include "Image.h"
#include "tinyxml2.h"
#include "glm/glm.hpp"
#include <iostream>
#include "happly.h"
#include "Camera.h"
#include "Material.h"
#include "Shape.h"
#include "Light.h"
#include "Shading.h"
#include "Node.h"
#include "Texture.h"

// Forward declarations to avoid cyclic references
class Shape;
class Triangle;
class Mesh;
class Sphere;
class Camera;
class Material;
class BoundingVolume;
class PointLight;
class Shading;
class Node;
class Texture;
class TextureMap;
using namespace std;
using namespace tinyxml2;

static void printVector(glm::vec3 vect, const char* optional = nullptr)
{
	if (optional != nullptr)
		cout << optional;
	cout << "x: " << vect.x << " y: " << vect.y << " z: " << vect.z << endl;
}
static void printVector(glm::vec4 vect, const char* optional = nullptr)
{
	if (optional != nullptr)
		cout << optional;
	cout << "x: " << vect.x << " y: " << vect.y << " z: " << vect.z << " w: " << vect.w << endl;
}
static void printMatrix(glm::mat4x4 mat, const char* optional = nullptr)
{
	if (optional != nullptr)
		cout << optional<< endl;
	for (int i = 0; i < 4; i++)
	{
		glm::vec4 vect = mat[i];
		cout << "x: " << vect.x << " y: " << vect.y << " z: " << vect.z << " w: " << vect.w << endl;
	}
}
class Scene
{
public:
	
	bool transformationEnabled;
	int maxRecursionDepth;	// Maximum recursion depth
	float intTestEps;				// IntersectionTestEpsilon. You will need this one while implementing intersect routines in Shape class
	float shadowRayEps;				// ShadowRayEpsilon. You will need this one while generating shadow rays.
	glm::vec3 backgroundColor;		// Background color
	glm::vec3 ambientLight;			// Ambient light radiance
	int numberofSample;
	vector<Camera*> cameras;		// Vector holding all cameras
	vector<PointLight*> lights;		// Vector holding all point lights
	vector<Material*> materials;	// Vector holding all materials
	vector<glm::vec3> vertices;		// Vector holding all vertices (vertex data)
	vector<glm::vec2> textCoord;
	vector<Shape*> objects;			// Vector holding all shapes
	vector<Texture *> textures;
	vector<TextureMap*> textureMaps;
	Shading* shading;
	
	BoundingVolume* boundingVolume;

	vector<glm::vec3> translationList;
	vector<glm::vec3> scalingList;
	vector<glm::vec4> rotationList;
	vector<glm::mat4x4*> translationMatrices;
	vector<glm::mat4x4*> scalingMatrices;
	vector<glm::mat4x4*> rotationMatrices;
	vector<glm::mat4x4*> normtranslationMatrices;
	vector<glm::mat4x4*> normscalingMatrices;
	vector<glm::mat4x4*> normrotationMatrices;
	Scene(const char* xmlPath);

	
	void initObjects();
	void initScene();






	

	
	void renderScene(void);			

	void convertPPMToPNG(string ppmFileName, int osType);

	void BoundingBoxIntersection(Ray ray, Node* node, IntersectionInfo* retVal);
	IntersectionInfo closestObject(Ray ray);
	static float randomTime();
	void readTexture(const char* fileName, int id);
private:
	void readXML(const char* xmlPath);
	void readPly(const char* str, const vector<Triangle>& faces, vector<int>* is);
	void renderImagePart(float start, float end, Camera* camera, Image* image);
	void setScene();
	void readPly(const char* fileName);
	void threading(Camera* camera, Image* image);
	void renderImage(Camera* camera, Image* image);
	
	void initMatrices();


	// Read XML
	void readConstants(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot);
	void readCamera(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot);
	void readMaterials(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot);
	void readTransformations(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot);
	void readVertices(const char*& str, XMLElement*& pElement, XMLNode* pRoot);
	void readTextCoord(const char*& str, XMLElement*& pElement, XMLNode* pRoot);
	vector<std::pair<char, int>> readTransform(const char*& str, XMLElement*& objElement, XMLElement*& pObject);
	void readMeshes(const char*& str, XMLError& eResult, XMLElement*& pElement, int& idCount, XMLElement*& objElement,
		XMLElement*& pObject);
	void readSpheres(const char*& str, XMLError& eResult, XMLElement*& pElement, int& idCount, XMLElement*& objElement,
		XMLElement*& pObject);
	void readTriangles(const char*& str, XMLError& eResult, XMLElement*& pElement, int& idCount,
		XMLElement*& objElement,
		XMLElement*& pObject);
	void readInstances(const char*& str, XMLError& eResult, XMLElement*& pElement, int& idCount,
		XMLElement*& objElement,
		XMLElement*& pObject);
	void readObjects(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot);
	void readLights(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot);
	void readTextureXml(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot);
	void readExr(const char* input);
	// Read XML

	int cameraCount;
	int lightCount;
	int objectCount;
	glm::vec3* ambientLightList;
};
#endif

