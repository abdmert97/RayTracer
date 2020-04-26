#ifndef _BOUNDINGVOLUME_H_
#define _BOUNDINGVOLUME_H_


#include "Scene.h"
#include "Node.h"
class Triangle;
class Shape;
class Node;
class BoundingVolume
{
	private:
		std::vector<Shape*> objects;
		std::vector<Triangle*> triangles;

	public:
		Node* root;
		int level=0;
		BoundingVolume(std::vector<Shape*> _objects)
		{
			root = new Node();
		
		    objects = std::move(_objects);
			for (int i = 0; i < objects.size();i++)
				root->ObjectIDs.push_back(i);
			build(root);

		}
		BoundingVolume(std::vector<Triangle*> triangles)
		{
			root = new Node();
	
			this->triangles = triangles;
			for (int i = 0; i < triangles.size(); i++)
				root->ObjectIDs.push_back(i);
			buildMeshVolume(root);

		}
		bool isInLeft(const glm::vec3& midPoint, BoundingBox* bounding_box);
		bool sortVector(const std::pair<int, glm::vec3>& v1, const std::pair<int, glm::vec3>& v2);



		void build(Node* node);
		void buildMeshVolume(Node* node);

};


#endif

