#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <iomanip>

using namespace std;


#define			DMA_SIZE  (1024*1024)
//将权值写入文件，供FPGA读取
void writeWeightToFile(vector< vector<int> >& weight, ofstream &fp, int channel_in, int channel_out, int PE_number, bool select_hex_write = false) {
	int base = 0;
	for (int n = 0; n < PE_number * 2; ++n) {
		for (int i = 0; i < channel_out / (PE_number * 2); ++i) {
			for (int k = 0; k < channel_in; ++k) {
				vector<int> a = weight[base + i * PE_number * 2 * channel_in + k];
				for (int m = 0; m < 9; ++m) {
					if (select_hex_write == false) {
						char temp = a[m];
						fp.write(&temp, 1);
					}
					else {
						if (a[m] < 0) {
							fp << setfill('0') << setw(2) << hex << 256 + a[m] << endl;
						}
						else {
							fp << setfill('0') << setw(2) << hex << a[m] << endl;
						}
					}
				}
			}
		}
		base += channel_in;
	}
}
// abo
void writeWeightToFile_proj(vector< vector<int> >& weight, ofstream &fp, int channel_in, int channel_out, string fileName) {
	ofstream fp_general(fileName);
	int  count = 1;
	for (int m = 0; m < 16; ++m) {
		for (int s = 0; s < channel_out / 16; ++s) {
			for (int k = 0; k < channel_in;++k) {
				vector<int> a = weight[k + m*channel_in + s*channel_in * 16];
				char temp = a[0];
				fp.write(&temp, 1);
				//abo
				if (a[0] < 0) {
					fp_general << setfill('0') << setw(2) << hex << 256 + a[0];
				}
				else {
					fp_general << setfill('0') << setw(2) << hex << a[0];
				}
				if (count % 8 == 0) {
					fp_general << endl;
					//abo
				}
				count = count + 1;
			}
		}
	}
}
void writeBiasToFile(vector<int> &bias, ofstream &fp, bool select_hex_write = false) {
	for (size_t i = 0; i < bias.size(); ++i) {
		if (select_hex_write == false) {
			char temp = bias[i];
			fp.write(&temp, 1);
		}
		else
		{
			if (bias[i] < 0) {
				fp << setfill('0') << setw(2) << hex << 256 + bias[i] << endl;
			}
			else {
				fp << setfill('0') << setw(2) << hex << bias[i] << endl;
			}
		}
	}
}
void read_bias_from_file(string fileName, vector<int>& bias_vector, int channel_out) {
	//读取bias
	ifstream bias_in(fileName);
	if (!bias_in) {
		cout << "bias error" << endl;
		system("pause");
		return;
	}
	for (int i = 0; i < channel_out; ++i) {
		int data;
		bias_in >> data;
		bias_vector.push_back(data);
	}
}
void read_weight_from_file(string fileName, vector< vector<int> >& weight, int channel_in, int channel_out) {
	ifstream weight_in(fileName);
	if (!weight_in) {
		cout << "weight error" << endl;
		system("pause");
		return;
	}
	//读入所有的weights
	for (int i = 0; i < channel_in*channel_out; ++i) {
		vector<int> one_weight;
		for (int j = 0; j < 9; ++j) {
			int temp;
			weight_in >> temp;
			one_weight.push_back(temp);
		}
		weight.push_back(one_weight);
	}
}
void read_weight_from_file_proj(string fileName, vector< vector<int> >& weight, int channel_in, int channel_out) {
	ifstream weight_in(fileName);
	if (!weight_in) {
		cout << "weight error" << endl;
		system("pause");
		return;
	}
	//读入所有的weights
	for (int i = 0; i < channel_in*channel_out; ++i) {
		vector<int> one_weight;
		int temp;
		weight_in >> temp;
		one_weight.push_back(temp); //一个数据的传入
		weight.push_back(one_weight);
	}
}

