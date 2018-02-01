//总体思路是：①对于视频的第一帧，用鼠标画出隔栏的区域，对接下来的每一帧，就单纯处理该区域（因为虾只在这个区域活动）
/*  ②给定隔栏的行数和列数，比如5行 8列，也就是5*8一共40个格子，每只格子放一只虾，就有40只虾
*   ③对每一帧的ROI图，分为平均的40个格子，然后再依次取这40个小图像，进行二值化后送进SVM进行判别，是直的姿势还是弯的姿势
*   ④当某一只虾超过一定时间，一只保持弯的姿势时，就判定这只虾死了，否则就是活的。
*/
#include <opencv2\opencv.hpp>
#include <opencv2\xfeatures2d.hpp>
#include <opencv2\core\utility.hpp>
#include <opencv2\tracker.hpp>
#include <opencv2\highgui.hpp>
#include <iostream>
#include <iomanip>
#include<vector>
#include<algorithm>
#include"func.h"
#include"func2.h"
using namespace cv;
using namespace std;
/*
//一些全局变量
Mat currentFrame;
const string WIN1 = "Input";
const string WIN2 = "Fore";
int box_cols = 4, box_rows = 3;   //格子的行数和列数
vector<int> status(100,0);  //记录每只虾已经连续被判定为死亡状态的帧数
int min_threshold = 1;       //超过died_min分钟连续保持死亡状态的虾，则判定为死亡
int dead_counts;           //判定为死亡需要经过的帧数
						   //虾死亡判定类
XiaDetect detect;

//鼠标回调函数
bool flag = false;
Rect initRect;
bool useMouse = true;
void onMouse(int event, int x, int y, int flags, void* param);   //鼠标回调函数，用来画框

																 //对一个视频源的第一帧，用鼠标画出矩形框，表示虾活动的区域
void init(VideoCapture& capture) {
	capture >> currentFrame;
	if (currentFrame.cols>1000 && currentFrame.rows>900)
		resize(currentFrame, currentFrame, Size(), 0.65, 0.65);
	namedWindow(WIN1);
	setMouseCallback(WIN1, onMouse);
	imshow(WIN1, currentFrame);
	waitKey();
}

//对每一帧，计算这一帧中每一个格子中虾的外接矩形
vector<Rect> getRects(const Mat& _img, int box_rows, int box_cols);
vector<Rect> getRects(const Mat& _img);	//直接计算所有外接矩形

int main() {
	string videoName = "E:\\lab\\program\\xia\\xia\\15.mp4";
	VideoCapture capture(videoName);   //读视频
	if (!capture.isOpened()) {
		cerr << "Load Video Failed!\n";
		return -1;
	}
	double fps = capture.get(CV_CAP_PROP_FPS);
	dead_counts =(int)( min_threshold * 60 * fps);    //连续超过dead_counts帧如果都为死亡状态，则判定为死亡
	capture.set(CV_CAP_PROP_POS_FRAMES, 6700);
	init(capture);

	namedWindow(WIN2);

	bool pause = false;    //是否暂停
	unsigned int counts = 0;       //当前帧数
	double time = 0;
	while (true) {     //一直循环
		if (!pause) {     //如果不暂停
						  //long long t0 = getTickCount();
			capture >> currentFrame;         //读入一帧
			if (!currentFrame.data)
				break;
			++counts;
			if (currentFrame.cols > 1000 && currentFrame.rows > 900)
				resize(currentFrame, currentFrame, Size(), 0.65, 0.65);   //尺寸缩小成原来一半
			currentFrame = currentFrame(initRect).clone();              //取ROI


			//vector<Rect> rects = getRects(currentFrame, box_rows, box_cols);    //各只虾的外接矩形框
			vector<Rect> rects = getRects(currentFrame);
			for (size_t i = 0; i < rects.size(); ++i) {
				if (rects[i].area() == 0)
					continue;
				//cout << currentFrame(rects[i]).channels() << endl;
				int result = detect.judge(currentFrame(rects[i]).clone());      //用SVM判定该虾的姿势（死的还是活的）
				if (result == 1) {    //活着的
					status[i] = 0;
					putText(currentFrame, "alive", rects[i].tl(), 1, 1, Scalar(0, 255, 0));
				}
				else {          //判定为死亡
					status[i]++;
					if (status[i] > dead_counts)
						putText(currentFrame, "died", rects[i].tl(), 1, 1, Scalar(255, 255, 255));
					else
						putText(currentFrame, "alive", rects[i].tl(), 1, 1, Scalar(0, 255, 0));
				}
				rectangle(currentFrame, rects[i], Scalar(0, 0, 255), 1);   //画出虾的外接矩形框

			}
			imshow(WIN1, currentFrame);
		}
		char ch = waitKey(1);
		if (ch == ' ')
			pause = !pause;
		else if (ch == 'q')
			break;
	}
	cout << "Video Over!\n";
	return 0;
}
void onMouse(int event, int x, int y, int flags, void* param) {
	if (useMouse) {
		switch (event) {
		case EVENT_LBUTTONDOWN:
			flag = true;
			initRect = Rect(x, y, 0, 0);
			break;
		case EVENT_MOUSEMOVE:
			if (flag) {
				initRect.width = x - initRect.x;
				initRect.height = y - initRect.y;
			}
			break;
		case EVENT_LBUTTONUP:
			flag = false;
			if (initRect.width < 0) {
				initRect.x += initRect.width;
				initRect.width *= -1;
			}
			if (initRect.height < 0) {
				initRect.y += initRect.height;
				initRect.height *= -1;
			}
			rectangle(currentFrame, initRect, Scalar(0, 0, 255), 1);
			imshow(WIN1, currentFrame);
			useMouse = false;*/
			/*cout << "输入格子的列数：";
			cin >> box_cols;
			cout << "输入格子的行数：";
			cin >> box_rows;*/
			//status = vector<int >(box_cols*box_rows, 0);
	/*		break;
		default:
			break;
		}
	}
}
bool cmp(const Rect& _lhs, const Rect& _rhs) {
	return _lhs.area() > _rhs.area();
}

//输入图像是用鼠标框好的部分，且经过预处理了
vector<Rect> getRects(const Mat& _img, int box_rows, int box_cols) {
	//确保为灰度图
	Mat img = _img.clone();
	if (_img.channels() == 3)
		cvtColor(_img, img, CV_BGR2GRAY);
	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	vector<Rect> result(box_rows*box_cols);
	int kk = 0;
	int width = _img.cols / box_cols;   //每只虾的方框的宽度以及高度
	int height = _img.rows / box_rows;
	for (int i = 0; i < box_rows; ++i) {
		for (int j = 0; j < box_cols; ++j) {
			Rect temp(j*width, i*height, width, height);
			Mat roi = img(temp); //当前的框
			threshold(roi, roi, 0, 255, THRESH_BINARY + THRESH_OTSU);
			morphologyEx(roi, roi, MORPH_OPEN, ele);  //
			vector<vector<Point> >contours;         //查找轮廓
			findContours(roi.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			vector<Rect> goodRects;
			for (size_t k = 0; k < contours.size(); ++k) {
				Rect rect = boundingRect(contours[k]);
				if (valid(rect, width, height)) {
					rect.x += temp.x;
					rect.y += temp.y;
					goodRects.push_back(rect);
				}
			}
			if (goodRects.size() !=0) {
				sort(goodRects.begin(), goodRects.end(), cmp);
				result[kk] = goodRects[0];
			}
			kk++;
		}
	}
	imshow(WIN2, img);
	return result;
}

vector<Rect> getRects(const Mat& _img) {
	//确保为灰度图
	Mat img = _img.clone();
	if (_img.channels() == 3)
		cvtColor(_img, img, CV_BGR2GRAY);
	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	vector<Rect> result;
	//int kk = 0;
	threshold(img, img, 0, 255, THRESH_BINARY + THRESH_OTSU);
	RemoveSmallRegion2(img, img, 100, 1);
	vector<vector<Point> >contours;
	findContours(img, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (int i = 0;i < contours.size();++i) {
		Rect rect = boundingRect(contours[i]);
		result.push_back(rect);
	}
	imshow(WIN2, img);
	return result;
}
*/


