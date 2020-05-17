#include "Scene.h"
#include <random>
#define PI 3.14159265
#define STB_IMAGE_IMPLEMENTATION   
#include "stb/stb_image.h"	
#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"
void Scene::initScene()
{
	
	setScene();
	initMatrices();
	// Create BoundingBox
	
	for (Shape* obj : objects)
	{
		obj->getBounds();
		obj->initTransformatrix();

		if(obj->shapeType == MeshType)
		{
			Mesh* mesh = (Mesh*)obj;
			cout << mesh->faces.size()<< endl;
			for (Triangle* const tris : mesh->faces)
			{
			
				tris->getBounds();
				tris->initTransformatrix();
			}
			if(mesh->shadingMode == SmoothShading)
				mesh->calculateFaceNormals();
		}
	}	
	boundingVolume = new BoundingVolume(objects);
	shading = new Shading(shadowRayEps, materials, ambientLightList, lightCount, objectCount, lights, &objects);
}
void Scene::renderScene(void)
{

	for (int i = 0; i < cameraCount; i++)
	{
		Camera* camera = cameras[i];
		ImagePlane imagePlane = camera->imgPlane;
		Image* image = new Image(imagePlane.nx, imagePlane.ny);
	
		threading(camera, image);
		//renderImage(camera, image);
		if (cameras[0]->keyValue > 0)
			toneMapping(image);
		image->saveImage(cameras[i]->imageName);
	}
}

void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		command = "convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// call command on Windows
	else if (osType == 2)
	{
		command = "magick convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// default action - don't do conversion
	else
	{
	}
}

Scene::Scene(const char* xmlPath)
{
	readXML(xmlPath);
}
void Scene::setScene()
{
	lightCount = lights.size();
	objectCount = objects.size();
	ambientLightList = new glm::vec3[materials.size()];
	for (int i = 0; i < materials.size(); i++)
	{
		Material mat = *materials[i];
		ambientLightList[i].x = (mat.ambientRef.x * ambientLight.x);
		ambientLightList[i].y = (mat.ambientRef.y * ambientLight.y);
		ambientLightList[i].z = (mat.ambientRef.z * ambientLight.z);
	}
	cameraCount = cameras.size();
	if(cameras[0]->keyValue>0)
	{
		colorMap = new glm::vec3*[cameras[0]->imgPlane.nx];
		for (int i = 0; i < cameras[0]->imgPlane.nx; ++i)
			colorMap[i] = new glm::vec3[cameras[0]->imgPlane.ny];
	}
}
void Scene::threading(Camera* camera, Image* image)
{

	int height = image->height;

	std::thread td0(&Scene::renderImagePart, this, 0, (float)10 / 64, camera, image);
	std::thread td1(&Scene::renderImagePart, this, (float)10 / 64, (float)18 / 64, camera, image);
	std::thread td2(&Scene::renderImagePart, this, (float)18 / 64, (float)25 / 64, camera, image);
	std::thread td3(&Scene::renderImagePart, this, (float)25 / 64, (float)31 / 64, camera, image);
	std::thread td4(&Scene::renderImagePart, this, (float)31 / 64, (float)32 / 64, camera, image);
	std::thread td5(&Scene::renderImagePart, this, (float)32 / 64, (float)33 / 64, camera, image);
	std::thread td6(&Scene::renderImagePart, this, (float)33 / 64, (float)34 / 64, camera, image);
	std::thread td7(&Scene::renderImagePart, this, (float)34 / 64, (float)36 / 64, camera, image);
	std::thread td8(&Scene::renderImagePart, this, (float)36 / 64, (float)38 / 64, camera, image);
	std::thread td9(&Scene::renderImagePart, this, (float)38 / 64, (float)46 / 64, camera, image);
	std::thread tda(&Scene::renderImagePart, this, (float)46 / 64, (float)53 / 64, camera, image);
	std::thread tdb(&Scene::renderImagePart, this, (float)53 / 64, (float)64 / 64, camera, image);
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
void Scene::renderImage(Camera* camera, Image* image)
{
	ImagePlane imagePlane = camera->imgPlane;
	IntersectionInfo returnValue;
	Shape* shape;

	int maxWidth = (imagePlane.nx);
	for (int w = 0; w < maxWidth; w++)
	{
		for (int h = 0; h < imagePlane.ny; h++)
		{

			Ray ray = camera->getPrimaryRay(w, h);

			// Selecting Closest object to the camera
			IntersectionInfo closestObjectReturnVal = closestObject(ray);
			if (closestObjectReturnVal.objectID == -1) // ray hits nothing
			{
				// Set background Color 
				Color background = { (unsigned char)backgroundColor.r,(unsigned char)backgroundColor.g,(unsigned char)backgroundColor.b };
				background.red = background.red > 255 ? 255 : background.red;
				background.grn = background.grn > 255 ? 255 : background.grn;
				background.blu = background.blu > 255 ? 255 : background.blu;
				image->setPixelValue(w, h, background);
			}
			else // if ray hits an object
			{

				// start Shading a object

				shape = objects[closestObjectReturnVal.objectID];
				glm::vec3 shadingColor = shading->shading(maxRecursionDepth, shape, closestObjectReturnVal, ray);
				//  cout<<(int)color.red<< " "<<(int)color.grn<< " "<<(int)color.blu<< " "<<endl;
				shadingColor.x = shadingColor.x > 255 ? 255 : shadingColor.x;
				shadingColor.y = shadingColor.y > 255 ? 255 : shadingColor.y;
				shadingColor.z = shadingColor.z > 255 ? 255 : shadingColor.z;
				Color lastColor = Color{ (unsigned int)shadingColor.x,(unsigned int)shadingColor.y,(unsigned int)shadingColor.z };
				image->setPixelValue(w, h, lastColor);
			}
		}
	}
}

float Scene::randomTime()
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	uniform_real_distribution<> dis(0.0, 1.0);
	return dis(gen);
}

