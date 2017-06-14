#include "Sample.h"

Sample::Sample(float** datas, int* labels, int sampleNum, int featureNum) {

	_dataset = datas;
	_labels = labels;
	_sampleIndex = NULL;
	_featureIndex = NULL;
	_sampleNum = sampleNum;
	_featureNum = featureNum;
	_selectedSampleNum = 0;
	_selectedFeatureNum = 0;

}

Sample::Sample(Sample* sample, int start, int end) {

	_sampleNum = sample->_sampleNum;
	_featureNum = sample->_featureNum;
	_selectedSampleNum = end - start + 1;
	_selectedFeatureNum = sample->_selectedFeatureNum;
	_dataset = sample->_dataset;
	_labels = sample->_labels;

	if (sample->_sampleIndex != NULL) {
		_sampleIndex = new int[_selectedSampleNum];
		for (int i = 0; i < _selectedSampleNum; i++) {
			_sampleIndex[i] = sample->_sampleIndex[start+i];
		}
	} else {
		_sampleIndex = NULL;
	}
	if (sample->_featureIndex != NULL) {
		_featureIndex = new int[_selectedFeatureNum];
		for (int i = 0; i < _selectedFeatureNum; i++) {
			_featureIndex[i] = sample->_featureIndex[i];
		}
	} else {
		_featureIndex = NULL;
	}

}

Sample::Sample(Sample *sample) {

	_sampleNum = sample->_sampleNum;
	_featureNum = sample->_featureNum;
	_selectedSampleNum = sample->_selectedSampleNum;
	_selectedFeatureNum = sample->_selectedFeatureNum;
	_dataset = sample->_dataset;
	_labels = sample->_labels;
	if (sample->_sampleIndex != NULL) {
		_sampleIndex = new int[_selectedSampleNum];
		for (int i = 0; i < _selectedSampleNum; i++) {
			_sampleIndex[i] = sample->_sampleIndex[i];
		}
	} else {
		_sampleIndex = NULL;
	}
	if (sample->_featureIndex != NULL) {
		_featureIndex = new int[_selectedFeatureNum];
		for (int i = 0 ; i < _selectedFeatureNum; i++) {
			_featureIndex[i] = sample->_featureIndex[i];
		}
	} else {
		_featureIndex = NULL;
	}

}

Sample::~Sample() {

	_dataset = NULL;
	_labels = NULL;
	if (_sampleIndex != NULL) {
		delete [] _sampleIndex;
		_sampleIndex = NULL;
	}
	if (_featureIndex != NULL) {
		delete [] _featureIndex;
		_featureIndex = NULL;
	}

}

void Sample::randomSelectSample(int selectedSampleNum) {

	_selectedSampleNum = selectedSampleNum;
	if (_sampleIndex != NULL) {
		delete [] _sampleIndex;
	}
	_sampleIndex = new int[selectedSampleNum];
	srand(time(NULL));
	for (int i = 0; i < selectedSampleNum; i++) {
		_sampleIndex[i] = rand()%_sampleNum;
	}

}

void Sample::randomSelectFeature(int selectedFeatureNum) {
	
	_selectedFeatureNum = selectedFeatureNum;
	if (_featureIndex != NULL) {
		delete [] _featureIndex;
	}
	_featureIndex = new int[selectedFeatureNum];
	srand(time(NULL));
	int i, j;
	for (i = 0, j = _featureNum-selectedFeatureNum; j < _featureNum; i++, j++) {
		int index = rand()%j;
		int flag = 0;
		for (int k = 0 ; k <= i; k++) {
			if (_featureIndex[k] == index) {
				flag = 1;
				break;
			}
		}
		if (flag == 0) {
			_featureIndex[i] = index;
		} else {
			_featureIndex[i] = j;
		}
	}

}
