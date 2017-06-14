#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include "RandomForest.cpp"

using namespace std;

#define line_len 2000
#define thread_num 10

struct Param {

	float** _datas;
	int* _labels;
	int _index;

};

void* threadFn(void* ptr) {

	Param* param = (Param*)ptr;
	float** datas = param->_datas;
	int* labels = param->_labels;
	int index = param->_index;

	RandomForest* rf = new RandomForest(datas, labels, 100, 2, 1866819, 201);
	rf->generateForest(11, 500, 0.00001, -1, -1);

	stringstream ss;
	string str;
	ss << index;
	ss >> str;
	str = "./models/model" + str + ".txt";
	const char* modelName = str.c_str();

	rf->saveModel(modelName);

}

int main(int argc, char* argv[]) {

	float** train_datas = new float*[1866819];
	int* train_labels = new int[1866819];
	for (int i = 0; i < 1866819; i++) {
		train_datas[i] = new float[201];
		for (int j = 0 ; j < 201; j++) {
			train_datas[i][j] = 0.0;
		}
	}

	char* line = new char[line_len];
	ifstream fin;
	fin.open("train_data.txt");
	int data_index = 0;
	while (fin.getline(line, line_len)) {
		char str[50];
		int len = strlen(line);
		int index = 0;
		int is_label = 1;
		int feature_index = -2;
		for (int i = 0 ; i <= len; i++) {
			if (line[i] == ' ' && is_label == 1) {
				str[index] = '\0';
				int label = atoi(str);
				train_labels[data_index] = label;
				index = 0;
				is_label = 0;
			} else if (line[i] == ':') {
				str[index] = '\0';
				feature_index = atoi(str) - 1;
				index = 0;
			} else if (line[i] == ' ' || i == len) {
				str[index] = '\0';
				float feature_value = atof(str);
				train_datas[data_index][feature_index] = feature_value;
				index = 0;
			} else {
				str[index] = line[i];
				index++;
			}
		}
		data_index++;
	}
	fin.close();
	delete [] line;

	pthread_t ids[thread_num];
	Param params[thread_num];

	for (int i = 0; i < thread_num; i++) {
		params[i]._datas = train_datas;
		params[i]._labels = train_labels;
		params[i]._index = i;
		int ret = pthread_create(&ids[i], NULL, threadFn, &params[i]);
		if (ret) {
			cout << "Create pthread error!" << endl;
		}
	}

	for (int i = 0; i < thread_num; i++) {
		pthread_join(ids[i], NULL);
	}

	ofstream fout;
	fout.open("model.txt");
    for (int i = 0; i < thread_num; i++) {

    	stringstream ss;
    	string str;
    	ss << i;
    	ss >> str;
    	str = "./models/model" + str + ".txt";
    	const char* modelName = str.c_str();

    	ifstream fin;
    	fin.open(modelName);
    	char line[200];
    	int isFirstLine = 1;
    	while (fin.getline(line, 200)) {
    		if (isFirstLine) {
    			if (i == 0) {
    				char str[20];
    				int len = strlen(line);
    				int index = 0;
    				int paramIndex = 0;
    				for (int i = 0; i <= len; i++) {
    					if (line[i] == '\t' || i == len) {
							str[index] = '\0';
							int num = atoi(str);
    						if (paramIndex == 0) {
    							num *= thread_num;
    							fout << num << '\t';
    						} else if (paramIndex == 1) {
    							fout << num << '\t';
    						} else {
    							fout << num << '\n';
    						}
							index = 0;
							paramIndex++;
    					} else {
    						str[index] = line[i];
    						index++;
    					}
    				}
    			}
    			isFirstLine = 0;
    		} else {
	    		fout << line << endl;
    		}
    	}
    	fin.close();

    }
    fout.close();

	RandomForest* rf = new RandomForest("model.txt");

	for (int i = 0; i < 1866819; i++) {
		delete [] train_datas[i];
	}
	delete [] train_datas;
	delete [] train_labels;

	float** test_datas = new float*[282796];
	for (int i = 0; i < 282796; i++) {
		test_datas[i] = new float[201];
		for (int j = 0; j < 201; j++) {
			test_datas[i][j] = 0.0;
		}
	}

	line = new char[line_len];
	fin.open("test_data.txt");
	data_index = 0;
	while (fin.getline(line, line_len)) {
		char str[50];
		int len = strlen(line);
		int index = 0;
		int is_index = 1;
		int feature_index = -1;
		for (int i = 0 ; i <= len; i++) {
			if (line[i] == ' ' && is_index == 1) {
				is_index = 0;
				index  = 0;
			} else if (line[i] == ':') {
				str[index] = '\0';
				feature_index = atoi(str) - 1;
				index = 0;
			} else if (line[i] == ' ' || i == len) {
				str[index] = '\0';
				float feature_value = atof(str);
				test_datas[data_index][feature_index] = feature_value;
				index = 0;
			} else {
				str[index] = line[i];
				index++;
			}
		}
		data_index++;
	}
	fin.close();
	delete [] line;

	float* results = new float[282796];
	for (int i = 0; i < 282796; i++) {
		results[i] = 0.0;
	}
	rf->predict(test_datas, results, 282796);

	fout.open("submission.txt");
	fout << "id,label" << endl;
	for (int i = 0 ; i < 282796; i++) {
		fout << i << ',' << results[i] << endl;
	}
	fout.close();
	delete [] results;

	for (int i = 0; i < 282796; i++) {
		delete [] test_datas[i];
	}
	delete [] test_datas;
	return 0;

}