void Scene::renderImagePart(float start, float end, Camera* camera, Image* image)
{
	Shape* shape;

	for (int w = 0; w < camera->imgPlane.nx; w++)
	{
		for (int h = start * camera->imgPlane.ny; h < end * camera->imgPlane.ny; h++)
		{
			Ray ray;
			int total = 0;
			glm::vec3 totalColor = glm::vec3(0);
			if (numberofSample != 0)
			{
				int sample =(int) sqrt(numberofSample);
				for (int i = 0; i < sample; i++)
				{
					for (int j = 0; j < sample; j++)
					{
						ray = camera->getPrimaryRay(w, h, i, j);
						IntersectionInfo closestObjectReturnVal = closestObject(ray);
						
						if (closestObjectReturnVal.objectID == -1) // ray hits nothing
						{
							 // Set background Color
							 int time = pow(sample,2) - pow(abs(i - sample / 2),2) - pow(abs(j - sample / 2),2);
					
							 total += time;
							for (TextureMap * map : textureMaps)
							{
								if(map->decalMode == ReplaceBG)
								{
									float u = (float)w / camera->imgPlane.nx;
									float v = (float)h / camera->imgPlane.ny;
									
									totalColor += map->getTextureColor(glm::vec2(u,v))* glm::vec3(time);
								}
							}
							
						
						}
						else // if ray hits an object
						{
							shape = objects[closestObjectReturnVal.objectID];
							
					   		
						     glm::vec3 shadingColor = shading->shading(maxRecursionDepth, shape, closestObjectReturnVal, ray);
					   		 //  cout<<(int)color.red<< " "<<(int)color.grn<< " "<<(int)color.blu<< " "<<endl;
					   		 shadingColor.x = shadingColor.x > 255 ? 255 : shadingColor.x;
					   		 shadingColor.y = shadingColor.y > 255 ? 255 : shadingColor.y;
					   		 shadingColor.z = shadingColor.z > 255 ? 255 : shadingColor.z;
							
							 int time = pow(sample, 2) - pow(abs(i - sample / 2), 2) - pow(abs(j - sample / 2), 2);
							
							 total += time;
							
					   			totalColor += (shadingColor * glm::vec3(time));
						
						}
					}
				}
				
				Color res = { totalColor.x/ total,totalColor.y / total,totalColor.z / total };
				
				image->setPixelValue(w, h, res);
			}
			else
			{
				ray = camera->getPrimaryRay(w, h);
			
				IntersectionInfo closestObjectReturnVal = closestObject(ray);
				if (closestObjectReturnVal.objectID == -1) // ray hits nothing
				{
					Color color;
						for (TextureMap * map : textureMaps)
							{
								if(map->decalMode == ReplaceBG)
								{
									float u = (float)w / camera->imgPlane.nx;
									float v = (float)h / camera->imgPlane.ny;

									glm::vec3 colorVec = map->getTextureColor(glm::vec2(u, v));
									 color = Color{ (unsigned int)colorVec.x,(unsigned int)colorVec.y,(unsigned int)colorVec.z };
								}
							}
					image->setPixelValue(w, h, color);
				}
				else // if ray hits an object
				{

					// start Shading a object

					shape = objects[closestObjectReturnVal.objectID];
					glm::vec3 shadingColor = shading->shading(maxRecursionDepth, shape, closestObjectReturnVal, ray);
					//  cout<<(int)color.red<< " "<<(int)color.grn<< " "<<(int)color.blu<< " "<<endl;
				
					// 0.2126R + 0.7152G+ 0.0722B;
					if(camera->keyValue>0)
					{
						//float luminance = 0.2126* shadingColor.r + 0.7152* shadingColor.g + 0.0722* shadingColor.b;
						colorMap[w][h]= shadingColor;
					
					}
					else
					{
						shadingColor.x = shadingColor.x > 255 ? 255 : shadingColor.x;
						shadingColor.y = shadingColor.y > 255 ? 255 : shadingColor.y;
						shadingColor.z = shadingColor.z > 255 ? 255 : shadingColor.z;
						Color lastColor = Color{ (unsigned int)shadingColor.x,(unsigned int)shadingColor.y,(unsigned int)shadingColor.z };
						image->setPixelValue(w, h, lastColor);
					}
				
				}
			}
			//	ray.origin    = transformation->inverseRotation(0, vec4(ray.origin, 1));

			// Selecting Closest object to the camera
		}
	}
	
}
void Scene::readConstants(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot)
{
	pElement = pRoot->FirstChildElement("MaxRecursionDepth");
	if (pElement != nullptr)
		pElement->QueryIntText(&maxRecursionDepth);
	
	pElement = pRoot->FirstChildElement("BackgroundColor");
	if(pElement)
	{
		str = pElement->GetText();
		sscanf(str, "%f %f %f", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);
	}
	else
	{
		backgroundColor = glm::vec3(0);
	}
	pElement = pRoot->FirstChildElement("ShadowRayEpsilon");
	if (pElement != nullptr)
		pElement->QueryFloatText(&shadowRayEps);

	pElement = pRoot->FirstChildElement("IntersectionTestEpsilon");
	if (pElement != nullptr)
		eResult = pElement->QueryFloatText(&intTestEps);
}


void Scene::readCamera(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot)
{
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement* pCamera = pElement->FirstChildElement("Camera");
	XMLElement* camElement;
	while (pCamera != nullptr)
	{
		int id;
		char imageName[64];
		glm::vec3 pos, gaze, up;
		ImagePlane imgPlane;
		float apertureSize = 0, focusDistance = 0;
		eResult = pCamera->QueryIntAttribute("id", &id);
		const char* type = pCamera->Attribute("type");
		if (type != nullptr && type[0] == 'l')
		{

			camElement = pCamera->FirstChildElement("Position");
			str = camElement->GetText();
			sscanf(str, "%f %f %f", &pos.x, &pos.y, &pos.z);

			camElement = pCamera->FirstChildElement("GazePoint");
			str = camElement->GetText();
			glm::vec3 gazePoint;
			sscanf(str, "%f %f %f", &gazePoint.x, &gazePoint.y, &gazePoint.z);

			camElement = pCamera->FirstChildElement("Up");
			str = camElement->GetText();
			sscanf(str, "%f %f %f", &up.x, &up.y, &up.z);

			camElement = pCamera->FirstChildElement("FovY");
			float fovY;
			eResult = camElement->QueryFloatText(&fovY);

			camElement = pCamera->FirstChildElement("NearDistance");
			eResult = camElement->QueryFloatText(&imgPlane.distance);
			
			camElement = pCamera->FirstChildElement("NumSamples");
			if (camElement != nullptr)
				eResult = camElement->QueryIntText(&numberofSample);

			camElement = pCamera->FirstChildElement("FocusDistance");
			if (camElement != nullptr)
				eResult = camElement->QueryFloatText(&focusDistance);

			camElement = pCamera->FirstChildElement("ApertureSize");
			if (camElement != nullptr)
				eResult = camElement->QueryFloatText(&apertureSize);

			

			camElement = pCamera->FirstChildElement("ImageResolution");
			str = camElement->GetText();
			sscanf(str, "%d %d", &imgPlane.nx, &imgPlane.ny);




			camElement = pCamera->FirstChildElement("ImageName");
			str = camElement->GetText();
			strcpy(imageName, str);


			glm::vec3 forward = normalize(pos - gazePoint);
			glm::vec3 left = cross(normalize(up), forward);
			glm::vec3 newUp = normalize(cross(forward, left));

			float height = imgPlane.distance * tan(fovY * 0.0174532925 * 0.5f);
			float r = (float)imgPlane.nx / imgPlane.ny;

			imgPlane.left = -1 * r * height;
			imgPlane.right = 1 * r * height;
			imgPlane.bottom = -1 * height;
			imgPlane.top = 1 * height;
			
			float keyValue = -1, burnPercent = -1, saturation = -1, gamma = -1;
			TMO tmo = Photographic;
			camElement = pCamera->FirstChildElement("Tonemap");
			if(camElement)
			{
				string tmoStr;
				XMLElement* TMO = camElement->FirstChildElement("TMO");
				if(TMO != nullptr)
				{
					tmoStr = TMO->GetText();
				
				}
				XMLElement* TMOOptions = camElement->FirstChildElement("TMOOptions");
				if (TMOOptions != nullptr)
				{
					str = TMOOptions->GetText();
				
					sscanf(str, "%f %f", &keyValue,&burnPercent);
				
				}
				XMLElement* Saturation = camElement->FirstChildElement("Saturation");
				if (Saturation != nullptr)
				{
					str = Saturation->GetText();

					sscanf(str, "%f", &saturation);
					
				}
				XMLElement* Gamma = camElement->FirstChildElement("Gamma");
				if (Gamma != nullptr)
				{
					str = Gamma->GetText();

					sscanf(str, "%f", &gamma);

				}
			}
			cameras.push_back(new Camera(id, imageName, pos, forward* glm::vec3(-1), newUp, imgPlane, numberofSample, apertureSize, focusDistance,keyValue,burnPercent,
			saturation,gamma,tmo));
		}
		else
		{
			camElement = pCamera->FirstChildElement("Position");
			str = camElement->GetText();
			sscanf(str, "%f %f %f", &pos.x, &pos.y, &pos.z);
			camElement = pCamera->FirstChildElement("Gaze");
			str = camElement->GetText();
			sscanf(str, "%f %f %f", &gaze.x, &gaze.y, &gaze.z);
			camElement = pCamera->FirstChildElement("Up");
			str = camElement->GetText();
			sscanf(str, "%f %f %f", &up.x, &up.y, &up.z);
			camElement = pCamera->FirstChildElement("NearPlane");
			str = camElement->GetText();
			sscanf(str, "%f %f %f %f", &imgPlane.left, &imgPlane.right, &imgPlane.bottom, &imgPlane.top);
			camElement = pCamera->FirstChildElement("NearDistance");
			eResult = camElement->QueryFloatText(&imgPlane.distance);
			camElement = pCamera->FirstChildElement("NumSamples");
			if(camElement!= nullptr)
				eResult = camElement->QueryIntText(&numberofSample);

			camElement = pCamera->FirstChildElement("FocusDistance");
			if (camElement != nullptr)
				eResult = camElement->QueryFloatText(&focusDistance);

			camElement = pCamera->FirstChildElement("ApertureSize");
			if (camElement != nullptr)
				eResult = camElement->QueryFloatText(&apertureSize);
			
			camElement = pCamera->FirstChildElement("ImageResolution");
			str = camElement->GetText();
			sscanf(str, "%d %d", &imgPlane.nx, &imgPlane.ny);
			camElement = pCamera->FirstChildElement("ImageName");
			str = camElement->GetText();
			strcpy(imageName, str);
			float keyValue = -1, burnPercent = -1, saturation = -1, gamma = -1;
			TMO tmo = Photographic;
			camElement = pCamera->FirstChildElement("Tonemap");
			if (camElement)
			{

				string tmoStr;
				XMLElement* TMO = camElement->FirstChildElement("TMO");
				if (TMO != nullptr)
				{
					tmoStr = TMO->GetText();

				}
				XMLElement* TMOOptions = camElement->FirstChildElement("TMOOptions");
				if (TMOOptions != nullptr)
				{
					str = TMOOptions->GetText();

					sscanf(str, "%f %f", &keyValue, &burnPercent);
					
				}
				XMLElement* Saturation = camElement->FirstChildElement("Saturation");
				if (Saturation != nullptr)
				{
					str = Saturation->GetText();

					sscanf(str, "%f", &saturation);

				}
				XMLElement* Gamma = camElement->FirstChildElement("Gamma");
				if (Gamma != nullptr)
				{
					str = Gamma->GetText();

					sscanf(str, "%f", &gamma);

				}
			}
			cameras.push_back(new Camera(id, imageName, pos, gaze, up, imgPlane, numberofSample,apertureSize,focusDistance,keyValue, burnPercent,
				saturation, gamma, tmo));
		}


		pCamera = pCamera->NextSiblingElement("Camera");
	}
}

