#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Scene.h"
#include "Ray.h"
#include <random>
class Ray;
// Structure for holding variables related to the image plane
typedef struct ImagePlane
{
	float left;     // "u" coordinate of the left edge
	float right;    // "u" coordinate of the right edge
	float bottom;   // "v" coordinate of the bottom edge
	float top;      // "v" coordinate of the top edge
    float distance; // distance to the camera (always positive)
    int nx;         // number of pixel columns
    int ny;         // number of pixel rows
} ImagePlane;

class Camera
{
public:
  char imageName[32];
  int id;
  ImagePlane imgPlane;     // Image plane
  int sample;
  float apertureSize;
  float focusDistance;
  glm::vec3 imgPostion;
	Camera(int id,                      // Id of the camera
           const char* imageName,       // Name of the output PPM file
           const glm::vec3& pos,         // Camera position
           const glm::vec3& gaze,        // Camera gaze direction
           const glm::vec3& up,          // Camera up direction
           const ImagePlane& imgPlane,
		   int sample, 
		   float apertureSize,
		   float focusDistance); // Image plane parameters

    // Computes the primary ray through pixel (row, col)
	Ray getPrimaryRay(int row, int col, int xSample=0, int ySample=0) const;

private:
   glm::vec3 pos;         // Camera position
   glm::vec3 gaze;        // Camera gaze direction
   glm::vec3 up;
   glm::vec3 u;
   glm::vec3 DotProduct(const glm::vec3& vector1,const glm::vec3& vector2);
   glm::vec3 pixelPositionOnImagePlane(int row, int column, int xSample = 0, int ySampl = 0) const;
   glm::vec3 normalizeVector(glm::vec3 v)const;
   float randomE() const;
 
};

#endif
