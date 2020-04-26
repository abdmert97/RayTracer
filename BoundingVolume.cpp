#include "BoundingVolume.h"


bool  compareVec3(const std::pair<glm::vec3, int>& lhs, const std::pair<glm::vec3, int>& rhs)
{
	return lhs.first.x < rhs.first.x;
}
void BoundingVolume::build(Node* node)
{
	node->boundingBox = new BoundingBox;
	int size = node->ObjectIDs.size();


	for(int i = 0; i< size;i++)
	{
		Shape* shape = objects[node->ObjectIDs[i]];
		BoundingBox *bounds = shape->getBounds();
		node->boundingBox->extend(bounds);
	}
	if (size <= 1) return;
	vector<std::pair<glm::vec3, int>> midpoints;
	for (int i = 0; i < size; i++)
	{
		Shape* shape = objects[node->ObjectIDs[i]];
		BoundingBox* bounds = shape->getBounds();
		midpoints.emplace_back(bounds->midPoint(), node->ObjectIDs[i]);
	}
	std::sort(midpoints.begin(), midpoints.end(),compareVec3);

	
	node->left = new Node;
	node->right = new Node;
	for (int i = 0; i < size; i++)
	{
		if (i<size/2)
		{
			node->left->ObjectIDs.push_back(midpoints[i].second);
		}
		else
			node->right->ObjectIDs.push_back(midpoints[i].second);
	}
	build(node->left);
	build(node->right);
}
void BoundingVolume::buildMeshVolume(Node* node)
{
	node->boundingBox = new BoundingBox;
	int size = node->ObjectIDs.size();
	
	for (int i = 0; i < size; i++)
	{
		Triangle *shape = triangles[node->ObjectIDs[i]];
		BoundingBox* bounds = shape->getBounds();
		node->boundingBox->extend(bounds);
	}
	if (size <= 1) return;
	vector<std::pair<glm::vec3,int>> midpoints;
	for (int i = 0; i < size; i++)
	{
		Triangle *shape = triangles[node->ObjectIDs[i]];
		BoundingBox* bounds = shape->getBounds();
		midpoints.emplace_back(bounds->midPoint(), node->ObjectIDs[i]);
	}

	std::sort(midpoints.begin(), midpoints.end(),compareVec3);


	node->left = new Node();
	
	node->right = new Node();
	
	for (int i = 0; i < size; i++)
	{

		if (i < size / 2)
		{
			node->left->ObjectIDs.push_back(midpoints[i].second);
		}
		else
			node->right->ObjectIDs.push_back(midpoints[i].second);
	}
	buildMeshVolume(node->left);
	buildMeshVolume(node->right);
}
bool BoundingVolume::isInLeft(const glm::vec3& midPoint, BoundingBox* bounding_box)
{
	if (midPoint.x > (bounding_box->min.x + bounding_box->max.x) / 2)
		return true;
	return false;
}