void Scene::readMaterials(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot)
{
	pElement = pRoot->FirstChildElement("Materials");
	XMLElement* pMaterial = pElement->FirstChildElement("Material");
	XMLElement* materialElement;
	while (pMaterial != nullptr)
	{
		materials.push_back(new Material());

		int curr = materials.size() - 1;

		eResult = pMaterial->QueryIntAttribute("id", &materials[curr]->id);
		
		const char* type = pMaterial->Attribute("type");
		const char* deGamma = pMaterial->Attribute("degamma");
		
		if (type == nullptr)
			materials[curr]->materialType = Default;
		else if (type[0] == 'm')
			materials[curr]->materialType = Mirror;
		else if (type[0] == 'c')
			materials[curr]->materialType = Conductor;
		else if (type[0] == 'd')
			materials[curr]->materialType = Dialectic;
		materialElement = pMaterial->FirstChildElement("AmbientReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->ambientRef.r, &materials[curr]->ambientRef.g, &materials[curr]->ambientRef.b);
		materialElement = pMaterial->FirstChildElement("DiffuseReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->diffuseRef.r, &materials[curr]->diffuseRef.g, &materials[curr]->diffuseRef.b);
		materialElement = pMaterial->FirstChildElement("SpecularReflectance");
		str = materialElement->GetText();
		sscanf(str, "%f %f %f", &materials[curr]->specularRef.r, &materials[curr]->specularRef.g, &materials[curr]->specularRef.b);



		if (deGamma != nullptr)
		{
			if (deGamma[0] == 't')
			{
				float gamma = cameras[0]->gamma;
				float x = std::pow(materials[curr]->ambientRef.r, gamma);
				float y = std::pow(materials[curr]->ambientRef.g, gamma);
				float z = std::pow(materials[curr]->ambientRef.b, gamma);
				float x2 = std::pow(materials[curr]->diffuseRef.r, gamma);
				float y2 = std::pow(materials[curr]->diffuseRef.g, gamma);
				float z2 = std::pow(materials[curr]->diffuseRef.b, gamma);
				float x3 = std::pow(materials[curr]->specularRef.r, gamma);
				float y3 = std::pow(materials[curr]->specularRef.g, gamma);
				float z3 = std::pow(materials[curr]->specularRef.b, gamma);
				materials[curr]->ambientRef = glm::vec3(x,y,z);
				materials[curr]->diffuseRef = glm::vec3(x2,y2,z2);
				materials[curr]->specularRef= glm::vec3(x3,y3,z3);
	
			}
		}
		
		materialElement = pMaterial->FirstChildElement("MirrorReflectance");
		if (materialElement != nullptr)
		{
			str = materialElement->GetText();
			sscanf(str, "%f %f %f", &materials[curr]->mirrorRef.r, &materials[curr]->mirrorRef.g, &materials[curr]->mirrorRef.b);
			materialElement = pMaterial->FirstChildElement("Roughness");
			if(materialElement != nullptr)
			{
				materialElement->QueryFloatText(&materials[curr]->roughness);
			}

		}


		materialElement = pMaterial->FirstChildElement("AbsorptionCoefficient");
		if (materialElement != nullptr)
		{
			str = materialElement->GetText();
			sscanf(str, "%f %f %f", &materials[curr]->absorptionCoefficient.r, &materials[curr]->absorptionCoefficient.g, &materials[curr]->absorptionCoefficient.b);
		}


		materialElement = pMaterial->FirstChildElement("PhongExponent");
		if (materialElement != nullptr)
			materialElement->QueryIntText(&materials[curr]->phongExp);

		materialElement = pMaterial->FirstChildElement("RefractionIndex");
		if (materialElement != nullptr)
			materialElement->QueryFloatText(&materials[curr]->refractionIndex);

		materialElement = pMaterial->FirstChildElement("AbsorptionIndex");
		if (materialElement != nullptr)
			materialElement->QueryFloatText(&materials[curr]->absorptionIndex);


		pMaterial = pMaterial->NextSiblingElement("Material");
	}
}

void Scene::readTransformations(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot)
{
	transformationEnabled = false;
	pElement = pRoot->FirstChildElement("Transformations");
	if (pElement != nullptr)
	{
		transformationEnabled = true;
		XMLElement* translation = pElement->FirstChildElement("Translation");
		while (translation != nullptr)
		{
			int id;
			eResult = translation->QueryIntAttribute("id", &id);
			glm::vec3 trans;
			str = translation->GetText();
			sscanf(str, "%f %f %f", &trans.x,
			       &trans.y, &trans.z);
			//cout << trans.x << " " << trans.y << " " << trans.z << endl;
			translationList.push_back(trans);

			translation = translation->NextSiblingElement("Translation");
		}
		XMLElement* scaling = pElement->FirstChildElement("Scaling");
		while (scaling != nullptr)
		{
			int id;
			eResult = scaling->QueryIntAttribute("id", &id);

			glm::vec3 trans;
			str = scaling->GetText();
			sscanf(str, "%f %f %f", &trans.x,
			       &trans.y, &trans.z);

			scalingList.push_back(trans);

			scaling = scaling->NextSiblingElement("Scaling");
		}
		XMLElement* rotation = pElement->FirstChildElement("Rotation");
		while (rotation != nullptr)
		{
			int id;
			eResult = rotation->QueryIntAttribute("id", &id);
			glm::vec4 trans;
			str = rotation->GetText();
			sscanf(str, "%f %f %f %f", &trans.x,
			       &trans.y, &trans.z, &trans.w);
			//cout << trans.x << " " << trans.y << " " << trans.z << " " << trans.w << endl;
			rotationList.push_back(trans);
			rotation = rotation->NextSiblingElement("Rotation");
		}
	}
}

void Scene::readVertices(const char*& str, XMLElement*& pElement, XMLNode* pRoot)
{
	pElement = pRoot->FirstChildElement("VertexData");
	if (pElement == nullptr)
		return;
	int cursor = 0;
	glm::vec3 tmpPoint;
	str = pElement->GetText();
	while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
		cursor++;
	while (str[cursor] != '\0')
	{
		for (int cnt = 0; cnt < 3; cnt++)
		{
			if (cnt == 0)
				tmpPoint.x = atof(str + cursor);
			else if (cnt == 1)
				tmpPoint.y = atof(str + cursor);
			else
				tmpPoint.z = atof(str + cursor);
			while (str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
				cursor++;
			while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;
		}
		vertices.push_back(tmpPoint);
	}
	
}
void Scene::readTextCoord(const char*& str, XMLElement*& pElement, XMLNode* pRoot)
{
	pElement = pRoot->FirstChildElement("TexCoordData");
	if (pElement == nullptr)
		return;
	int cursor = 0;
	glm::vec2 tmpPoint;
	str = pElement->GetText();
	while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
		cursor++;
	while (str[cursor] != '\0')
	{
		for (int cnt = 0; cnt < 2; cnt++)
		{
			if (cnt == 0)
				tmpPoint.x = atof(str + cursor);
			else if (cnt == 1)
				tmpPoint.y = atof(str + cursor);
			while (str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
				cursor++;
			while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;
		}
	
		textCoord.push_back(tmpPoint);
	}
}

vector<std::pair<char, int>> Scene::readTransform(const char*& str, XMLElement*& objElement, XMLElement*& pObject)
{
	vector<std::pair<char, int>> transformList;
	objElement = pObject->FirstChildElement("Transformations");
	if (objElement != nullptr)
	{
		int cursor = 0;

		str = objElement->GetText();
		while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
			cursor++;

		while (str[cursor] != '\0')
		{
			std::pair<char, int> transform;
			transform.first = str[cursor++];
			transform.second = char(str[cursor++]) - '0';

			while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;
			//cout << transform.first << " first " << transform.second << endl;
			transformList.push_back(transform);
		}
	}
	return transformList;
}

void Scene::readMeshes(const char*& str, XMLError& eResult, XMLElement*& pElement, int& idCount, XMLElement*& objElement, XMLElement*& pObject)
{
	pObject = pElement->FirstChildElement("Mesh");
	while (pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;
		int tp1Index=1;
		int tp2Index=1;
		int tp3Index=1;
		int cursor = 0;
		int vertexOffset = 0;
		vector<Triangle*> faces;
		vector<int>* meshIndices = new vector<int>;
	
		int textureOffset = 0;
		glm::vec3 motionBlur = glm::vec3(0);

		eResult = pObject->QueryIntAttribute("id", &id);
		ShadingMode shadingMode;
		const char* shade = pObject->Attribute("shadingMode");
		if (shade == nullptr)
			shadingMode = FlatShading;
		else if (shade[0] == 's')
			shadingMode = SmoothShading;
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);


		int text1 = -1, text2 = -1;
		objElement = pObject->FirstChildElement("Textures");
		if (objElement != nullptr)
		{
			str = objElement->GetText();

			sscanf(str, "%d %d", &text1, &text2);
			text1--;
			if (text2 != -1)
			{
				text2--;
			}

		}
		
		objElement = pObject->FirstChildElement("MotionBlur");
		if(objElement!= nullptr)
		{
			str = objElement->GetText();
			sscanf(str, "%f %f %f", &motionBlur.x, &motionBlur.y, &motionBlur.z);
			transformationEnabled = true;
		}
		vector<std::pair<char, int>> transformList = readTransform(str, objElement, pObject);
		if(transformList.size()!=0)
			transformationEnabled = true;
		objElement = pObject->FirstChildElement("Faces");

	
		eResult =objElement->QueryIntAttribute("textureOffset", &textureOffset);
	
		const char* type = objElement->Attribute("plyFile");
		if (type != nullptr && type[0] == 'p')
		{

			const string filePath = "hw5/" + (string)type;
		
			happly::PLYData plyIn(filePath);
			int currID = vertices.size();
	
			std::vector<std::array<double, 3>> vPos = plyIn.getVertexPositions();
			std::vector<std::vector<int>> fInd = plyIn.getFaceIndices<int>();
			
			Material* material = materials[matIndex - 1];

			for (std::array<double, 3> vertex : vPos)
			{
				vertices.push_back(glm::vec3{ (float)vertex[0], (float)vertex[1], (float)vertex[2] });
			}
			for (std::vector<int> indices : fInd)
			{
				if(indices.size() == 4)
				{
					p1Index = indices[0] + currID + 1;
					p2Index = indices[1] + currID + 1;
					p3Index = indices[2] + currID + 1;
					int p4Index = indices[3] + currID + 1;
					faces.push_back((new Triangle(id, matIndex - 1,text1,text2, material, p1Index, p2Index, p3Index, &vertices, TriangleShape, transformList, motionBlur,shadingMode)));
					faces.push_back((new Triangle(id, matIndex - 1,text1,text2, material, p3Index, p4Index, p1Index, &vertices, TriangleShape, transformList, motionBlur,shadingMode)));
					meshIndices->push_back(p1Index);
					meshIndices->push_back(p2Index);
					meshIndices->push_back(p3Index);
					meshIndices->push_back(p4Index);
				}
				else
				{
					p1Index = indices[0] + currID + 1;
					p2Index = indices[1] + currID + 1;
					p3Index = indices[2] + currID + 1;

					faces.push_back((new Triangle(id, matIndex - 1,text1,text2, material, p1Index, p2Index, p3Index, &vertices, TriangleShape, transformList, motionBlur,shadingMode)));
					meshIndices->push_back(p1Index);
					meshIndices->push_back(p2Index);
					meshIndices->push_back(p3Index);
				}
				
			}
			objects.push_back(new Mesh(idCount++, matIndex - 1,text1,text2, material, faces, meshIndices, &vertices, MeshType, transformList,motionBlur,shadingMode));
		}
		else
		{

			objElement->QueryIntAttribute("vertexOffset", &vertexOffset);
			str = objElement->GetText();
			while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;
			while (str[cursor] != '\0')
			{
				for (int cnt = 0; cnt < 3; cnt++)
				{
					if (cnt == 0)
					{
						p1Index = atoi(str + cursor) + vertexOffset;
						tp1Index = atoi(str + cursor) + textureOffset;
					}
					else if (cnt == 1)
					{
						p2Index = atoi(str + cursor) + vertexOffset;
						tp2Index = atoi(str + cursor) + textureOffset;
					}
					else
					{
						p3Index = atoi(str + cursor) + vertexOffset;
						tp3Index = atoi(str + cursor) + textureOffset;
					}
					while (str[cursor] != ' ' && str[cursor] != '\t' && str[cursor] != '\n')
						cursor++;
					while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
						cursor++;
				}

				Material* material = materials[matIndex - 1];
				Triangle* tris = (new Triangle(id, matIndex - 1, text1, text2, material, p1Index, p2Index, p3Index, &vertices, TriangleShape, transformList, motionBlur, shadingMode));
				tris->textPoint1 = tp1Index-1;
				tris->textPoint2 = tp2Index-1;
				tris->textPoint3 = tp3Index-1;
				faces.push_back(tris);
				meshIndices->push_back(p1Index);
				meshIndices->push_back(p2Index);
				meshIndices->push_back(p3Index);
			}

			Material* material = materials[matIndex - 1];
			objects.push_back(new Mesh(idCount++, matIndex - 1,text1,text2, material, faces, meshIndices, &vertices, MeshType, transformList,motionBlur,shadingMode));
			

		}
		pObject = pObject->NextSiblingElement("Mesh");
	}
}

void Scene::readSpheres(const char*& str, XMLError& eResult, XMLElement*& pElement, int& idCount, XMLElement*& objElement, XMLElement*& pObject)
{
	pObject = pElement->FirstChildElement("Sphere");

	while (pObject != nullptr)
	{
		int id;
		int matIndex;
		int cIndex;
		float R;

		glm::vec3 motionBlur = glm::vec3(0);

		eResult = pObject->QueryIntAttribute("id", &id);

		ShadingMode shadingMode;
		const char* shade = pObject->Attribute("shadingMode");
		if (shade == nullptr)
			shadingMode = FlatShading;
		else if (shade[0] == 's')
			shadingMode = SmoothShading;
		
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);

		int text1 = -1, text2 = -1;
		objElement = pObject->FirstChildElement("Textures");
		if (objElement != nullptr)
		{
			str = objElement->GetText();

			sscanf(str, "%d %d", &text1, &text2);
			text1--;
			if (text2!= -1)
			{
				text2--;
			}

		}
		
		objElement = pObject->FirstChildElement("MotionBlur");
		if (objElement != nullptr)
		{
			str = objElement->GetText();
		
			sscanf(str, "%f %f %f", &motionBlur.x, &motionBlur.y, &motionBlur.z);
			transformationEnabled = true;

		}
		vector<std::pair<char, int>> transformList;
		objElement = pObject->FirstChildElement("Transformations");
		if (objElement != nullptr)
		{
			int cursor = 0;

			str = objElement->GetText();
			while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;

			while (str[cursor] != '\0')
			{
				std::pair<char, int> transform;
				transform.first = str[cursor++];
				transform.second = char(str[cursor++]) - '0';

				while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
					cursor++;
				
				transformList.push_back(transform);
			}
		}
		if (transformList.size() != 0)
			transformationEnabled = true;

		objElement = pObject->FirstChildElement("Center");
		eResult = objElement->QueryIntText(&cIndex);
		objElement = pObject->FirstChildElement("Radius");
		eResult = objElement->QueryFloatText(&R);
		Material* material = materials[matIndex - 1];
		objects.push_back(new Sphere(idCount++, matIndex - 1,text1,text2, material, cIndex, R, &vertices, SphereType, transformList,motionBlur,shadingMode));

		pObject = pObject->NextSiblingElement("Sphere");
	}
}

void Scene::readTriangles(const char*& str, XMLError& eResult, XMLElement*& pElement, int& idCount, XMLElement*& objElement, XMLElement*& pObject)
{
	pObject = pElement->FirstChildElement("Triangle");
	while (pObject != nullptr)
	{
		int id;
		int matIndex;
		int p1Index;
		int p2Index;
		int p3Index;

		glm::vec3 motionBlur = glm::vec3(0);
		eResult = pObject->QueryIntAttribute("id", &id);
		ShadingMode shadingMode;
		const char* shade = pObject->Attribute("shadingMode");
		if (shade == nullptr)
			shadingMode = FlatShading;
		else if (shade[0] == 's')
			shadingMode = SmoothShading;
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);


		int text1 = -1, text2 = -1;
		objElement = pObject->FirstChildElement("Textures");
		if (objElement != nullptr)
		{
			str = objElement->GetText();

			sscanf(str, "%d %d", &text1, &text2);
			text1--;
			if (text2 != -1)
			{
				text2--;
			}

		}
		objElement = pObject->FirstChildElement("MotionBlur");
		if (objElement != nullptr)
		{
			str = objElement->GetText();
	
			sscanf(str, "%f %f %f", &motionBlur.x, &motionBlur.y, &motionBlur.z);
			transformationEnabled = true;

		}
		vector<std::pair<char, int>> transformList;
		objElement = pObject->FirstChildElement("Transformations");
		if (objElement != nullptr)
		{
			int cursor = 0;

			str = objElement->GetText();
			while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
				cursor++;

			while (str[cursor] != '\0')
			{
				std::pair<char, int> transform;
				transform.first = str[cursor++];
				transform.second = char(str[cursor++]) - '0';

				while (str[cursor] == ' ' || str[cursor] == '\t' || str[cursor] == '\n')
					cursor++;
		
				transformList.push_back(transform);
			}
		}
		if (transformList.size() != 0)
			transformationEnabled = true;
		objElement = pObject->FirstChildElement("Indices");
		str = objElement->GetText();
		sscanf(str, "%d %d %d", &p1Index, &p2Index, &p3Index);
		Material* material = materials[matIndex - 1];
		objects.push_back(new Triangle(idCount++, matIndex - 1,text1,text2, material, p1Index, p2Index, p3Index, &vertices, TriangleShape, transformList,motionBlur,shadingMode));

		pObject = pObject->NextSiblingElement("Triangle");
	}
}

void Scene::readInstances(const char*& str, XMLError& eResult, XMLElement*& pElement, int& idCount,
	XMLElement*& objElement, XMLElement*& pObject)
{
	pObject = pElement->FirstChildElement("MeshInstance");
	while (pObject != nullptr)
	{
		int id, baseMeshId;
		int matIndex;
		bool resetTransform;

		glm::vec3 motionBlur = glm::vec3(0);
		eResult = pObject->QueryIntAttribute("id", &id);
		ShadingMode shadingMode;
		const char* shade = pObject->Attribute("shadingMode");
		if (shade == nullptr)
			shadingMode = FlatShading;
		else if (shade[0] == 's')
			shadingMode = SmoothShading;
		eResult = pObject->QueryIntAttribute("baseMeshId", &baseMeshId);
		eResult = pObject->QueryBoolAttribute("resetTransform", &resetTransform);
		objElement = pObject->FirstChildElement("Material");
		eResult = objElement->QueryIntText(&matIndex);

		int text1 = -1, text2 = -1;
		objElement = pObject->FirstChildElement("Textures");
		if (objElement != nullptr)
		{
			str = objElement->GetText();

			sscanf(str, "%d %d", &text1, &text2);
			text1--;
			if (text2 != -1)
			{
				text2--;
			}

		}
		objElement = pObject->FirstChildElement("MotionBlur");
		if (objElement != nullptr)
		{
			str = objElement->GetText();

			sscanf(str, "%f %f %f", &motionBlur.x, &motionBlur.y, &motionBlur.z);

			transformationEnabled = true;
		}
		vector<std::pair<char, int>> transformList = readTransform(str, objElement, pObject);
		Material* material = materials[matIndex - 1];
		Mesh* lastID = (Mesh*)objects[objects.size() - 1];
		int meshCount = lastID->faces[0]->id;
		Mesh* targetMesh = (Mesh*)objects[objects.size() - 1 - (meshCount - baseMeshId)];
		if (resetTransform == false)
		{
			vector<std::pair<char, int>> newtransformList;
			for (std::pair<char, int> element : targetMesh->transformations)
			{
				newtransformList.push_back(element);
			}
			for (std::pair<char, int> element : transformList)
			{
				newtransformList.push_back(element);
			}
			objects.push_back(new Mesh(idCount++, matIndex - 1,text1,text2, material, targetMesh->faces, nullptr,
				&vertices, MeshType, newtransformList, motionBlur,shadingMode));
		}
		else
			objects.push_back(new Mesh(idCount++, matIndex - 1,text1,text2, material, targetMesh->faces, nullptr,
				&vertices, MeshType, transformList, motionBlur,shadingMode));
		pObject = pObject->NextSiblingElement("MeshInstance");
	
	}
}

void Scene::readObjects(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot)
{
	// Parse objects
	pElement = pRoot->FirstChildElement("Objects");
	int idCount = 0;
	XMLElement* objElement;
	// Parse spheres

	//readPly("hw2/ply/dragon_remeshed.ply");
	// Parse meshes
	XMLElement* pObject;
	
	readMeshes(str, eResult, pElement, idCount, objElement, pObject);
	// Read instances
	readInstances(str, eResult, pElement, idCount, objElement, pObject);
	// Parse spheres
	readSpheres(str, eResult, pElement, idCount, objElement, pObject);

	// Parse triangles
	readTriangles(str, eResult, pElement, idCount, objElement, pObject);

	

}

void Scene::readLights(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot)
{
	int id;
	glm::vec3 position;
	glm::vec3 intensity;
	pElement = pRoot->FirstChildElement("Lights");

	XMLElement* pLight = pElement->FirstChildElement("AmbientLight");
	XMLElement* lightElement;
	str = pLight->GetText();
	sscanf(str, "%f %f %f", &ambientLight.r, &ambientLight.g, &ambientLight.b);

	pLight = pElement->FirstChildElement("PointLight");
	while (pLight != nullptr)
	{
		eResult = pLight->QueryIntAttribute("id", &id);
		lightElement = pLight->FirstChildElement("Position");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &position.x, &position.y, &position.z);
		lightElement = pLight->FirstChildElement("Intensity");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &intensity.r, &intensity.g, &intensity.b);

		lights.push_back(new PointLight(position, intensity));

		pLight = pLight->NextSiblingElement("PointLight");
	}
	pLight = pElement->FirstChildElement("DirectionalLight");
	while (pLight != nullptr)
	{

		lightElement = pLight->FirstChildElement("Direction");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &position.x, &position.y, &position.z);
		lightElement = pLight->FirstChildElement("Radiance");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &intensity.r, &intensity.g, &intensity.b);

		directionalLights.push_back(new DirectionalLight(position, intensity));

		pLight = pLight->NextSiblingElement("DirectionalLight");
	}
	
	pLight = pElement->FirstChildElement("SpotLight");
	while (pLight != nullptr)
	{
		glm::vec3 direction;
		float angle1;
		float angle2;
		lightElement = pLight->FirstChildElement("Position");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &position.x, &position.y, &position.z);
		lightElement = pLight->FirstChildElement("Direction");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &direction.r, &direction.g, &direction.b);
		lightElement = pLight->FirstChildElement("Intensity");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &intensity.r, &intensity.g, &intensity.b);
		lightElement = pLight->FirstChildElement("CoverageAngle");
		str = lightElement->GetText();
		sscanf(str, "%f",&angle1);
		lightElement = pLight->FirstChildElement("FalloffAngle");
		str = lightElement->GetText();
		sscanf(str, "%f", &angle2);

		spotLights.push_back(new SpotLight(position, direction,intensity,angle1/2,angle2/2));

		pLight = pLight->NextSiblingElement("SpotLight");
	}
	
	pLight = pElement->FirstChildElement("AreaLight");
	while (pLight != nullptr)
	{
		glm::vec3 normal;
		float size;

		lightElement = pLight->FirstChildElement("Position");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &position.x, &position.y, &position.z);
		lightElement = pLight->FirstChildElement("Normal");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &normal.r, &normal.g, &normal.b);
		lightElement = pLight->FirstChildElement("Radiance");
		str = lightElement->GetText();
		sscanf(str, "%f %f %f", &intensity.r, &intensity.g, &intensity.b);
		lightElement = pLight->FirstChildElement("Size");
		str = lightElement->GetText();
		sscanf(str, "%f", &size);


		areaLights.push_back(new AreaLight(position, normal, intensity, size));

		pLight = pLight->NextSiblingElement("AreaLight");
	}




}
void Scene::readTextureXml(const char*& str, XMLError& eResult, XMLElement*& pElement, XMLNode* pRoot)
{

	pElement = pRoot->FirstChildElement("Textures");
	if (pElement != nullptr)
	{
	
		
		XMLElement* images = pElement->FirstChildElement("Images");
		if(images != nullptr)
		{
			XMLElement* image = images->FirstChildElement("Image");
			while (image != nullptr)
			{
				int id;
				eResult = image->QueryIntAttribute("id", &id);

				str = image->GetText();
				readTexture(str, id - 1);
				image = image->NextSiblingElement("Image");
			}
			
		}
	

		XMLElement* maps = pElement->FirstChildElement("TextureMap");
		XMLElement* mapElement;
		while (maps != nullptr)
		{
			int id;
			int imageID = -1;
			TextureType textureType = StandardTexture;
			InterpolationType interpolationType = Bilinear;
			DecalMode decalMode = ReplaceKD;
			NoiseConversion noiseConversion = Absval;
			float bumpFactor = 1;
			int normalizer= 255;
			float noiseScale = 1;
			eResult = maps->QueryIntAttribute("id", &id);
			const char* type = maps->Attribute("type");
			
			if (type[0] == 'i')
			{
				textureType = StandardTexture;
			}
			else
			{
				textureType = Perlin;
			}
			mapElement = maps->FirstChildElement("ImageId");
			if(mapElement!= nullptr)
			{
				str = mapElement->GetText();
				sscanf(str, "%d", &imageID);
			}
		

			
			mapElement = maps->FirstChildElement("Interpolation");
			
			if(mapElement != nullptr)
			{
				str = mapElement->GetText();
				if (str[0] == 'b')
				{
					interpolationType = Bilinear;
				}
				else
				{
					interpolationType = Nearest;
				}
			}

			mapElement = maps->FirstChildElement("DecalMode");
			
			if (mapElement != nullptr)
			{
				str = mapElement->GetText();
				if (str[0] == 'r' && str[8] == 'k')
				{
					decalMode = ReplaceKD;
				}
				else if (str[0] == 'r' && str[8] == 'n')
				{
					decalMode = ReplaceNormal;
				}
				else if (str[0] == 'r' && str[8] == 'a')
				{
					decalMode = ReplaceALL;
				}
				else if (str[0] == 'b' && str[1] == 'l')
				{
					decalMode = BlendKD;
				}
				else if (str[0] == 'b' && str[1] == 'u')
				{
					decalMode = BumpNormal;
					mapElement = maps->FirstChildElement("BumpFactor");
					if(mapElement)
					{
						str = mapElement->GetText();
						sscanf(str, "%f",&bumpFactor);
					
					}
				}
				else if (str[0] == 'r' && str[8] == 'b')
				{
					decalMode = ReplaceBG;
				}
			}
			mapElement = maps->FirstChildElement("Normalizer");
			if(mapElement != nullptr)
			{
				str = mapElement->GetText();
				sscanf(str, "%d", &normalizer);
			}
			if( textureType == Perlin)
			{
				mapElement = maps->FirstChildElement("NoiseScale");
				if (mapElement != nullptr)
				{
					str = mapElement->GetText();
					sscanf(str, "%f", &noiseScale);
				}

				mapElement = maps->FirstChildElement("NoiseConversion");
				if (mapElement != nullptr)
				{
					str = mapElement->GetText();
					if (str[0] == 'l')
						noiseConversion = Linear;
				}
			}
			//texture type
			
			
			if(imageID != -1)
				textureMaps.push_back(new TextureMap(id - 1,textures[imageID-1], textureType, interpolationType, decalMode, normalizer, bumpFactor, noiseConversion, noiseScale));
			else
			{
				textureMaps.push_back(new TextureMap(id - 1, nullptr, textureType, interpolationType, decalMode, normalizer, bumpFactor, noiseConversion, noiseScale));
			}
			maps = maps->NextSiblingElement("TextureMap");
		}
	
	}
	
}

