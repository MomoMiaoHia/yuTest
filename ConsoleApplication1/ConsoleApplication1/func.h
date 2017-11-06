#ifndef FUNC_H_
#define FUNC_H_

#include<opencv2\opencv.hpp>
#include<iostream>
#include<algorithm>
#include<ctime>
#include<sstream>
#include<fstream>
#include"svm.h"
using namespace cv;
using namespace std;

class XiaDetect {
private:
	svm_model* m_model;
	const int DIMS;

public:
	XiaDetect() :DIMS(1764) {
		m_model = svm_load_model("xia-detect-2.model");
	}
	~XiaDetect() {
		svm_free_and_destroy_model(&m_model);
	}
	int judge(const Mat& _img) {     //��ϺͼƬ�����ж���0�������ģ�1������
		Mat src = _img.clone();
		if (src.channels() == 3)
			cvtColor(src, src, CV_BGR2GRAY);
		threshold(src, src, 0, 255, THRESH_BINARY + THRESH_OTSU);  //������Ƕ�ֵͼ
		resize(src, src, Size(64, 64));
		HOGDescriptor hog(Size(64, 64), Size(16, 16), Size(8, 8), Size(8, 8), 9);
		vector<float> descriptors;   //����������
		hog.compute(src, descriptors);  //DIMSά
		svm_node *testNode = new svm_node[DIMS + 1];
		for (int j = 0; j < DIMS; j++)
		{
			svm_node node;
			node.index = j + 1;
			node.value = descriptors[j];
			testNode[j] = node;    //���õ�k��
		}
		svm_node node;
		node.index = -1;
		testNode[DIMS] = node;
		double result = svm_predict(m_model, testNode);
		delete[] testNode;
		return static_cast<int>(result);
	}
};

string num2str(int i)
{
	stringstream ss;
	ss << i;
	return ss.str();
}
inline double dis(const Point &a, const Point &b)    //��������֮�����
{
	double value = sqrt((double)(a.x - b.x)*(a.x - b.x) + (double)(a.y - b.y)*(a.y - b.y));
	return value;
}
bool valid(const Rect& objRect, int width, int heiht)        //��ת�����Ƿ����㳤��Ҫ��
{
	//return true;
	if (objRect.width > width ||objRect.height > heiht || (objRect.width == width && objRect.height == heiht))
		return false;
	return true;
}

double getMean(const vector<double>& vec)            //���ֵ
{
	size_t size = vec.size();
	double sum = 0.0;
	for (int i = 0;i<size;i++)
		sum += vec[i];
	return sum / size;
}
string getTime(double seconds)   //��ȡ��ǰ��Ƶʱ��
{
	int hours = 0, mins = 0, sec = 0;;  //ʱ���֣���
	hours = static_cast<int>(seconds / 3600);  //���Сʱ��
	seconds -= hours * 3600;   //ʣ�������
	mins = static_cast<int>(seconds / 60);       //���������
	seconds -= mins * 60;;     //ʣ�������
	sec = static_cast<int>(seconds);

	string time;
	if (hours != 0)
	{
		time += num2str(hours);
		time += "ʱ";
	}
	if (mins != 0)
	{
		time += num2str(mins);
		time += "��";
	}
	time += num2str(sec);
	time += "��";
	return time;
}
#endif