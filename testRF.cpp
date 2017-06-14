#include <cstring>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include "RandomForest.cpp"

using namespace std;

#define line_len 2000

int main(int argc, char* argv[]) {

	// int train_datas_num = atoi(argv[1]);
	// int train_feature_num = atoi(argv[2]);
	// int tree_depth = atoi(argv[3]);
	// int min_sample_num = atoi(argv[4]);
	// float min_info_gain = atof(argv[5]);
	// int test_datas_num = atoi(argv[6]);

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
				if (feature_index == -1) {
					cout << "wrong!" << endl;
				}
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

	RandomForest* rf = new RandomForest(train_datas, train_labels, 4000, 2, 1866819, 201);
	rf->generateForest(6, 100, 0.0005, -1, -1);

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

	ofstream fout;
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