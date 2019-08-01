#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <iomanip>

using namespace std;
using namespace cv;


//将图像分割为2x2的块
void split2x2(Mat &image, string fileName1, string fileName2,string fileName3,string fileName4, int select = 0,bool conv1_1_flag = false) {
	ofstream fp1(fileName1), fp2(fileName2),fp3(fileName3),fp4(fileName4);
	int height = image.rows;
	int width = image.cols;
	//part1
	for (int i = 0; i < width / 2; ++i) {
		for (int j = 0; j < height / 2 - height / 4; ++j) {
			Mat fmap2x2 = image(Rect(i * 2, j * 2, 2, 2));
			vector<Mat> fmap2x2_vector;
			split(fmap2x2, fmap2x2_vector);
			for (size_t k = 0; k < fmap2x2_vector.size(); ++k) {
				if (select == 1) {
					for (int kk = 0; kk < 2; ++kk) {
						for (int mm = 0; mm < 2; ++mm) {
							if (fmap2x2_vector[k].at<int>(kk, mm) < 0) {
								if(conv1_1_flag==false) //不是第一层，数据用8bit表示
									fp1 << setfill('0') << setw(2) << hex << 256 + fmap2x2_vector[k].at<int>(kk, mm);
								else {//是第一层，数据用16bit表示
									fp1 << setfill('0') << setw(4) << hex << 65536 + fmap2x2_vector[k].at<int>(kk, mm) << endl;
									short temp = fmap2x2_vector[k].at<int>(kk, mm);
									char  a,b;
									a = temp>>8;
									b = temp&0xff;
									fp3.write(&b,1);
									fp3.write(&a,1);
									
								}
							}
							else {
								if (conv1_1_flag == false) //不是第一层，数据用8bit表示
									fp1 << setfill('0') << setw(2) << hex << fmap2x2_vector[k].at<int>(kk, mm);
								else {
									fp1 << setfill('0') << setw(4) << hex << fmap2x2_vector[k].at<int>(kk, mm) << endl;
									short temp = fmap2x2_vector[k].at<int>(kk, mm);
									char  a,b;
									a = temp>>8;
									b = temp&0xff;
									fp3.write(&b,1);
									fp3.write(&a,1);
									
								}
							}
						}
					}
					//fp1 << endl;
				}
				else {
					for (int kk = 0; kk < 2; ++kk) {
						for (int mm = 0; mm < 2; ++mm) {
							fp1 << fmap2x2_vector[k].at<int>(kk, mm) << endl;
						}
					}
				}
			}
		}
	}
	//part2
	for (int i = 0; i < width / 2; ++i) {
		for (int j = 0; j < height / 4; ++j) {
			Mat fmap2x2 = image(Rect(i * 2, j * 2 + (height / 2 - height / 4) * 2, 2, 2));
			vector<Mat> fmap2x2_vector;
			split(fmap2x2, fmap2x2_vector);
			for (size_t k = 0; k < fmap2x2_vector.size(); ++k) {
				if (select == 1) {
					for (int kk = 0; kk < 2; ++kk) {
						for (int mm = 0; mm < 2; ++mm) {
							if (fmap2x2_vector[k].at<int>(kk, mm) < 0) {
								if (conv1_1_flag == false) //不是第一层，数据用8bit表示
									fp2 << setfill('0') << setw(2) << hex << 256 + fmap2x2_vector[k].at<int>(kk, mm);
								else{ //是第一层，数据用16bit表示
									fp2 << setfill('0') << setw(4) << hex << 65536 + fmap2x2_vector[k].at<int>(kk, mm) << endl;
									short temp = fmap2x2_vector[k].at<int>(kk, mm);
									char  a,b;
									a = temp>>8;
									b = temp&0xff;
									fp4.write(&b,1);
									fp4.write(&a,1);							
								}
							}
							else {
								if (conv1_1_flag == false) //不是第一层，数据用8bit表示
									fp2 << setfill('0') << setw(2) << hex << fmap2x2_vector[k].at<int>(kk, mm);
								else{
									fp2 << setfill('0') << setw(4) << hex << fmap2x2_vector[k].at<int>(kk, mm) << endl;
									short temp = fmap2x2_vector[k].at<int>(kk, mm);
									char  a,b;
									a = temp>>8;
									b = temp&0xff;
									fp4.write(&b,1);
									fp4.write(&a,1);							
								}
							}
						}
					}
					//fp2 << endl;
				}
				else {
					for (int kk = 0; kk < 2; ++kk) {
						for (int mm = 0; mm < 2; ++mm) {
							fp2 << fmap2x2_vector[k].at<int>(kk, mm) << endl;
						}
					}
				}
			}
		}
	}

	fp1.close();
	fp2.close();
	fp3.close();
	fp4.close();
}

