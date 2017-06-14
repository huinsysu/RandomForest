#ifndef SAMPLE_H
#define SAMPLE_H

#include <cmath>
#include <cstdlib>
#include <ctime>

class Sample
{
public:
	Sample(float** datas, int* labels, int sampleNum, int featureNum);
	Sample(Sample* sample, int start, int end);
	Sample(Sample* sample);
	~Sample();

	void randomSelectSample(int selectedSampleNum);
	void randomSelectFeature(int selectedFeatureNum);
	
	inline void releaseIndex() {

		if (_sampleIndex != NULL) {
			delete _sampleIndex;
			_sampleIndex = NULL;
		}
		if (_featureIndex != NULL) {
			delete _featureIndex;
			_featureIndex = NULL;
		}

	};

	float** _dataset;
	int* _labels;
	int* _sampleIndex;
	int* _featureIndex;
	
	int _sampleNum;
	int _featureNum;
	int _selectedSampleNum;
	int _selectedFeatureNum;

};

#endif