Mat src;
vector<Rect>centers;
bool selectObject = false;
Point origin;//用于保存鼠标选择第一次单击时点的位置    
Rect selection;//用于保存鼠标选择的矩形框    
int trackObject = 0;

/*-------------------------------

函数功能 显示轨迹

--------------------------------*/
void displayTrajectory(Mat img, vector<Point> traj, Scalar s)
{
	if (!traj.empty())
	{
		for (size_t i = 0; i < traj.size() - 1; i++)
		{
			line(img, traj[i], traj[i + 1], s, 2, 8, 0);
		}
	}
}
/*-------------------------------

鼠标控制

--------------------------------*/
void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)//只有当鼠标左键按下去时才有效，然后通过if里面代码就可以确定所选择的矩形区域selection了    
	{
		selection.x = MIN(x, origin.x);//矩形左上角顶点坐标    
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);//矩形宽    
		selection.height = std::abs(y - origin.y);//矩形高    

		selection &= Rect(0, 0, src.cols, src.rows);//用于确保所选的矩形区域在图片范围内  
													//rectangle(src,selection,Scalar(0,0,255),2);  

	}
	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);//鼠标刚按下去时初始化了一个矩形区域    
		selectObject = true;
		break;
	case CV_EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0){
			trackObject = -1;
		}
		break;
	}
}