int main() {

	int channel_in, channel_out, PE_number;

	string fileName2, fileName3;
	string fileName;

	vector<int> bias_vector;
	vector< vector<int> > weight;

	ofstream fp_weight_bias("weight_bias.dat", ios::binary);
	ofstream fp_weight_bias_hex("weight_bias_hex.dat", ios::binary);

	////////////////////////////////
	/////////////////////////////////////第1层///////////////////////////////////////////////
	//weight
	//conv1_1
	channel_in = 3;
	channel_out = 64;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv1_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number,true);
	weight.clear();

	//conv1_2
	channel_in = 64;
	channel_out = 64;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv1_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number,true);
	weight.clear();

	//conv1_3
	channel_in = 64;
	channel_out = 64;
	PE_number = 2;
	fileName3 = "C:\\SSD\\test_515\\conv1_3_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	/////////////////////////////////////第2层///////////////////////////////////////////////
	//conv2_1_1
	channel_in = 64;
	channel_out = 64;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv2_1_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv2_1_2
	channel_in = 64;
	channel_out = 64;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv2_1_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv2_2_1
	channel_in = 64;
	channel_out = 64;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv2_2_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv2_2_2
	channel_in = 64;
	channel_out = 64;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv2_2_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();
	///////////////////////////////////////第3层///////////////////////////////////////////////
	//conv3_1_1
	channel_in = 64;
	channel_out = 128;
	PE_number = 2; //步长为2 的PE 为2
	fileName3 = "C:\\SSD\\test_515\\conv3_1_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv3_1_2
	channel_in = 128;
	channel_out = 128;
	PE_number = 4;//步长为4 的PE 为4
	fileName3 = "C:\\SSD\\test_515\\conv3_1_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv3_2_1
	channel_in = 128;
	channel_out = 128;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv3_2_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv3_2_2
	channel_in = 128;
	channel_out = 128;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv3_2_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	/////////////////////////////////////第4层///////////////////////////////////////////////
	//conv4_1_1
	channel_in = 128;
	channel_out = 256;
	PE_number = 2; //步长为2 的PE 为2
	fileName3 = "C:\\SSD\\test_515\\conv4_1_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv4_1_2
	channel_in = 256;
	channel_out = 256;
	PE_number = 4;//步长为4 的PE 为4
	fileName3 = "C:\\SSD\\test_515\\conv4_1_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv4_2_1
	channel_in = 256;
	channel_out = 256;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv4_2_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv4_2_2
	channel_in = 256;
	channel_out = 256;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv4_2_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	/////////////////////////////////////第5层///////////////////////////////////////////////
	//conv5_1_1
	channel_in = 256;
	channel_out = 512;
	PE_number = 2; //步长为2 的PE 为2
	fileName3 = "C:\\SSD\\test_515\\conv5_1_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv5_1_2
	channel_in = 512;
	channel_out = 512;
	PE_number = 4;//步长为4 的PE 为4
	fileName3 = "C:\\SSD\\test_515\\conv5_1_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv5_2_1
	channel_in = 512;
	channel_out = 512;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv5_2_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv5_2_2
	channel_in = 512;
	channel_out = 512;
	PE_number = 4;
	fileName3 = "C:\\SSD\\test_515\\conv5_2_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	/////////////////////////////////////第6层///////////////////////////////////////////////
	//conv6_1
	channel_in = 512;
	channel_out = 256;
	PE_number = 2; //步长为2 的PE 为2
	fileName3 = "C:\\SSD\\test_515\\conv6_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv6_2
	channel_in = 256;
	channel_out = 256;
	PE_number = 4;//步长为4 的PE 为4
	fileName3 = "C:\\SSD\\test_515\\conv6_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	/////////////////////////////////////第7层///////////////////////////////////////////////
	//conv7_1
	channel_in = 256;
	channel_out = 256;
	PE_number = 2; //步长为2 的PE 为2
	fileName3 = "C:\\SSD\\test_515\\conv7_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv7_2
	channel_in = 256;
	channel_out = 256;
	PE_number = 4;//步长为4 的PE 为4
	fileName3 = "C:\\SSD\\test_515\\conv7_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv8_1
	channel_in = 256;
	channel_out = 256;
	PE_number = 2;//步长为2 的PE 为2
	fileName3 = "C:\\SSD\\test_515\\conv8_1_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();

	//conv8_2
	channel_in = 256;
	channel_out = 256;
	PE_number = 4;//步长为4 的PE 为4
	fileName3 = "C:\\SSD\\test_515\\conv8_2_weights.txt";

	read_weight_from_file(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile(weight, fp_weight_bias, channel_in, channel_out, PE_number);
	writeWeightToFile(weight, fp_weight_bias_hex, channel_in, channel_out, PE_number, true);
	weight.clear();
	////////////////////////////////
	//bias
	//conv1_1
	/////////////////////////////////////第1层///////////////////////////////////////////////
	channel_out = 64;

	fileName2 = "C:\\SSD\\test_515\\conv1_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex,true);

	//ofstream fp_bias;
	//fp_bias.open("conv1_1_bias.dat");
	//writeBiasToFile(bias_vector, fp_bias,true);
	//fp_bias.close();

	bias_vector.clear();

	////conv1_2
	channel_out = 64;

	fileName2 = "C:\\SSD\\test_515\\conv1_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex,true);

	//fp_bias.open("conv1_2_bias.dat");
	//writeBiasToFile(bias_vector, fp_bias,true);
	//fp_bias.close();

	bias_vector.clear();

	//conv1_3
	channel_out = 64;

	fileName2 = "C:\\SSD\\test_515\\conv1_3_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex,true);

	//fp_bias.open("conv1_3_bias.dat");
	//writeBiasToFile(bias_vector, fp_bias,true);
	//fp_bias.close();

	bias_vector.clear();

	/////////////////////////////////////第2层///////////////////////////////////////////////
	//conv2_1_1
	channel_out = 64;

	fileName2 = "C:\\SSD\\test_515\\conv2_1_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex,true);

	//fp_bias.open("conv2_1_1_bias.dat");
	//writeBiasToFile(bias_vector, fp_bias,true);
	//fp_bias.close();

	bias_vector.clear();

	//conv2_1_2
	channel_out = 64;

	fileName2 = "C:\\SSD\\test_515\\conv2_1_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex,true);

	//fp_bias.open("conv2_1_2_bias.dat");
	//writeBiasToFile(bias_vector, fp_bias,true);
	//fp_bias.close();

	bias_vector.clear();

	//conv2_2_1
	channel_out = 64;

	fileName2 = "C:\\SSD\\test_515\\conv2_2_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex,true);

	//fp_bias.open("conv2_2_1_bias.dat");
	//writeBiasToFile(bias_vector, fp_bias,true);
	//fp_bias.close();

	bias_vector.clear();

	//conv2_2_2
	channel_out = 64;

	fileName2 = "C:\\SSD\\test_515\\conv2_2_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex,true);

	//fp_bias.open("conv2_2_2_bias.dat");
	//writeBiasToFile(bias_vector, fp_bias,true);
	//fp_bias.close();

	bias_vector.clear();

	///////////////////////////////////////第3层///////////////////////////////////////////////
	////conv3_1_1
	channel_out = 128;

	fileName2 = "C:\\SSD\\test_515\\conv3_1_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv3_1_2
	channel_out = 128;

	fileName2 = "C:\\SSD\\test_515\\conv3_1_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv3_2_1
	channel_out = 128;

	fileName2 = "C:\\SSD\\test_515\\conv3_2_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv3_2_2
	channel_out = 128;

	fileName2 = "C:\\SSD\\test_515\\conv3_2_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	/////////////////////////////////////第4层///////////////////////////////////////////////
	//conv4_1_1
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv4_1_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv4_1_2
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv4_1_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv4_2_1
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv4_2_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv4_2_2
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv4_2_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();



	/////////////////////////////////////第5层///////////////////////////////////////////////
	//conv5_1_1
	channel_out = 512;

	fileName2 = "C:\\SSD\\test_515\\conv5_1_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv5_1_2
	channel_out = 512;

	fileName2 = "C:\\SSD\\test_515\\conv5_1_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv5_2_1
	channel_out = 512;

	fileName2 = "C:\\SSD\\test_515\\conv5_2_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv5_2_2
	channel_out = 512;

	fileName2 = "C:\\SSD\\test_515\\conv5_2_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();


	/////////////////////////////////////第6层///////////////////////////////////////////////
	//conv6_1
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv6_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv6_2
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv6_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();


	/////////////////////////////////////第7层///////////////////////////////////////////////
	//conv7_1
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv7_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv7_2
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv7_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv8_1
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv8_1_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv8_2
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv8_2_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	/////////proj_weights 
	//conv2_proj weights
	channel_in = 64;
	channel_out = 128;
	fileName3 = "C:\\SSD\\test_515\\conv2_proj_weights.txt";
	fileName = "C:\\SSD\\test_515\\conv2_proj_weights_general.txt";

	read_weight_from_file_proj(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile_proj(weight, fp_weight_bias, channel_in, channel_out, fileName);
	weight.clear();


	channel_in = 128;
	channel_out = 256;
	fileName3 = "C:\\SSD\\test_515\\conv3_proj_weights.txt";
	fileName = "C:\\SSD\\test_515\\conv3_proj_weights_general.txt";

	read_weight_from_file_proj(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile_proj(weight, fp_weight_bias, channel_in, channel_out, fileName);
	weight.clear();


	channel_in = 256;
	channel_out = 512;
	fileName3 = "C:\\SSD\\test_515\\conv4_proj_weights.txt";
	fileName = "C:\\SSD\\test_515\\conv4_proj_weights_general.txt";

	read_weight_from_file_proj(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile_proj(weight, fp_weight_bias, channel_in, channel_out, fileName);
	weight.clear();

	channel_in = 512;
	channel_out = 256;
	fileName3 = "C:\\SSD\\test_515\\conv6_proj_weights.txt";
	fileName = "C:\\SSD\\test_515\\conv6_proj_weights_general.txt";

	read_weight_from_file_proj(fileName3, weight, channel_in, channel_out);//从文件读权值
	writeWeightToFile_proj(weight, fp_weight_bias, channel_in, channel_out, fileName);
	weight.clear();




	bias_vector.clear();  //清空之前的
	//conv2_proj bias
	channel_out = 128;

	fileName2 = "C:\\SSD\\test_515\\conv2_proj_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

	//conv3_proj bias
	channel_out = 256;

	fileName2 = "C:\\SSD\\test_515\\conv3_proj_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();


	//conv4_proj bias
	channel_out = 512;

	fileName2 = "C:\\SSD\\test_515\\conv4_proj_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	//writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();


	//conv6_proj bias
	int channel_out_6_proj;
	channel_out_6_proj = 256;

	fileName2 = "C:\\SSD\\test_515\\conv6_proj_bias.txt";
	read_bias_from_file(fileName2, bias_vector, channel_out_6_proj); //从文件读bias
	writeBiasToFile(bias_vector, fp_weight_bias);
	writeBiasToFile(bias_vector, fp_weight_bias_hex, true);
	bias_vector.clear();

///////////////////////////////////////////
///////////////////////////////////////////
	
	//不够整数DMA就补0
	long valid_length = fp_weight_bias.tellp();
	long fill_lenght = DMA_SIZE - valid_length%DMA_SIZE;
	char* fill_data = new char[fill_lenght];
	memset(fill_data,0,fill_lenght);
	fp_weight_bias.write(fill_data,fill_lenght);

	fp_weight_bias.close();
	for(int i=0;i<fill_lenght;++i)
		fp_weight_bias_hex << "00" << endl;
	fp_weight_bias_hex.close();

	/////////////////////////////////
	////////////////////////////////
	ifstream fp0("weight_bias.dat");
	ifstream fp1("weight_bias_hex.dat");
	int		 lenght;
	fp0.seekg(0,SEEK_END);
	lenght = fp0.tellg();
	fp0.seekg(0,SEEK_SET);
	fp0.close();

	ofstream fp2("pcie_wegiht_bias_sim_data.dat");
	cout << lenght << endl;
	for(int i=0;i<lenght;i+=16){
		string buf[16];
		for(int j=0;j<16;++j)
			fp1 >> buf[j];
		for(int j=15;j>=0;--j)
			fp2 << buf[j];
		fp2 << endl;
	}
	system("pause");
	return 0;
}