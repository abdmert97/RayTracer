#include "Camera.h"

extern Scene* pScene;
using namespace std;
Camera::Camera(int id,                      // Id of the camera
               const char* imageName,       // Name of the output PPM file
               const glm::vec3& pos,         // Camera position
               const glm::vec3& gaze,        // Camera gaze direction
               const glm::vec3& up,          // Camera up direction
               const ImagePlane& imgPlane,
			   int sample,
	           float apertureSize,
	           float focusDistance)  // Image plane parameters
{
    this->id  = id;
    int i = 0;
    while(imageName[i])
    {
        this->imageName[i] = imageName[i];
        i++;
    }
    this->imageName[i] = 0 ;
    this->pos = pos;
    this->gaze = normalizeVector(gaze);
    this->up = normalizeVector(up);
    this->imgPlane = imgPlane;
    this->u = cross(up,gaze);
	u = normalizeVector(u);
	this->sample = sample;
	this->apertureSize = apertureSize;
	this->focusDistance = focusDistance;
	imgPostion = gaze * imgPlane.distance;
}

/* Takes coordinate of an image pixel as row and col, and
 * returns the ray going through that pixel.
 */
float Camera::randomE() const
{
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	uniform_real_distribution<> dis(-0.5, 0.5);
	return dis(gen);
}
Ray Camera::getPrimaryRay(int col, int row, int xSample, int ySample) const
{
    glm::vec3 pixelPosition = pixelPositionOnImagePlane(row,col,xSample,ySample);

    Ray ray = Ray(pos,glm::normalize(pixelPosition-pos));
	if (apertureSize != 0)
	{
		float e1 = randomE() * apertureSize;
		float e2 = randomE() * apertureSize;
		glm::vec3 dir = ray.direction;
		glm::vec3 s = pos + e1 * u + e2 * up;
		float t_fd =  focusDistance / glm::dot(dir, gaze);
		glm::vec3 p = ray.getPoint(t_fd);
		glm::vec3 d = p - s;
		d = glm::normalize(d);
		ray.origin = s;
		ray.direction = d;
	}
	ray.p = pixelPosition;
    return ray;
}

glm::vec3 Camera::pixelPositionOnImagePlane(int row ,int column,int xSample ,int ySample ) const
{

	if(sample != 0)
	{
		
		float x = (float)xSample / sqrt(sample);
		float y = (float)ySample / sqrt(sample);
	
		float s_u = imgPlane.right - (column + x) * ((imgPlane.right - imgPlane.left) / imgPlane.nx);
		float s_v = imgPlane.top - (row + y) * ((imgPlane.top - imgPlane.bottom) / imgPlane.ny);


		glm::vec3 pixelPosition = up * s_v + u * s_u + pos + imgPostion;
		return pixelPosition;
	}
	else
	{
		float s_u = imgPlane.right - (column + 0.5) * ((imgPlane.right - imgPlane.left) / imgPlane.nx);
		float s_v = imgPlane.top - (row + 0.5) * ((imgPlane.top - imgPlane.bottom) / imgPlane.ny);


		glm::vec3 pixelPosition = up * s_v + u * s_u + pos + imgPostion;
		return pixelPosition;
	}
	
		


}

glm::vec3 Camera::normalizeVector(glm::vec3 v)const
{

    float magnitude = sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
    if(magnitude == 0)
    {
        glm::vec3 resultVector = {0,0,0}; // ne olacak bilmiyorum böyle gelirse
         return resultVector;
    }
    else
    {
        glm::vec3 resultVector = {v.x/magnitude , v.y / magnitude, v.z/magnitude};

        return resultVector;
    }

}
