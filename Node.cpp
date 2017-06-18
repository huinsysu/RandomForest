#include "Node.h"
#include <iostream>
using namespace std;

int cmp(const void* a, const void* b) {

	Pair* arg1 = (Pair*) a;
	Pair* arg2 = (Pair*) b;
	if (arg1->_featureValue < arg2->_featureValue) return -1;
	else if (arg1->_featureValue == arg2->_featureValue) return 0;
	else return 1;

}

Node::Node(Sample* sample, int classNum) {

	_sample = new Sample(sample);
	_classNum = classNum;
	_featureId = -1;
	_threshold = -1;
	_isLeaf = false;
	_label = -1;
	_gini = -1.0;
	_probs = NULL;

}

Node::Node(int isLeaf, int featureId, float threshold, int label, int* probs, int classNum) {

	_sample = NULL;
	_gini = -1.0;
	if (isLeaf == 1) {
		_isLeaf = true;
	} else {
		_isLeaf = false;
	}
	_classNum = classNum;
	_featureId = featureId;
	_threshold = threshold;
	_label = label;
	_probs = new int[_classNum];
	for (int i = 0; i < _classNum; i++) {
		_probs[i] = probs[i];
	}

}

Node::~Node() {

	if (_sample != NULL) {
		delete _sample;
		_sample = NULL;
	}
	if (_probs != NULL) {
		delete [] _probs;
		_probs = NULL;
	}

}

void Node::sortSample(int featureId) {

    int sampleNum = _sample->_selectedSampleNum;
	Pair* pairs = new Pair[sampleNum];
	for (int i = 0; i < sampleNum; i++) {
		pairs[i]._id = _sample->_sampleIndex[i];
		pairs[i]._featureValue = _sample->_dataset[_sample->_sampleIndex[i]][featureId];
	}
	qsort(pairs, sampleNum, sizeof(Pair), cmp);
	for (int i = 0; i < sampleNum; i++) {
		_sample->_sampleIndex[i] = pairs[i]._id;
	}
	delete [] pairs;
	
}

void Node::calculateParameter() {
	
	_probs = new int[_classNum];
	for (int i = 0 ; i < _classNum; i++) {
		_probs[i] = 0;
	}
	int sampleNum = _sample->_selectedSampleNum;
	for (int i = 0; i < sampleNum; i++) {
		int sampleIndex = _sample->_sampleIndex[i];
		int classIndex = _sample->_labels[sampleIndex];
		_probs[classIndex]++;
	}
	float gini = 0.0;
	for (int i = 0; i < _classNum; i++) {
		float p = (float)_probs[i] / sampleNum;
		gini += p * p;
	}
	_gini = 1.0 - gini;

}

void Node::calculateGainInfo(Node **tree, int id, float minInfoGain) {

	int sampleNum = _sample->_selectedSampleNum;
	int featureNum = _sample->_selectedFeatureNum;

	int* leftProbs = new int[_classNum];
	int* rightProbs = new int[_classNum];


	float maxInfoGain = 0.0;
	int maxSampleIndexOnLeft = -1;
	int maxFeatureId = -1;
	float maxThreshold = 0.0;

	calculateParameter();

	for (int i = 0; i < featureNum; i++) {
		float innerMaxInfoGain = 0.0;
		int innerMaxSampleIndexOnLeft = -1;
		int innerMaxFeatureId = -1;
		float innerMaxThreshold = 0.0;
		
		int featureId = _sample->_featureIndex[i];
		sortSample(featureId);
		for (int j = 0; j < _classNum; j++) {
			leftProbs[j] = 0;
			rightProbs[j] = _probs[j];
		}
		for (int j = 0; j < sampleNum-1; j++) {
			int sampleIndex = _sample->_sampleIndex[j];
			int nextSampleIndex = _sample->_sampleIndex[j+1];
			int classIndex = _sample->_labels[sampleIndex];
			leftProbs[classIndex]++;
			rightProbs[classIndex]--;
			if (_sample->_dataset[nextSampleIndex][featureId] - _sample->_dataset[sampleIndex][featureId] < 0.00001) {
				continue;
			}
			float leftGini = 0.0;
			float rightGini = 0.0;
			for (int k = 0 ; k < _classNum; k++) {
				float p = (float)leftProbs[k] / (j+1);
				leftGini += p * p;
				p = (float)rightProbs[k] / (sampleNum-j-1);
				rightGini += p * p;
			}
			leftGini = 1- leftGini;
			rightGini = 1- rightGini;
			float leftRatio = (1.0+j) / sampleNum;
			float rightRatio = (sampleNum-j-1.0) / sampleNum;
			float infoGain = _gini - leftRatio*leftGini - rightRatio*rightGini;
			if (infoGain > innerMaxInfoGain) {
				innerMaxInfoGain = infoGain;
				innerMaxSampleIndexOnLeft = j;
				innerMaxFeatureId = featureId;
				innerMaxThreshold = _sample->_dataset[sampleIndex][featureId];
			}
		}
		if (innerMaxInfoGain > maxInfoGain) {
			maxInfoGain = innerMaxInfoGain;
			maxSampleIndexOnLeft = innerMaxSampleIndexOnLeft;
			maxFeatureId = innerMaxFeatureId;
			maxThreshold = innerMaxThreshold;
		}
	}

	delete [] leftProbs;
	delete [] rightProbs;	

	if (maxInfoGain < minInfoGain) {
		setLeaf();
	} else {
		_featureId = maxFeatureId;
		_threshold = maxThreshold;
		sortSample(maxFeatureId);
		Sample* leftSample = new Sample(_sample, 0, maxSampleIndexOnLeft);
		Sample* rightSample = new Sample(_sample, maxSampleIndexOnLeft+1, sampleNum-1);
		tree[2*id+1] = new Node(leftSample, _classNum);
		tree[2*id+2] = new Node(rightSample, _classNum);
		leftSample->releaseIndex();
		rightSample->releaseIndex();
		delete leftSample;
		delete rightSample;
	}
	_sample->releaseIndex();
	delete _sample;
	_sample = NULL;

}

void Node::setLeaf() {

	if (_probs == NULL) {
		calculateParameter();
	}

	_isLeaf = true;
	float maxProbs = 0.0;
	int classIndex = 0;
	for (int i = 0; i < _classNum; i++) {
		if (_probs[i] > maxProbs) {
			maxProbs = _probs[i];
			classIndex = i;
		}
	}
	_label = classIndex;

}

int Node::predict(float* data, int id) {

	int featureValue = data[_featureId];
	int nextId = -1;
	if (featureValue <= _threshold) {
		nextId = 2*id+1;
	} else {
		nextId = 2*id+2;
	}
	return nextId;
	
}