void Scene::readExr(const char* input)
{
	float* out; // width * height * RGBA
	int width;
	int height;
	const char* err = NULL; // or nullptr in C++11

	int ret = LoadEXR(&out, &width, &height, input, &err);
	if (ret != TINYEXR_SUCCESS) {
		if (err) {
			fprintf(stderr, "ERR : %s\n", err);
			FreeEXRErrorMessage(err); // release memory of error message.
		}
	}
	else {
		cout << "succss" << endl;
		free(out); // release memory of image data
	}
}

void Scene::readXML(const char* xmlPath)
{
	const char* str;
	XMLDocument xmlDoc;
	XMLError eResult;
	XMLElement* pElement;

	maxRecursionDepth = 1;
	shadowRayEps = 0.001;
	numberofSample = 0;
	eResult = xmlDoc.LoadFile(xmlPath);

	XMLNode* pRoot = xmlDoc.FirstChild();
	
	readConstants(str, eResult, pElement, pRoot);
	// Parse cameras
	readCamera(str, eResult, pElement, pRoot);

	// Parse materals
	readMaterials(str, eResult, pElement, pRoot);
	
	// Parse transformations
	readTransformations(str, eResult, pElement, pRoot);


	// Parse vertex data
	readVertices(str, pElement, pRoot);
	
	readTextCoord(str, pElement, pRoot);

	readObjects(str, eResult, pElement, pRoot);
	
	// Parse lights
	readLights(str, eResult, pElement, pRoot);
	// Read textures
	readTextureXml(str, eResult, pElement, pRoot);

}

