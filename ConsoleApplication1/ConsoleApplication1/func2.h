#ifndef FUNC2_H_
#define FUNC2_H_

#include<opencv2\opencv.hpp>
#include<iostream>
#include<algorithm>
#include<ctime>
#include <vector>
#include<sstream>
#include<fstream>

using namespace cv;
using namespace std;

void RemoveSmallRegion(Mat& src, Mat& dst, int Arealimit, int CheckMode, int NeighborMode) {
	//checkmode��0����ȥ��������1����ȥ��������neighbormode: 0����4����1����8����
	int RemoveCount = 0;	//ȥ���ĸ���
							//��¼ÿ�����ص����״̬�ı�ǩ��0����δ��飬1�������ڼ��,2�����鲻�ϸ���Ҫ��ת��ɫ����3������ϸ������
	Mat Pointlabel = Mat::zeros(src.size(), CV_8UC1);
	CheckMode = 255 * (1 - CheckMode);


	for (int i = 0;i < src.rows;++i) {
		//��ǲ���Ҫȥ������ɫ����
		uchar* iData = src.ptr<uchar>(i);
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0;j < src.cols;++j) {
			if (iData[j] == CheckMode) {
				iLabel[j] = 3;
			}
		}
	}

	vector<Point2i> NeihborPos;		//��¼�����λ��
									//(0,0)��4����
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeighborMode == 1) {
		//8����
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, -1));
		NeihborPos.push_back(Point2i(1, 1));
	}

	int NeighborCount = 4 + 4 * NeighborMode;
	int CurX = 0, CurY = 0;

	for (int i = 0; i < src.rows;++i) {
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0;j < src.cols;++j) {
			if (iLabel[j] == 0) {
				//δ���
				vector<Point2i> GrowBuffer;	//ջ�����ڴ洢������
				GrowBuffer.push_back(Point2i(i, j));
				Pointlabel.at<uchar>(i, j) = 1;
				int CheckResult = 1;	//�����жϽ�����Ƿ񳬳���С����1Ϊδ������2Ϊ����

				for (int z = 0;z < GrowBuffer.size();++z) {
					//��ջ�еĵ���������飬�������ĵ����ջ
					for (int q = 0;q < NeighborCount;++q) {
						CurX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurX >= 0 && CurX < src.rows&&CurY >= 0 && CurY < src.cols) {
							if (Pointlabel.at<uchar>(CurX, CurY) == 0) {
								GrowBuffer.push_back(Point2i(CurX, CurY));
								Pointlabel.at<uchar>(CurX, CurY) = 1;
							}
						}
					}
				}
				if (GrowBuffer.size() > Arealimit) {
					CheckResult = 2;
				}
				else
				{
					CheckResult = 1;
					++RemoveCount;
				}
				for (int z = 0;z < GrowBuffer.size();++z) {
					CurX = GrowBuffer.at(z).x;
					CurY = GrowBuffer.at(z).y;
					Pointlabel.at<uchar>(CurX, CurY) += CheckResult;
				}

			}
		}
	}


	for (int i = 0;i < src.rows;++i) {
		uchar* iData = src.ptr<uchar>(i);
		uchar* iDstData = dst.ptr<uchar>(i);
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0;j < src.cols;++j) {
			if (iLabel[j] == 2) {
				iDstData[j] = CheckMode;
			}
			else if (iLabel[j] == 3) {
				iDstData[j] = iData[j];
			}
		}
	}
	cout << "Removed:" << RemoveCount << endl;
}


void RemoveSmallRegion2(Mat& src, Mat& dst, int AreaLimit, int CheckMode) {
	//CheckMode:0����ȥ��������1����ȥ��������
	dst = src.clone();
	vector<vector<Point> >contours(100);
	findContours(dst.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (int k = 0;k < contours.size();++k) {
		if ((int)contourArea(contours[k], false) < AreaLimit) {
			drawContours(dst, contours, k, Scalar((1 - CheckMode) * 255), FILLED);
		}
		//cout << k << ":" << contourArea(contours[k], false)<<endl;
	}
}


#endif