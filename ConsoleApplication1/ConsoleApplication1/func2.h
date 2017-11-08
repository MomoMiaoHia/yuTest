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
	//checkmode：0代表去除黑区域，1代表去除白区域；neighbormode: 0代表4领域，1代表8领域；
	int RemoveCount = 0;	//去除的个数
							//记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查
	Mat Pointlabel = Mat::zeros(src.size(), CV_8UC1);
	CheckMode = 255 * (1 - CheckMode);


	for (int i = 0;i < src.rows;++i) {
		//标记不需要去除的颜色区域
		uchar* iData = src.ptr<uchar>(i);
		uchar* iLabel = Pointlabel.ptr<uchar>(i);
		for (int j = 0;j < src.cols;++j) {
			if (iData[j] == CheckMode) {
				iLabel[j] = 3;
			}
		}
	}

	vector<Point2i> NeihborPos;		//记录领域点位置
									//(0,0)的4邻域
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeighborMode == 1) {
		//8邻域
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
				//未检测
				vector<Point2i> GrowBuffer;	//栈，用于存储生长点
				GrowBuffer.push_back(Point2i(i, j));
				Pointlabel.at<uchar>(i, j) = 1;
				int CheckResult = 1;	//用于判断结果（是否超出大小），1为未超出，2为超出

				for (int z = 0;z < GrowBuffer.size();++z) {
					//对栈中的点进行邻域检查，生长出的点加入栈
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
	//CheckMode:0代表去除黑区域，1代表去除白区域；
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