void writeFmapToFile(Mat &fmap, string fileName1, string fileName2,string fileName3,string fileName4,bool conv1_1_flag=false) {
	split2x2(fmap, fileName1, fileName2,fileName3,fileName4, 1,conv1_1_flag);
}

void read_fmap_from_file(string fileName, vector<Mat> &fmap_mat,int height,int width,int channel_in) {
	ifstream fmap_in(fileName);
	if (!fmap_in) {
		cout << "error " << endl;
		system("pause");
		return ;
	}
	//读入所有fmap
	vector< vector<int> > fmap;
	for (int i = 0; i < channel_in; ++i) {
		vector<int> one_fmap;
		for (int j = 0; j < height*width; ++j) {
			int temp;
			fmap_in >> temp;
			one_fmap.push_back(temp);
		}
		fmap.push_back(one_fmap);
	}

	//将所有vector类型的fmap转换为Mat类型
	for (vector< vector<int> >::iterator iter = fmap.begin(); iter != fmap.end(); ++iter) {
		vector<int> one_fmap;
		one_fmap = *iter;
		Mat a(one_fmap);
		Mat b = a.reshape(0, height);
		fmap_mat.push_back(b.clone());
	}
}


int main(){
	string fileName = "C:\\SSD\\20180424量化输出\\conv1_1_input.txt";

	vector<Mat> fmap_mat;
	int height = 514;  //扩展后
	int width = 770;
	int channel_in = 3;
	read_fmap_from_file(fileName,fmap_mat,height,width,channel_in);
	
	Mat fmap;
	merge(fmap_mat,fmap);
	writeFmapToFile(fmap,"part1.dat","part2.dat","fpag_part1.dat","fpga_part2.dat",true);

	//用于生成仿真的数据
	ifstream fp1("part1.dat"),fp2("part2.dat");
	ofstream fp3("pcie_fmap_sim_data.dat");
	for(int i=0;i<128*385*3;++i){
		string a[4],b[4];
		fp1 >> a[0] >> a[1] >> a[2] >> a[3];
		fp2 >> b[0] >> b[1] >> b[2] >> b[3];
		fp3 << b[3] << b[2] << b[1] << b[0] << a[3] << a[2] << a[1] << a[0] << endl;
	}
	string c = "0000000000000000";
	for(int i=0;i<385*3;++i){
		string a[4];
		fp1 >> a[0] >> a[1] >> a[2] >> a[3];
		fp3 << c << a[3] << a[2] << a[1] << a[0] << endl;
	}
	for(int i=0;i<1024*1024*3/16-129*385*3;++i)
		fp3 << c << c << endl;
	fp3.close();
	fp1.close();
	fp2.close();

	//用于生成通过pcie发送的文件
	ifstream fp4("fpag_part1.dat"),fp5("fpga_part2.dat");
	ofstream fp6("pcie_fmap_fpga_data.dat");
	for(int i=0;i<128*385*3;++i){
		char a[8],b[8];
		fp4.read(a,8);
		fp5.read(b,8);
		fp6.write(a,8);
		fp6.write(b,8);
	}
	char c1[8] = {0,0,0,0,0,0,0,0};
	for(int i=0;i<385*3;++i){
		char a[8];
		fp4.read(a,8);
		fp6.write(a,8);
		fp6.write(c1,8);
	}
	for(int i=0;i<1024*1024*3/16-129*385*3;++i){
		fp6.write(c1,8);
		fp6.write(c1,8);
	}
	fp4.close();
	fp5.close();
	fp6.close();
	system("pause");
	return 0;
}

