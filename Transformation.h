#ifndef _TRANSFORMATION_H_
#define _TRANSFORMATION_H_

#include "Scene.h"
using namespace std;

// Class for point lights
class Transformation
{
	public:
		vector<glm::vec3> translationList;
		vector<glm::vec3> scalingList;
		vector<glm::vec4> rotationList;
		vector<glm::mat4x4 *> translationMatrices;
		vector<glm::mat4x4 *> scalingMatrices;
		vector<glm::mat4x4 *> rotationMatrices;

		vector<glm::mat4x4*> inverseTranslationMatrices;
		vector<glm::mat4x4*> inverseScalingMatrices;
		vector<glm::mat4x4*> inverseRotationMatrices;
		vector<glm::mat4x4*> NormalTranslationMatrices;
		vector<glm::mat4x4*> NormalScalingMatrices;
		vector<glm::mat4x4*> NormalRotationMatrices;
		glm::vec4 translation(int id,glm::vec4 position);
		glm::vec4 scaling(int id,glm::vec4 scale);
		glm::vec4 rotation(int id,glm::vec4 rotationList);
		glm::vec4 inverseTranslation(int id, glm::vec4 position);
		glm::vec3 inverseRotation(int id, glm::vec4 _rotation);
		glm::vec4 inverseScaling(int id, glm::vec4 scale);
		void initMatrices();
		glm::vec4 translationNormal(int id, glm::vec4 position);
		glm::vec4 scalingNormal(int id, glm::vec4 scale);
		glm::vec4 rotationNormal(int id, glm::vec4 rotationList);
		glm::vec4 multiplyMatrixWithVec4(glm::mat4x4 m, glm::vec4 v)
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
		glm::mat4x4 multiplyMatrixWithMatrix(glm::mat4x4 m,glm::mat4x4 m2)
		{
			double values[4];
			double total;
			glm::mat4x4 result = glm::mat4x4(0);
			for (int i = 0; i < 4; i++)
			{
				total = 0;
				for (int j = 0; j < 4; j++)
				{
					for (int k = 0; k < 4 ;k++)
					{
						result[i][j] += m[i][k] *m2[k][j];
					}

					
				}
			
			}

			return result;
		}
		glm::mat4x4 transposeMatrix(glm::mat4x4 m)
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
};


#endif
