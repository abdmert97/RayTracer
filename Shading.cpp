#include "Shading.h"
#include <random>
#define PI 3.14159265
extern Scene* pScene;
glm::vec3 Shading::shading(int depth, Shape*& shape, IntersectionInfo& closestObjectInfo, Ray& ray,float n_t)
{
	// If depth is less then zero stop shading
	if (depth < 0)
	{
		glm::vec3 color = { 0,0,0 };
		return color;
	}
	
	Material material = *materials[shape->matIndex];

	bool isInside = glm::dot(ray.direction,closestObjectInfo.hitNormal) > 0;
	
	glm::vec3 color = { 0,0,0 };
	glm::vec3 cameraVectorNormalized = glm::normalize(ray.origin - closestObjectInfo.intersectionPoint);
	
	if(!isInside)
	{
		color = ambientLightList[shape->matIndex];

		glm::vec3 lightVector;
		
		for (int l = 0; l < lightCount; l++)
		{
			PointLight* light = lights[l];
			lightVector = (light->position - closestObjectInfo.intersectionPoint);
			// Shadows

			if(shape->textureIndex != -1 && pScene->textureMaps[shape->textureIndex]->decalMode == ReplaceALL)
			{
				;
			}
			else if (isShadow(light->position, closestObjectInfo.intersectionPoint))
			{
				continue;
			}
			// Shading
			glm::vec3 shaders;
			if(shape->textureIndex != -1)
			{
				calculateTextureColor(closestObjectInfo, material,shape->textureIndex, shape->textureIndex2, (Light*)light, lightVector, cameraVectorNormalized, shaders);
				
			}
			else
			{
				calculateColor(closestObjectInfo, material, (Light*)light, lightVector, cameraVectorNormalized, shaders);
			}
			color = color + shaders;
		}
		for (int l = 0 ; l < pScene->directionalLights.size();l++)
		{
			DirectionalLight* light = pScene->directionalLights[l];
			lightVector = -light->direction;
			glm::vec3 s = closestObjectInfo.intersectionPoint + glm::vec3(200)*lightVector;
			if (isShadow(s, closestObjectInfo.intersectionPoint))
			{
				continue;
			}
			// Shading
			glm::vec3 shaders;
			if (shape->textureIndex != -1)
			{
				calculateTextureColor(closestObjectInfo, material, shape->textureIndex, shape->textureIndex2, (Light*)light, lightVector, cameraVectorNormalized, shaders);

			}
			else
			{
				calculateColor(closestObjectInfo, material, (Light*)light, lightVector, cameraVectorNormalized, shaders);
			}
			color = color + shaders;
		}
		for (int l = 0; l < pScene->spotLights.size(); l++)
		{
			SpotLight *light = pScene->spotLights[l];
			lightVector = (light->position - closestObjectInfo.intersectionPoint);
			// Shadows

			if (isShadow(light->position, closestObjectInfo.intersectionPoint))
			{
				continue;
			}
			// Shading
			glm::vec3 shaders;
			if (shape->textureIndex != -1)
			{
				calculateTextureColor(closestObjectInfo, material, shape->textureIndex, shape->textureIndex2, (Light*)light, lightVector, cameraVectorNormalized, shaders);

			}
			else
			{
				calculateColor(closestObjectInfo, material, (Light*)light, lightVector, cameraVectorNormalized, shaders);
			}
			color = color + shaders;
		}
		for (int l = 0; l < pScene->areaLights.size(); l++)
		{
			AreaLight *light = pScene->areaLights[l];
			lightVector = (light->position - closestObjectInfo.intersectionPoint);
			// Shadows
		
			if (isShadow(light->position, closestObjectInfo.intersectionPoint))
			{
				continue;
			}
			// Shading
			glm::vec3 shaders;
			if (shape->textureIndex != -1)
			{
				calculateTextureColor(closestObjectInfo, material, shape->textureIndex, shape->textureIndex2, (Light*)light, lightVector, cameraVectorNormalized, shaders);

			}
			else
			{
				calculateColor(closestObjectInfo, material, (Light*)light, lightVector, cameraVectorNormalized, shaders);
			}
			color = color + shaders;
		}

		for (int l = 0; l < pScene->enviromentLight.size(); l++)
		{
			EnviromentLight* light = pScene->enviromentLight[l];
			light->normal = closestObjectInfo.hitNormal;
			light->computeLightContribution(closestObjectInfo.intersectionPoint);
			lightVector = (light->position - closestObjectInfo.intersectionPoint);
			// Shadows
			
			if (isShadow(light->position, closestObjectInfo.intersectionPoint))
			{
				continue;
			}
		
			// Shading
			glm::vec3 shaders;
			if (shape->textureIndex != -1)
			{
				calculateTextureColor(closestObjectInfo, material, shape->textureIndex, shape->textureIndex2, (Light*)light, lightVector, cameraVectorNormalized, shaders);
			
			}
			else
			{
				calculateColor(closestObjectInfo, material, (Light*)light, lightVector, cameraVectorNormalized, shaders);
			}
			color = color + shaders;
		}
	}

	if (depth <= 0)
		return color;
	if (material.materialType == Default)
		return color;
	if (material.materialType == Mirror)
		getReflection(depth, closestObjectInfo, material, color, cameraVectorNormalized);
	else if (material.materialType == Conductor || material.materialType == Dialectic)
		refraction(depth, ray, closestObjectInfo, material, color,ray.direction,n_t);
	
	return color;
}
bool Shading::isShadow(glm::vec3& lightPosition, glm::vec3& intersectionPoint)const
{
	glm::vec3 lightVector = glm::normalize(lightPosition - intersectionPoint);
	
	Ray shadowRay = Ray((intersectionPoint + lightVector * shadowRayEps), lightVector);
	Ray shadowRay_ = Ray(intersectionPoint, lightVector);
	float tFromIntersectionToLight = shadowRay_.gett(lightPosition);
	
	IntersectionInfo returnVal = pScene->closestObject(shadowRay);
	Shape* shadowShape;
	if (returnVal.t < tFromIntersectionToLight)
	{
		return true;
	}
	/*
	for (int o = 0; o < objectCount; o++) {
		shadowShape = objects[o];
		returnVal = (shadowShape->intersect(*shadowRay));
		if (returnVal.isIntersect)
		{
			if (returnVal.t < tFromIntersectionToLight)
			{
				return true;
			}
		}
	}*/

	return false;
}
void Shading::calculateTextureColor(IntersectionInfo& closestObjectInfo, Material material,int textureIndex, int textureIndex2, Light* light, glm::vec3 lightVector, glm::vec3 cameraVectorNormalized, glm::vec3& shaders) const
{
	TextureMap *map = pScene->textureMaps[textureIndex];
	TextureMap* map2 = pScene->textureMaps[textureIndex];
	if(textureIndex2 != -1)
	{
		map2 = pScene->textureMaps[textureIndex2];
	}
	glm::vec3 normal = closestObjectInfo.hitNormal;

	glm::vec3 lightVectorNormalized = normalize(lightVector);
	
	
	shaders = { 0,0,0 };
	glm::vec3 blinnPhongShade  ;

	glm::vec3 diffuseShade;
	if(map->textureType == Perlin)
	{
		glm::vec3 textureColor = map->getTextureColor(closestObjectInfo.textCoord,closestObjectInfo.intersectionPoint);

		if(map->noiseConversion == Absval)
		{
			textureColor = glm::vec3(abs(textureColor.r));
		}
		else
		{
			float col = textureColor.r;

			col = (col + 1.0) / 2.0;
			
			textureColor = glm::vec3(col);
	
		}
		if (map->decalMode == BumpNormal)
		{

			glm::vec3 gradient = glm::vec3(closestObjectInfo.TBN[0][0], closestObjectInfo.TBN[1][0],
				closestObjectInfo.TBN[2][0]);
			glm::vec3 bumpNormal = closestObjectInfo.hitNormal- gradient*map->bumpFactor;
			bumpNormal = glm::normalize(bumpNormal);
			normal = bumpNormal;
			

		}
		else if (map->decalMode == ReplaceKD)
		{
			material.diffuseRef = textureColor;
	
		}
		if (map->decalMode == ReplaceALL)
		{
			shaders = textureColor;
			return;

		}
		
	}
	else
	{
		
		glm::vec3 textureColor = map->getTextureColor(closestObjectInfo.textCoord);
		if (map->decalMode == ReplaceALL)
		{
			shaders = textureColor;
			
			return;

		}
		if (map->decalMode == ReplaceKD)
		{
			material.diffuseRef = textureColor / glm::vec3(map->normalizer);
	
		
		}
		else if (map->decalMode == ReplaceNormal)
		{
			glm::vec3 norm = textureColor / glm::vec3(255) - glm::vec3(0.5);

			norm = glm::normalize(multiplyMatrixWithVec3(closestObjectInfo.TBN, norm));

			normal = norm;

		}
		else if (map->decalMode == BlendKD)
		{

		
			material.diffuseRef += (textureColor / glm::vec3(255));
			material.diffuseRef /= 2;
			
	
		}
		else if (map->decalMode == BumpNormal)
		{
			
			glm::vec3 P_u = glm::vec3( closestObjectInfo.TBN[0][0],closestObjectInfo.TBN[1][0],
			closestObjectInfo.TBN[2][0] ) * map->bumpFactor;
			glm::vec3 P_v = glm::vec3(closestObjectInfo.TBN[0][1], closestObjectInfo.TBN[1][1],
				closestObjectInfo.TBN[2][1]) * map->bumpFactor;
			float eps = 0.00002;			
			glm::vec3 B_u = (map->getTextureColor(closestObjectInfo.textCoord-glm::vec2(eps,0))-textureColor);
			glm::vec3 B_v = (map->getTextureColor(closestObjectInfo.textCoord- glm::vec2(0, eps)) - textureColor);;
			glm::vec3 bumpNormal = closestObjectInfo.hitNormal + B_v * (glm::cross(P_u, closestObjectInfo.hitNormal)) + B_u * (glm::cross(P_v, closestObjectInfo.hitNormal));
			bumpNormal = glm::normalize(bumpNormal);	
			normal = bumpNormal;
		}
	}
	if(textureIndex2!= -1)
	{
		if (map2->textureType == Perlin)
		{
			glm::vec3 textureColor = map2->getTextureColor(closestObjectInfo.textCoord, closestObjectInfo.intersectionPoint);

			if (map2->noiseConversion == Absval)
			{
				textureColor = glm::vec3(abs(textureColor.r));
			}
			else
			{
				float col = textureColor.r;

				col = (col + 1.0) / 2.0;

				textureColor = glm::vec3(col);

			}
	
			if (map2->decalMode == BumpNormal)
			{

				glm::vec3 gradient = glm::vec3(closestObjectInfo.TBN[0][0], closestObjectInfo.TBN[1][0],
					closestObjectInfo.TBN[2][0]);
				glm::vec3 bumpNormal = closestObjectInfo.hitNormal - gradient * map2->bumpFactor;
				bumpNormal = glm::normalize(bumpNormal);
				normal = bumpNormal;
			

			}
			else if (map2->decalMode == ReplaceKD)
			{
				material.diffuseRef = textureColor;
			}
			if (map->decalMode == ReplaceALL)
			{
				shaders = textureColor;
				return;

			}
		}
		else
		{

			glm::vec3 textureColor = map2->getTextureColor(closestObjectInfo.textCoord);
			if (map->decalMode == ReplaceALL)
			{
				shaders = textureColor;
				return;

			}
			if (map2->decalMode == ReplaceKD)
			{
				material.diffuseRef = textureColor / glm::vec3(map2->normalizer);

			}
			else if (map2->decalMode == ReplaceNormal)
			{
				glm::vec3 norm = textureColor / glm::vec3(255) - glm::vec3(0.5);

				norm = glm::normalize(multiplyMatrixWithVec3(closestObjectInfo.TBN, norm));

				normal = norm;

			}
			else if (map2->decalMode == BlendKD)
			{


				material.diffuseRef += (textureColor / glm::vec3(255));
				material.diffuseRef /= 2;

			
			}
			else if (map2->decalMode == BumpNormal)
			{
				
					glm::vec3 P_u = glm::vec3(closestObjectInfo.TBN[0][0], closestObjectInfo.TBN[1][0],
						closestObjectInfo.TBN[2][0]) * map2->bumpFactor;
					glm::vec3 P_v = glm::vec3(closestObjectInfo.TBN[0][1], closestObjectInfo.TBN[1][1],
						closestObjectInfo.TBN[2][1]) * map->bumpFactor;
					float eps = 0.00002;
					glm::vec3 B_u = map2->getTextureColor(closestObjectInfo.textCoord - glm::vec2(eps, 0)) - textureColor;
					glm::vec3 B_v = map2->getTextureColor(closestObjectInfo.textCoord - glm::vec2(0, eps)) - textureColor;;
					glm::vec3 bumpNormal = closestObjectInfo.hitNormal + B_v * (glm::cross(P_u, closestObjectInfo.hitNormal)) + B_u * (glm::cross(P_v, closestObjectInfo.hitNormal));
					bumpNormal = glm::normalize(bumpNormal);
					normal = bumpNormal;
				
			
			}
		}
	}
	glm::vec3 intensity = light->computeLightContribution(closestObjectInfo.intersectionPoint);
	diffuseShade = diffuseShading(lightVectorNormalized, material, intensity, normal);
	blinnPhongShade = blinnPhongShading(lightVectorNormalized, cameraVectorNormalized, material, intensity, normal);
	shaders = diffuseShade + blinnPhongShade;
	

}
glm::vec3 Shading::diffuseShading(glm::vec3 lightRayVector, Material& material, glm::vec3& lightIntensity, glm::vec3& normal)const
{

	
	float cosTheta = dot(lightRayVector, normal);

	if (cosTheta < 0) cosTheta = 0;

	glm::vec3 diffuse = glm::vec3(lightIntensity * cosTheta * material.diffuseRef);
	if (material.brdf.brdfType == NormalizedModifiedPhong || material.brdf.brdfType == NormalizedModifiedBlinnPhong 
	 || material.brdf.brdfType == TorranceSparrow)
	{
		float pi = (1 / PI);
		diffuse = diffuse * pi;
	}

	return diffuse;

}
void Shading::calculateColor(IntersectionInfo& closestObjectInfo, Material material,Light* light, glm::vec3 lightVector, glm::vec3 cameraVectorNormalized, glm::vec3& shaders) const
{
	glm::vec3 lightVectorNormalized = normalize(lightVector);
	glm::vec3 intensity = light->computeLightContribution(closestObjectInfo.intersectionPoint);
	shaders = { 0,0,0 };

	glm::vec3 blinnPhongShade = blinnPhongShading(lightVectorNormalized, cameraVectorNormalized, material, intensity, closestObjectInfo.hitNormal);
	glm::vec3 diffuseShade = diffuseShading(lightVectorNormalized, material, intensity, closestObjectInfo.hitNormal);
	shaders = diffuseShade +blinnPhongShade ;


}
glm::vec3 Shading::blinnPhongShading(glm::vec3 lightRayVector, glm::vec3& cameraRayVector, Material& material, glm::vec3& lightIntensity, glm::vec3& normal)const
{

	
	if (material.brdf.id != -1 )
	{
		if(material.brdf.brdfType == OriginalPhong)
		{
			float cosAlpha = glm::dot(lightRayVector, normal);
			if (acos(cosAlpha) * 57.2957795 < 90&& cosAlpha>0)
			{
				glm::vec3 ref = glm::normalize( lightRayVector - normal * glm::dot(lightRayVector, normal) * glm::vec3(2));
				float cosR = glm::dot(cameraRayVector, ref);
				float phong = pow(cosR, material.brdf.exponent) / cosAlpha;
		
				glm::vec3 blinnPhongLight = lightIntensity * (material.specularRef) * phong;
				return blinnPhongLight;
			}
			else
			{
				glm::vec3 blinnPhongLight = glm::vec3(0);
				return blinnPhongLight;
			}
		}
		else if (material.brdf.brdfType == ModifiedPhong)
		{
			float cosAlpha = glm::dot(lightRayVector, normal);
			if (acos(cosAlpha) * 57.2957795 < 90 && cosAlpha > 0)
			{
				glm::vec3 ref = glm::normalize(lightRayVector - normal * glm::dot(lightRayVector, normal) * glm::vec3(2));
				float cosR = glm::dot(cameraRayVector, ref);
				float phong = pow(cosR, material.brdf.exponent);

				glm::vec3 blinnPhongLight = lightIntensity * (material.specularRef) * phong;
				
				return blinnPhongLight;
			}
			else
			{
				glm::vec3 blinnPhongLight = glm::vec3(0);
				return blinnPhongLight;
			}
		}
		else if	(material.brdf.brdfType == NormalizedModifiedPhong)
		{
			glm::vec3 ref = glm::normalize(lightRayVector - normal * glm::dot(lightRayVector, normal) * glm::vec3(2));
			float cosR = glm::dot(cameraRayVector, ref);
			float phong = pow(cosR, material.brdf.exponent);
			float exp = ((material.brdf.exponent + 2) / (2 * PI) * phong);
			
			glm::vec3 blinnPhongLight = lightIntensity * (material.specularRef) *exp;

			return blinnPhongLight;
		}
		else if (material.brdf.brdfType == OriginalBlinnPhong)
		{
			glm::vec3 halfVector = normalize(lightRayVector + cameraRayVector);

			float cosAlpha = glm::dot(lightRayVector, normalize(normal));
			
			float cosR = glm::dot(halfVector, normal);
			float phong = pow(cosR, material.brdf.exponent) ;

			glm::vec3 blinnPhongLight = lightIntensity * (material.specularRef) * phong;
			return blinnPhongLight;
		}
		else if (material.brdf.brdfType == ModifiedBlinnPhong)
		{
			glm::vec3 halfVector = normalize(lightRayVector + cameraRayVector);

		
			float cosR = glm::dot(halfVector, normal);
			float phong = pow(cosR, material.brdf.exponent) ;

			glm::vec3 blinnPhongLight = lightIntensity * (material.specularRef) * phong;
			return blinnPhongLight;
		}
		else if (material.brdf.brdfType == NormalizedModifiedBlinnPhong)
		{

			glm::vec3 ref = glm::normalize(lightRayVector - normal * glm::dot(lightRayVector, normal) * glm::vec3(2));
			glm::vec3 halfVector = normalize(lightRayVector + cameraRayVector);
			float cosR = glm::dot(halfVector, normal);
			float phong = pow(cosR, material.brdf.exponent);
			float exp = ((material.brdf.exponent + 8) / (8 * PI) * phong);

			glm::vec3 blinnPhongLight = lightIntensity * (material.specularRef) * exp;

			return blinnPhongLight;
		}
		else if (material.brdf.brdfType == TorranceSparrow)
		{
			float cosAlpha = glm::dot(lightRayVector, normal);
			
				glm::vec3 halfVector = normalize(lightRayVector + cameraRayVector);
				float cosH = glm::dot(halfVector, normal);
				float DAlpha = ((material.brdf.exponent + 2) / (2 * PI)) * pow(cosH, material.brdf.exponent);
				float x = 2 * glm::dot(normal, halfVector) * glm::dot(normal, cameraRayVector);
				float y = glm::dot(cameraRayVector, halfVector);
				float z = 2 * glm::dot(normal, halfVector) * glm::dot(normal, lightRayVector);
				float a = min(x / y, z / y);
				float G = min(1.0f, a);
				float cosPhi = glm::dot(normal, cameraRayVector);
				float cosTheta = glm::dot(normal, lightRayVector);
				float cosBeta = glm::dot(halfVector, cameraRayVector);
				float n = material.refractionIndex;
				float R_0 = fresnel(lightRayVector, normal, n, material, 1);
				float FBeta = R_0 + (1 - R_0) * pow((1 - cosBeta), 5);
				float den =1/( 4 * cosTheta * cosPhi);
				glm::vec3 blinnPhongLight = lightIntensity * (material.specularRef) * DAlpha * FBeta * G * den;
				return blinnPhongLight;
			
		}

	}
	glm::vec3 halfVector = normalize(lightRayVector + cameraRayVector);

	float cosAlpha = glm::dot(halfVector, normalize(normal));
	if (cosAlpha < 0) cosAlpha = 0;
	float phong = pow(cosAlpha, material.phongExp);
	glm::vec3 blinnPhongLight = glm::vec3(lightIntensity.x * material.specularRef.x*phong, lightIntensity.y * material.specularRef.y * phong, lightIntensity.z * material.specularRef.z * phong);

	return blinnPhongLight;

}
glm::vec3 Shading::reflect(const glm::vec3& incoming, const glm::vec3& normal)
{
	return incoming - normal * glm::dot(incoming, normal) * glm::vec3(2);
}
float Shading::randomE()
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	uniform_real_distribution<> dis(-0.5, 0.5);
	return dis(gen);
}
glm::vec3 Shading::glossyReflection(float roughness, glm::vec3 vec)
{
	float  x = abs(vec.x);
	float  y = abs(vec.y);
	float  z = abs(vec.z);
	float minV = min(x, min(y, z));
	glm::vec3 r = glm::vec3(vec);
	if(minV == x)
	{
		r.x = 1;
	}
	else if (minV == y)
	{
		r.y = 1;
	}
	else if (minV == z)
	{
		r.z = 1;
	}
	glm::vec3 u = glm::normalize(glm::cross(vec, r));
	glm::vec3 v = glm::normalize(glm::cross(vec, u));
	float e1 =randomE();
	float e2 =randomE();

	return glm::normalize(vec + roughness * (e1 * u + e2 * v));
}
void Shading::getReflection(int depth, IntersectionInfo& intersectionInfo, Material material, glm::vec3& color, glm::vec3 cameraVectorNormalized)
{
	if (depth <= 0) return;
	if (material.materialType == Mirror || material.materialType == Conductor || material.materialType == Dialectic)
	{

		glm::vec3 reflectionDirection = normalize(reflect(cameraVectorNormalized * glm::vec3(-1), intersectionInfo.hitNormal));
		if(material.roughness != 0)
			reflectionDirection = glossyReflection(material.roughness, reflectionDirection);
		Ray reflectionRay = Ray(intersectionInfo.intersectionPoint + reflectionDirection * shadowRayEps, reflectionDirection);

		IntersectionInfo reflectionInfo = pScene->closestObject(reflectionRay);

		if (reflectionInfo.isIntersect)
		{
			Shape* reflectionShape = (*objects)[reflectionInfo.objectID];

			glm::vec3 reflectanceColor = { 0,0,0 };

			reflectanceColor = reflectanceColor + shading((depth - 1), reflectionShape, reflectionInfo, reflectionRay);

			glm::vec3 reflectance = {
				(reflectanceColor.x * material.mirrorRef.x),
				(reflectanceColor.y * material.mirrorRef.y),
				(reflectanceColor.z * material.mirrorRef.z) };

			color = color + reflectance;
		}
		else
		{
			glm::vec3 reflectanceColor = {
				(pScene->backgroundColor.r * material.mirrorRef.x),
				(pScene->backgroundColor.g * material.mirrorRef.y),
				(pScene->backgroundColor.b * material.mirrorRef.z) };
			color = color + reflectanceColor;
		}
	}
}
glm::vec3 Shading::refract(const glm::vec3& incoming, const glm::vec3& normal, const float& refractionIndex, float n_i)
{

	float cosi = glm::dot(incoming, normal);

	if (cosi < -1) cosi = -1;
	if (cosi > 1) cosi = 1;

	float etai = n_i, etat = refractionIndex;
	glm::vec3 n = normal;
	if (cosi < 0) { cosi = -cosi; }
	else { std::swap(etai, etat); n = normal * glm::vec3(-1); }
	float eta = etai / etat;

	float k = 1 - eta * eta * (1 - cosi * cosi);
	//return k < 0 ? glm::vec3{ 0,0,0 } : (incoming+n*cosi)*eta- n*sqrt(1-eta*eta*(1-cosi*cosi));
	return k < 0 ? glm::vec3{ 0,0,0 } : incoming * eta + n * (eta * cosi - sqrtf(k));
}
float Shading::fresnel(const glm::vec3& incoming, const glm::vec3& normal, const float& refractionIndex, Material material, float n_i)const
{
	float kr = 0;

	float cosi = dot(incoming, normal);
	if (cosi < -1) cosi = -1;
	if (cosi > 1) cosi = 1;
	float  n_t = refractionIndex;
	if (cosi > 0) { std::swap(n_i, n_t); }

	// Compute sini using Snell's law
	float sint = (n_i / n_t) * sqrtf(std::max(0.f, 1 - cosi * cosi));
	// Total internal reflection

	float cost = sqrtf(std::max(0.f, 1 - sint * sint));
	cosi = fabsf(cosi);

	float Rs = material.materialType == Dialectic ?
		((n_t * cosi) - (n_i * cost)) / ((n_t * cosi) + (n_i * cost)) :
		(n_t * n_t + material.absorptionIndex * material.absorptionIndex - 2 * n_t * cosi + 1) /
		(n_t * n_t + material.absorptionIndex * material.absorptionIndex + 2 * n_t * cosi + 1);
	float Rp = material.materialType == Dialectic ?
		((n_i * cosi) - (n_t * cost)) / ((n_i * cosi) + (n_t * cost)) :
		(n_t * n_t + material.absorptionIndex * material.absorptionIndex * cosi * cosi - 2 * n_t * cosi + 1) /
		(n_t * n_t + material.absorptionIndex * material.absorptionIndex * cosi * cosi + 2 * n_t * cosi + 1);

	//	cout << Rs << " " << Rp << endl;

	if (material.materialType == Dialectic)
	{
		kr = (Rs * Rs + Rp * Rp) / 2;
	}
	else
	{
		kr = (Rs + Rp) / 2;

	}
	return kr;
	// As a consequence of the conservation of energy, transmittance is given by:
	// kt = 1 - kr;
}
void Shading::refraction(int depth, Ray ray, IntersectionInfo& intersection, Material material, glm::vec3& color, glm::vec3 rayDirection, float n_i)
{
	glm::vec3 refractionColor{ 0,0,0 };
	float n_t = material.refractionIndex;

	float kr = fresnel(rayDirection, intersection.hitNormal, n_t, material, n_i);
	//kt transmit



	if (material.materialType == Dialectic && kr < 1) {

		glm::vec3 hitPoint = intersection.intersectionPoint;
		bool outside = glm::dot(ray.direction, intersection.hitNormal) < 0;
		glm::vec3 bias = intersection.hitNormal * glm::vec3(0.01);
		glm::vec3 refractionDirection = normalize(refract(rayDirection, intersection.hitNormal, n_t, n_i));
		glm::vec3 refractionRayOrig = outside ? hitPoint - bias : hitPoint + bias;

		Shape* intersectShape = (*objects)[intersection.objectID];
		Ray reflectionRay = Ray(refractionRayOrig, refractionDirection);
		IntersectionInfo refractionIntersect = pScene->closestObject(reflectionRay);
		if (refractionIntersect.isIntersect)
		{
			Shape* refractionShape = (*objects)[refractionIntersect.objectID];

			refractionColor = shading(depth - 1, refractionShape, refractionIntersect, reflectionRay, n_i)*(glm::vec3(1)- material.absorptionCoefficient);

		}

	}




	if (material.materialType == Dialectic)
	{
		glm::vec3 abs = material.absorptionCoefficient;

		material.mirrorRef = { 1,1,1 };
	}


	glm::vec3 reflectionColor = { 0,0,0 };
	glm::vec3 cameraVector = glm::normalize(ray.origin - intersection.intersectionPoint);
	if (depth > 0)
		getReflection(depth, intersection, material, reflectionColor, cameraVector);


	// mix the two
	if (material.materialType == Dialectic)
		color = color + reflectionColor * kr + refractionColor * (1 - kr);
	else
		color = color + reflectionColor * kr;
}