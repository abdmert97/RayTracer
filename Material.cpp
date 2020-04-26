#include "Material.h"

Material::Material(void)
{
	phongExp = 0;
	
	glm::vec3 ambientRef = {0,0,0};	// Coefficients for ambient reflection
	glm::vec3 diffuseRef = { 0,0,0 };	// Coefficients for diffuse reflection
	glm::vec3 specularRef = { 0,0,0 };	// Coefficients for specular reflection
	glm::vec3 mirrorRef = { 0,0,0 };		// Coefficients for mirror reflection
	
	refractionIndex = 1;
}