void Scene::BoundingBoxIntersection(Ray ray, Node* node, IntersectionInfo* retVal)
{
	if (transformationEnabled == true )
	{
		float t_min = retVal->t;

		for (int i = 0; i < node->ObjectIDs.size(); i++)
		{
			 Shape* shape = objects[node->ObjectIDs[i]];
			Ray rayTransformed = Ray(ray.origin, ray.direction);
			if (glm::length(shape->motionBlur) != 0)
			{
				// Motion blur
				float motionTime = randomTime();

				rayTransformed.origin -= motionTime * shape->motionBlur;

			}
			if(shape->transformations.size() != 0)
				rayTransformed = shape->applyTransform(rayTransformed);
			
			

			if (shape->bounds->isIntersect(rayTransformed) <= t_min)
			{
				IntersectionInfo intesectionInfo = shape->intersect(ray, &rayTransformed);
				if (intesectionInfo.isIntersect == true)
				{
					if (intesectionInfo.t <= t_min)
					{
						t_min = intesectionInfo.t;
						intesectionInfo.objectID = node->ObjectIDs[i];
						*retVal = intesectionInfo;
					}
				}
			}
		}
	}

	else
	{
		float t_min = retVal->t;
		float t_int = node->boundingBox->isIntersect(ray);
		if (t_int != -1 && t_int <= t_min)
		{
			if (node->left == nullptr && node->right == nullptr)
			{
				
				 Shape* shape = objects[node->ObjectIDs[0]];

				 Ray rayTransformed = Ray(ray.origin, ray.direction);
				 if (glm::length(shape->motionBlur) != 0)
				 {
					 // Motion blur
					 float motionTime = randomTime();

					 rayTransformed.origin -= motionTime * shape->motionBlur;

				 }
				if (shape->bounds->isIntersect(rayTransformed) <= t_min)
				{
					

					IntersectionInfo intesectionInfo = shape->intersect(ray,&rayTransformed);
					if (intesectionInfo.isIntersect == true)
					{
						if (intesectionInfo.t <= t_min)
						{
							t_min = intesectionInfo.t;
							intesectionInfo.objectID = node->ObjectIDs[0];
							*retVal = intesectionInfo;
						}
					}
				}
			}
			if (node->left != nullptr)
			{
				BoundingBoxIntersection(ray, node->left, retVal);
			}

			if (node->right != nullptr)
			{
				BoundingBoxIntersection(ray, node->right, retVal);
			}
		}
	}
	/**/
}

