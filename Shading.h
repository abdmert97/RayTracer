#ifndef _SHADING_H_
#define _SHADING_H_

#include "Light.h"
#include "Scene.h"
//#include "Reflection.h"
//#include  "Refraction.h"

class Reflection;
class Refraction;

class PointLight;
class Material;
class Shape;
class IntersectionInfo;
class Shading
{

private:
	
	float shadowRayEps;
	vector<Material*> materials;
	glm::vec3* ambientLightList;
	int lightCount;
	int objectCount;
	vector<PointLight*> lights;
	vector<Shape*> *objects;
public:
	
	

	Shading( float shadow_ray_eps, const vector<Material*>& materials, glm::vec3* ambient_light_list,
		int light_count, int object_count, const vector<PointLight*>& point_lights,vector<Shape*>* objects)
		: 
		  shadowRayEps(shadow_ray_eps),
		  materials(materials),
		  ambientLightList(ambient_light_list),
		  lightCount(light_count),
		  objectCount(object_count),
		  lights(point_lights),
		  objects(objects)
	{
	}

private:
	bool isShadow(glm::vec3& lightPosition, glm::vec3& intersectionPoint)const;
	void calculateColor(IntersectionInfo& closestObjectInfo, Material material, PointLight* light, glm::vec3 lightVector,
	glm::vec3 cameraVectorNormalized, glm::vec3& shaders) const;
	glm::vec3 blinnPhongShading(glm::vec3 lightRayVector, glm::vec3& cameraRayVector, Material& material,
	glm::vec3& lightIntensity, glm::vec3& normal) const;
	glm::vec3 diffuseShading(glm::vec3 lightRayVector, Material& material, glm::vec3& lightIntensity, glm::vec3& normal)const;
public:
	glm::vec3 shading(int depth, Shape*& shape, IntersectionInfo& closestObjectInfo, Ray& ray, float n_t =1);
	glm::vec3 reflect(const glm::vec3& incoming, const glm::vec3& normal);
	glm::vec3 glossyReflection(float roughness, glm::vec3 vec);
	void getReflection(int depth, IntersectionInfo& intersectionInfo, Material material, glm::vec3& color, glm::vec3 cameraVectorNormalized);
	glm::vec3 refract(const glm::vec3& incoming, const glm::vec3& normal, const float& refractionIndex, float n_i);
	float fresnel(const glm::vec3& incoming, const glm::vec3& normal, const float& refractionIndex, Material material, float n_i);
	void refraction(int depth, Ray ray, IntersectionInfo& intersection, Material material, glm::vec3& color, glm::vec3 rayDirection, float n_i);
	float randomE();
};



#endif