int main(int argc, char *argv[])
{
	namedWindow("tracker");
	setMouseCallback("tracker", onMouse, 0);

	vector<Ptr<Tracker>>trackers_i;
	MultiTracker myTracker;
	vector<vector<Point>> trajectorys;//轨迹记录  
	int objectCounter = 0;
	
	string videoName = "E:\\lab\\program\\xia\\xia\\15.mp4";
	VideoCapture cap(videoName);   //读视频
	double fps = cap.get(CV_CAP_PROP_FPS);
	cap.set(CV_CAP_PROP_POS_FRAMES, 6700);
	//VideoCapture cap(0);//打开默认的摄像头    
	//cap >> src;
	
	bool stop = false;
	while (!stop)
	{
		cap >> src;

		if (selectObject)
		{
			rectangle(src, selection, Scalar(0, 0, 255), 2, 8, 0);
		}
		if (trackObject < 0)
		{
			trackers_i.push_back(TrackerKCF::create());
			myTracker.add(trackers_i[objectCounter],src, selection);
			cout << selection.x << "," << selection.y << endl;
			Point cen = Point(selection.x + selection.width / 2.0, selection.y + selection.height / 2.0);
			trajectorys.resize(objectCounter + 1);
			trajectorys[objectCounter].push_back(cen);//从[0]开始初始化轨迹起始点  

			objectCounter++;//待跟踪目标个数加1  
			trackObject = 1;

		}
		if (trackObject) 
		{
			vector<Rect2d> r;
			myTracker.update(src,r);

			//size_t s = r.size();
			RNG rng;
			
			for (size_t i = 0; i < r.size(); i++)
			{
				Scalar scalar = Scalar(rng.uniform(0, 256), rng.uniform(0, 256), rng.uniform(0, 256));
				rectangle(src, r[i], scalar, 2, 8, 0);
				Point C = Point(r[i].x + r[i].width / 2.0, r[i].y + r[i].height / 2.0);
				trajectorys[i].push_back(C);
				displayTrajectory(src, trajectorys[i], scalar);
				char name[10];
				sprintf_s(name, "%zd", (i + 1));
				putText(src, name, r[i].tl(), 3, 0.8, Scalar(0, 255, 255), 2, 8, false);
				myTracker.update(src, r);
			}

		}
		
		imshow("tracker", src);
		if (waitKey(30) == 27) //Esc键退出    
		{
			stop = true;
		}
		//cap >> src;
	}
	return 0;
}