IntersectionInfo Scene::closestObject(Ray ray)
{
	IntersectionInfo returnValue{};
	returnValue.t = 9999;
	
	Node* node = boundingVolume->root;
	BoundingBoxIntersection(ray, node, &returnValue);
	

	return  returnValue;
}

void Scene::initMatrices()
{
	for (int i = 0; i < translationList.size(); i++)
	{
		glm::mat4x4* translationMatrix = new glm::mat4x4(0);
		(*translationMatrix)[3][3] = 1;
		(*translationMatrix)[0][0] = 1;
		(*translationMatrix)[1][1] = 1;
		(*translationMatrix)[2][2] = 1;
		(*translationMatrix)[0][3] =-1*translationList[i].x;
		(*translationMatrix)[1][3] =-1*translationList[i].y;
		(*translationMatrix)[2][3] =-1*translationList[i].z;

		translationMatrices.push_back(translationMatrix);
		translationMatrix = new glm::mat4x4(0);
		(*translationMatrix)[3][3] = 1;
		(*translationMatrix)[0][0] = 1;
		(*translationMatrix)[1][1] = 1;
		(*translationMatrix)[2][2] = 1;
		(*translationMatrix)[0][3] = translationList[i].x;
		(*translationMatrix)[1][3] = translationList[i].y;
		(*translationMatrix)[2][3] = translationList[i].z;
		normtranslationMatrices.push_back(translationMatrix);
		
	}
	for (int i = 0; i < scalingList.size(); i++)
	{
		glm::mat4x4* scalingMatrix = new glm::mat4x4(0);

		(*scalingMatrix)[0][0] = 1.0/scalingList[i].x;
		(*scalingMatrix)[1][1] = 1.0/scalingList[i].y;
		(*scalingMatrix)[2][2] = 1.0/scalingList[i].z;
		(*scalingMatrix)[3][3] = 1;
		scalingMatrices.push_back(scalingMatrix);
		scalingMatrix = new glm::mat4x4(0);

		(*scalingMatrix)[0][0] =scalingList[i].x;
		(*scalingMatrix)[1][1] =scalingList[i].y;
		(*scalingMatrix)[2][2] =scalingList[i].z;
		(*scalingMatrix)[3][3] = 1;
		normscalingMatrices.push_back(scalingMatrix);
	}
	for (int i = 0; i < rotationList.size(); i++)
	{
		double cost = cos(-1*rotationList[i][0] * PI / 180.0);
		double sint = sin(-1*rotationList[i][0] * PI / 180.0);
		glm::vec4 u = glm::vec4(rotationList[i][1], rotationList[i][2], rotationList[i][3], -1);
		u = glm::normalize(u);

		glm::mat4x4* rotationMatrix = new glm::mat4x4(0);

		(*rotationMatrix)[0][0] = cost + u.x * u.x * (1 - cost);
		(*rotationMatrix)[0][1] = u.x * u.y * (1 - cost) - u.z * sint;
		(*rotationMatrix)[0][2] = u.x * u.z * (1 - cost) + u.y * sint;

		(*rotationMatrix)[1][0] = u.y * u.x * (1 - cost) + u.z * sint;
		(*rotationMatrix)[1][1] = cost + u.y * u.y * (1 - cost);
		(*rotationMatrix)[1][2] = u.y * u.z * (1 - cost) - u.x * sint;

		(*rotationMatrix)[2][0] = u.z * u.x * (1 - cost) - u.y * sint;
		(*rotationMatrix)[2][1] = u.z * u.y * (1 - cost) + u.x * sint;
		(*rotationMatrix)[2][2] = cost + u.z * u.z * (1 - cost);
		(*rotationMatrix)[3][3] = 1;
		rotationMatrices.push_back(rotationMatrix);

		rotationMatrix = new glm::mat4x4(0);
		cost = cos(rotationList[i][0] * PI / 180.0);
		sint = sin(rotationList[i][0] * PI / 180.0);
		(*rotationMatrix)[0][0] = cost + u.x * u.x * (1 - cost);
		(*rotationMatrix)[0][1] = u.x * u.y * (1 - cost) - u.z * sint;
		(*rotationMatrix)[0][2] = u.x * u.z * (1 - cost) + u.y * sint;

		(*rotationMatrix)[1][0] = u.y * u.x * (1 - cost) + u.z * sint;
		(*rotationMatrix)[1][1] = cost + u.y * u.y * (1 - cost);
		(*rotationMatrix)[1][2] = u.y * u.z * (1 - cost) - u.x * sint;

		(*rotationMatrix)[2][0] = u.z * u.x * (1 - cost) - u.y * sint;
		(*rotationMatrix)[2][1] = u.z * u.y * (1 - cost) + u.x * sint;
		(*rotationMatrix)[2][2] = cost + u.z * u.z * (1 - cost);
		(*rotationMatrix)[3][3] = 1;
		normrotationMatrices.push_back(rotationMatrix);
	}


}

