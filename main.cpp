
#include <time.h>
#include <cstdio>
#include <iostream>
#include "Scene.h"

Scene* pScene;
int main(int argc, char* argv[])
{


	
	/* Do your stuff here */
	const char* xmlPath;
	xmlPath = "hw4/bump_mapping_transformed.xml";

	clock_t tStart = clock();
	for (int i = 0; i < 1; i++)
	{
		float time = (double)(clock() - tStart) / CLOCKS_PER_SEC;
		pScene = new Scene(xmlPath);
		pScene->initScene();
		
		time = (double)(clock() - tStart) / CLOCKS_PER_SEC;
		printf("Time taken for construction: %.5fs\n", time);
		tStart = clock();
		pScene->renderScene();
		time = (double)(clock() - tStart) / CLOCKS_PER_SEC;
		printf("%.5f time \n", time);
		pScene->convertPPMToPNG(pScene->cameras[0]->imageName, 2);
	}

	return 0;
}
