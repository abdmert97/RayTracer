#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "Scene.h"

enum MaterialType
{
	Default,
	Mirror,
	Conductor,
	Dialectic
};
// Class to hold variables related to a material
class Material
{
public:
	int id;	// Material id
	MaterialType materialType;
	int phongExp;	// Phong exponent
	glm::vec3 ambientRef;	// Coefficients for ambient reflection
	glm::vec3 diffuseRef;	// Coefficients for diffuse reflection
	glm::vec3 specularRef;	// Coefficients for specular reflection
	glm::vec3 mirrorRef;		// Coefficients for mirror reflection
	glm::vec3 absorptionCoefficient;
	float absorptionIndex;
	float refractionIndex;
	float roughness;
	Material(void);	// Constructor
	
private:
	// Write any other stuff here
};

#endif