void Scene::toneMapping(Image* image)	
{
	vector<float> luminances;
	float totalLuminance = 0;
	float worldLuminance;

	for (int w = 0 ; w < cameras[0]->imgPlane.nx;w++)
	{
		for (int h = 0; h < cameras[0]->imgPlane.ny; h++)
		{
			glm::vec3 shadingColor = colorMap[w][h];
			float luminance = 0.27 * shadingColor.r + 0.67 * shadingColor.g + 0.06 * shadingColor.b;
			luminances.push_back(luminance);
			totalLuminance += log(luminance + 0.001);
			
		}
	}
	std::sort(luminances.begin(), luminances.end());
	worldLuminance = exp(totalLuminance / (cameras[0]->imgPlane.nx * cameras[0]->imgPlane.ny));
	const float worldDivided = cameras[0]->keyValue / worldLuminance;
	 float lWhite = luminances[((100 - cameras[0]->burnPercent) / 100.0) * luminances.size()];
	/*float** LD;
	LD = new float* [cameras[0]->imgPlane.nx];
	for (int i = 0; i < cameras[0]->imgPlane.nx; ++i)
		LD[i] = new float[cameras[0]->imgPlane.ny];*/


	 float* rgb = new float[3 * cameras[0]->imgPlane.nx * cameras[0]->imgPlane.ny];
	 int i = 0;
	for (int w = 0; w < cameras[0]->imgPlane.nx; w++)
	{
		for (int h = 0; h < cameras[0]->imgPlane.ny; h++)
		{
			glm::vec3 shadingColor = colorMap[w][h];
			float luminance = 0.27 * shadingColor.r + 0.67 * shadingColor.g + 0.06 * shadingColor.b;
			float Lxy = worldDivided * luminance;
			float LD = (Lxy * (1 + (Lxy / (lWhite * lWhite)))) / (1 + Lxy);
			
			float r = pow(shadingColor.r / luminance, cameras[0]->saturation)* LD;
			rgb[3 * i] = r;
			if (r > 1) r = 1;
			r = pow(r, 1.0 / cameras[0]->gamma) ;
			
			r *= 255;
			
			float g = pow(shadingColor.g / luminance, cameras[0]->saturation)* LD;
			rgb[3 * i + 1] = g;
			if (g > 1) g = 1;
			g = pow(g, 1.0 / cameras[0]->gamma);
			
			g *= 255;
			
			float b = pow(shadingColor.b / luminance, cameras[0]->saturation)* LD;
			rgb[3 * i + 2] = b;
			if (b > 1) b = 1;
			b = pow(b, 1.0 / cameras[0]->gamma);
			
			b *= 255;
			
			shadingColor = glm::vec3(r, g, b);
			i++;
			shadingColor.x = shadingColor.x > 255 ? 255 : shadingColor.x;
			shadingColor.y = shadingColor.y > 255 ? 255 : shadingColor.y;
			shadingColor.z = shadingColor.z > 255 ? 255 : shadingColor.z;
			
			Color lastColor = Color{ (unsigned int)shadingColor.x,(unsigned int)shadingColor.y,(unsigned int)shadingColor.z };
			image->setPixelValue(w, h, lastColor);
			
		}
	}
	saveEXR(rgb);

}

