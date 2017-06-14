#ifndef TREE_H
#define TREE_H

#include "Node.cpp"

struct Result{

	int _label;
	float _prob;
	
};

class Tree
{
public:
	Tree(int classNum, int maxDepth, int minLeafSample, int minInfoGain);
	Tree(int _maxNodeNum);
	~Tree();

	void generateTree(Sample* sample);
	int predict(float* data);
	
	Node** _cTree;
	int _classNum;
	int _maxDepth;
	int _maxNodeNum;
	int _minLeafSample;
	int _minInfoGain;

};

#endif