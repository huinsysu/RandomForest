#ifndef RANDOMFOREST_H
#define RANDOMFOREST_H

#include "Tree.cpp"
#include <fstream>
#include <iostream>
using namespace std;

class RandomForest
{
public:
	RandomForest(float** datas, int* label, int treeNum, int classNum, int sampleNum, int featureNum);
	RandomForest(const char* modelPath);
	~RandomForest();

	void generateForest(int maxDepth, int minLeafSample, float minInfoGain, int trainFeatureNumPerNode, int trainSampleNumPerTree);
	void predict(float** testData, float* result, int testSize);
	void saveModel(const char* path);

	Tree** _forest;
	Sample* _trainSample;
	int _treeNum;
	int _classNum;
	int _maxDepth;
	int _trainFeatureNumPerNode;
	int _trainSampleNumPerTree;
	float _minInfoGain;

};

#endif