void Scene::saveEXR(float* rgb)
{
	int width = cameras[0]->imgPlane.nx, height  = cameras[0]->imgPlane.ny;
	
	EXRHeader header;
	InitEXRHeader(&header);

	EXRImage image;
	InitEXRImage(&image);

	image.num_channels = 3;

	std::vector<float> images[3];
	images[0].resize(width * height);
	images[1].resize(width * height);
	images[2].resize(width * height);

	for (int i = 0; i < width * height; i++) {
		images[0][i] = rgb[3 * i + 0];
		images[1][i] = rgb[3 * i + 1];
		images[2][i] = rgb[3 * i + 2];
	}

	float* image_ptr[3];
	image_ptr[0] = &(images[2].at(0)); // B
	image_ptr[1] = &(images[1].at(0)); // G
	image_ptr[2] = &(images[0].at(0)); // R

	image.images = (unsigned char**)image_ptr;
	image.width = width;
	image.height = height;

	header.num_channels = 3;
	header.channels = (EXRChannelInfo*)malloc(sizeof(EXRChannelInfo) * header.num_channels);
	// Must be BGR(A) order, since most of EXR viewers expect this channel order.
	strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
	strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
	strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

	header.pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
	header.requested_pixel_types = (int*)malloc(sizeof(int) * header.num_channels);
	for (int i = 0; i < header.num_channels; i++) {
		header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
		header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
	}

	const char* err;
	int ret = SaveEXRImageToFile(&image, &header, "exp.exr", &err);
	if (ret != TINYEXR_SUCCESS) {
		fprintf(stderr, "Save EXR err: %s\n", err);
		return;
	}
	printf("Saved exr file. [ %s ] \n", "exp");

	free(rgb);

	free(header.channels);
	free(header.pixel_types);
	free(header.requested_pixel_types);

	return;
}

void Scene::readTexture(const char* fileName,int id)
{
	int width, height, bpp;
	
	const string filePath = "hw4/" + (string)fileName;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &bpp, STBI_rgb);
	if (!data) {
		std::cout << "No File." << std::endl;
		std::cout << fileName << endl;
	}
	else
	{
		std::cout << fileName << " readed" << endl;
		Texture *texture =new Texture(id,height, width, data);
		textures.push_back(texture);
	}
	stbi_image_free(data);
}
