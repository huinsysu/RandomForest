#ifndef NODE_H
#define NODE_H

#include "Sample.cpp"
#include <algorithm>

struct Pair{

	int _id;
	float _featureValue;

};

class Node
{
public:
	Node(Sample* sample, int classNum);
	Node(int isLeaf, int featureId, float threshold, int label, int* probs, int classNum);
	~Node();

    void sortSample(int featureId);
	void calculateParameter();
	void calculateGainInfo(Node** tree, int id, float minInfoGain);
	void setLeaf();
	int predict(float* data, int id);

	Sample* _sample;
	int _classNum;
	int _featureId;
	float _threshold;
	bool _isLeaf;
	int _label;
	float _gini;
	int* _probs;


};

#endif