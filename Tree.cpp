#include "Tree.h"

Tree::Tree(int classNum, int maxDepth, int minLeafSample, int minInfoGain) {

	_classNum = classNum;
	_maxDepth = maxDepth;
	_minLeafSample = minLeafSample;
	_minInfoGain = minInfoGain;
	_maxNodeNum = pow(2, _maxDepth) - 1;

	_cTree = new Node*[_maxNodeNum];
	for (int i = 0 ; i < _maxNodeNum; i++) {
		_cTree[i] = NULL;
	}

}

Tree::Tree(int maxNodeNum) {
	
	_maxNodeNum = maxNodeNum;
	_cTree = new Node*[_maxNodeNum];
	for (int i = 0; i < _maxNodeNum; i++) {
		_cTree[i] = NULL;
	}

}

Tree::~Tree() {

	if (_cTree != NULL) {
		for (int i = 0; i < _maxNodeNum; i++) {
			if (_cTree[i] != NULL) {
				delete _cTree[i];
			}
		}
		delete [] _cTree;
		_cTree = NULL;
	}

}

void Tree::generateTree(Sample* sample) {

	_cTree[0] = new Node(sample, _classNum);    //tree head

	for (int i = 0 ; i < _maxNodeNum; i++) {
		if (_cTree[i] == NULL) {
			continue;
		}
		if (_cTree[i]->_isLeaf) {
			continue;
		}
		if (_cTree[i]->_sample->_selectedSampleNum  <= _minLeafSample) {
			_cTree[i]->setLeaf();
			continue;
		}
		if (2*i+2 > _maxNodeNum) {
			_cTree[i]->setLeaf();
			continue;
		}
		_cTree[i]->calculateGainInfo(_cTree, i, _minInfoGain);
	}

}

int Tree::predict(float *data) {

	Node* node = _cTree[0];
	int id = 0;
	while (!node->_isLeaf) {
		id = node->predict(data, id);
		node = _cTree[id];
	}
	// Result result;
	int label = node->_label;
	// result._label = label;
	// result._prob = node->_probs[label];
	return label;

}