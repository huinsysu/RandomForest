#include "RandomForest.h"

RandomForest::RandomForest(float** datas, int* label, int treeNum, int classNum, int sampleNum, int featureNum) {
	
	_trainSample = new Sample(datas, label, sampleNum, featureNum);
	_treeNum = treeNum;
	_classNum = classNum;

	_forest = new Tree*[_treeNum];
	for (int i = 0; i < _treeNum; i++) {
		_forest[i] = NULL;
	}

}

RandomForest::RandomForest(const char* modelPath) {

	ifstream fin;
	fin.open(modelPath);
	char* line = new char[200];
	
	int isFirstLine = 1;
	int maxNodeNum = 0;
	int treeIndex = 0;
	int nodeIndex = 0;

	while (fin.getline(line, 200)) {
		if (isFirstLine) {
			isFirstLine = 0;
			char str[20];
			int index = 0;
			int len = strlen(line);
			int paramIndex = 0;
			for (int i= 0; i <= len; i++) {
				if (line[i] == '\t' || i == len) {
					str[index] = '\0';
					int num = atoi(str);
					if (paramIndex == 0) {
						_treeNum = num;
						_forest = new Tree*[_treeNum];
						for (int i = 0; i < _treeNum; i++) {
							_forest[i] = NULL;
						}
					} else if (paramIndex == 1) {
						_classNum = num;
					} else {
						_maxDepth = num;
						maxNodeNum = pow(2, _maxDepth) - 1;
					}
					paramIndex++;
					index = 0;
				} else {
					str[index] = line[i];
					index++;
				}
			}
		} else {
			if (nodeIndex % maxNodeNum == 0) {
				_forest[treeIndex] = new Tree(maxNodeNum);
				treeIndex++;
			}

			char str[50];
			int index = 0;
			int len = strlen(line);
			int paramIndex = 0;

			int isLeaf = -1;
			int featureId = -1;
			float threshold = -1;
			int label = -1;
			int prob = -1;
			int* probs = new int[_classNum];

			for (int i = 0 ; i <= len; i++) {
				if (line[i] == '\t' || i == len) {
					str[index] = '\0';
					if (paramIndex == 0) {
						isLeaf = atoi(str);
						if (isLeaf == -1) {
							break;
						}
					} else if (paramIndex == 1) {
						featureId = atoi(str);
					} else if (paramIndex == 2) {
						threshold = atof(str);
					} else if (paramIndex == 3) {
						label = atoi(str); 
					} else {
						prob = atoi(str);
						probs[paramIndex-4] = prob;
					}
					paramIndex++;
					index = 0;
				} else {
					str[index] = line[i];
					index++;
				}
			}
			if (isLeaf != -1) {
				(_forest[treeIndex-1])->_cTree[nodeIndex] = new Node(isLeaf, featureId, threshold, label, probs, _classNum);
				delete [] probs;
			}
			nodeIndex = (nodeIndex+1) % maxNodeNum;
		}

	}
	delete [] line;
	fin.close();
}

RandomForest::~RandomForest() {

	if (_forest != NULL) {
		for (int i = 0; i < _treeNum; i++) {
			if (_forest[i] != NULL) {
				delete _forest[i];
			}
		}
		delete [] _forest;
	}
	if (_trainSample != NULL) {
		delete _trainSample;
	}

}

void RandomForest::generateForest(int maxDepth, int minLeafSample, float minInfoGain, int trainFeatureNumPerNode, int trainSampleNumPerTree) {

	_maxDepth = maxDepth;
	if (trainFeatureNumPerNode  == -1) {
		_trainFeatureNumPerNode = sqrt(_trainSample->_featureNum);
	} else {
		_trainFeatureNumPerNode = trainFeatureNumPerNode;
	}
	if (trainSampleNumPerTree == -1) {
		_trainSampleNumPerTree = _trainSample->_sampleNum * 0.667;
	} else {
		_trainSampleNumPerTree = trainSampleNumPerTree;
	}

	if (_treeNum < 1) {
		cout << "Tree number must be bigger than 0!" << endl;
		cout << "Generating faild!" << endl;
		return;
	}
	if (_maxDepth < 2) {
		cout << "Max depth must be bigger than 1!" << endl;
		cout << "Generating faild!" << endl;
		return;
	}
	if (minLeafSample < 2) {
		cout << "Minimum samples in a leaf must be bigger than 1!" << endl;
		cout << "Generating faild" << endl;
		return;
	}

	for (int i = 0; i < _treeNum; i++) {
		cout << "Generate Tree " << i << endl;
		_forest[i] = new Tree(_classNum, maxDepth, minLeafSample, minInfoGain);
		Sample* trainSample = new Sample(_trainSample);
		trainSample->randomSelectSample(_trainSampleNumPerTree);
		trainSample->randomSelectFeature(_trainFeatureNumPerNode);
		_forest[i]->generateTree(trainSample);
		trainSample->releaseIndex();
		delete trainSample;
	}
	_trainSample->releaseIndex();
	delete _trainSample;
	_trainSample = NULL;

}

void RandomForest::predict(float** testData, float* results, int testSize) {

	int* probs = new int[_classNum];
	for (int i = 0; i < testSize; i++) {
		for (int j = 0 ; j < _classNum; j++) {
			probs[j] = 0;
		}
		for (int j = 0; j < _treeNum; j++) {
			int label = _forest[j]->predict(testData[i]);
			probs[label]++;
		}
		int maxNum = 0;
		int label = -1;
		for (int j = 0; j < _classNum; j++) {
			if (probs[j] > maxNum) {
				maxNum = probs[j];
				label = j;
			}
		}
		if (label == 1) {
			results[i] = (float)probs[label]/_treeNum;
		} else {
			results[i] = 1 - (float)probs[label]/_treeNum;
		}
	}
	delete [] probs;

}

void RandomForest::saveModel(const char* path) {

	ofstream fout;
	fout.open(path);
	int maxNodeNum = pow(2, _maxDepth) - 1;
	fout << _treeNum << '\t' << _classNum << '\t' << _maxDepth << '\n';
	for (int i = 0 ; i < _treeNum; i++) {
		Node** cTree = _forest[i]->_cTree;
		for (int j = 0 ; j < maxNodeNum; j++) {
			if (cTree[j] == NULL) {
				fout << -1 << '\n';
				continue;
			}
			int isLeaf = 0;
			if (cTree[j]->_isLeaf) {
				isLeaf = 1;
			}
			if (isLeaf) {
				fout << 1 << '\t';
			} else {
				fout << 0 << '\t';
			}
			fout << cTree[j]->_featureId << '\t';
			fout << cTree[j]->_threshold << '\t';
			int label = cTree[j]->_label;
			fout << label << '\t';
			for (int k = 0; k < _classNum; k++) {
				fout << cTree[j]->_probs[k];
				if (k != _classNum - 1) {
					fout << '\t';
				} else {
					fout << '\n';
				}
			}
		}
	}
	fout.close();

}
