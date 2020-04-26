#ifndef _NODE_H_
#define _NODE_H_


#include "BoundingBox.h"



class BoundingBox;
class Node
{
	
	public:
		BoundingBox *boundingBox;
		Node* left;
		Node* right;
	
		std::vector<int> ObjectIDs;
		Node()
		{
			
	
			left = nullptr;
			right = nullptr;
		}
		Node(Node* left,Node *right)
		{
		
			this->left = left;
			this->right = right;
		}

};
#endif

