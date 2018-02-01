//����˼·�ǣ��ٶ�����Ƶ�ĵ�һ֡������껭�����������򣬶Խ�������ÿһ֡���͵��������������ΪϺֻ�����������
/*  �ڸ�������������������������5�� 8�У�Ҳ����5*8һ��40�����ӣ�ÿֻ���ӷ�һֻϺ������40ֻϺ
*   �۶�ÿһ֡��ROIͼ����Ϊƽ����40�����ӣ�Ȼ��������ȡ��40��Сͼ�񣬽��ж�ֵ�����ͽ�SVM�����б���ֱ�����ƻ����������
*   �ܵ�ĳһֻϺ����һ��ʱ�䣬һֻ�����������ʱ�����ж���ֻϺ���ˣ�������ǻ�ġ�
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
//һЩȫ�ֱ���
Mat currentFrame;
const string WIN1 = "Input";
const string WIN2 = "Fore";
int box_cols = 4, box_rows = 3;   //���ӵ�����������
vector<int> status(100,0);  //��¼ÿֻϺ�Ѿ��������ж�Ϊ����״̬��֡��
int min_threshold = 1;       //����died_min����������������״̬��Ϻ�����ж�Ϊ����
int dead_counts;           //�ж�Ϊ������Ҫ������֡��
						   //Ϻ�����ж���
XiaDetect detect;

//���ص�����
bool flag = false;
Rect initRect;
bool useMouse = true;
void onMouse(int event, int x, int y, int flags, void* param);   //���ص���������������

																 //��һ����ƵԴ�ĵ�һ֡������껭�����ο򣬱�ʾϺ�������
void init(VideoCapture& capture) {
	capture >> currentFrame;
	if (currentFrame.cols>1000 && currentFrame.rows>900)
		resize(currentFrame, currentFrame, Size(), 0.65, 0.65);
	namedWindow(WIN1);
	setMouseCallback(WIN1, onMouse);
	imshow(WIN1, currentFrame);
	waitKey();
}

//��ÿһ֡��������һ֡��ÿһ��������Ϻ����Ӿ���
vector<Rect> getRects(const Mat& _img, int box_rows, int box_cols);
vector<Rect> getRects(const Mat& _img);	//ֱ�Ӽ���������Ӿ���

int main() {
	string videoName = "E:\\lab\\program\\xia\\xia\\15.mp4";
	VideoCapture capture(videoName);   //����Ƶ
	if (!capture.isOpened()) {
		cerr << "Load Video Failed!\n";
		return -1;
	}
	double fps = capture.get(CV_CAP_PROP_FPS);
	dead_counts =(int)( min_threshold * 60 * fps);    //��������dead_counts֡�����Ϊ����״̬�����ж�Ϊ����
	capture.set(CV_CAP_PROP_POS_FRAMES, 6700);
	init(capture);

	namedWindow(WIN2);

	bool pause = false;    //�Ƿ���ͣ
	unsigned int counts = 0;       //��ǰ֡��
	double time = 0;
	while (true) {     //һֱѭ��
		if (!pause) {     //�������ͣ
						  //long long t0 = getTickCount();
			capture >> currentFrame;         //����һ֡
			if (!currentFrame.data)
				break;
			++counts;
			if (currentFrame.cols > 1000 && currentFrame.rows > 900)
				resize(currentFrame, currentFrame, Size(), 0.65, 0.65);   //�ߴ���С��ԭ��һ��
			currentFrame = currentFrame(initRect).clone();              //ȡROI


			//vector<Rect> rects = getRects(currentFrame, box_rows, box_cols);    //��ֻϺ����Ӿ��ο�
			vector<Rect> rects = getRects(currentFrame);
			for (size_t i = 0; i < rects.size(); ++i) {
				if (rects[i].area() == 0)
					continue;
				//cout << currentFrame(rects[i]).channels() << endl;
				int result = detect.judge(currentFrame(rects[i]).clone());      //��SVM�ж���Ϻ�����ƣ����Ļ��ǻ�ģ�
				if (result == 1) {    //���ŵ�
					status[i] = 0;
					putText(currentFrame, "alive", rects[i].tl(), 1, 1, Scalar(0, 255, 0));
				}
				else {          //�ж�Ϊ����
					status[i]++;
					if (status[i] > dead_counts)
						putText(currentFrame, "died", rects[i].tl(), 1, 1, Scalar(255, 255, 255));
					else
						putText(currentFrame, "alive", rects[i].tl(), 1, 1, Scalar(0, 255, 0));
				}
				rectangle(currentFrame, rects[i], Scalar(0, 0, 255), 1);   //����Ϻ����Ӿ��ο�

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
			/*cout << "������ӵ�������";
			cin >> box_cols;
			cout << "������ӵ�������";
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

//����ͼ����������õĲ��֣��Ҿ���Ԥ������
vector<Rect> getRects(const Mat& _img, int box_rows, int box_cols) {
	//ȷ��Ϊ�Ҷ�ͼ
	Mat img = _img.clone();
	if (_img.channels() == 3)
		cvtColor(_img, img, CV_BGR2GRAY);
	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	vector<Rect> result(box_rows*box_cols);
	int kk = 0;
	int width = _img.cols / box_cols;   //ÿֻϺ�ķ���Ŀ���Լ��߶�
	int height = _img.rows / box_rows;
	for (int i = 0; i < box_rows; ++i) {
		for (int j = 0; j < box_cols; ++j) {
			Rect temp(j*width, i*height, width, height);
			Mat roi = img(temp); //��ǰ�Ŀ�
			threshold(roi, roi, 0, 255, THRESH_BINARY + THRESH_OTSU);
			morphologyEx(roi, roi, MORPH_OPEN, ele);  //
			vector<vector<Point> >contours;         //��������
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
	//ȷ��Ϊ�Ҷ�ͼ
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
Point origin;//���ڱ������ѡ���һ�ε���ʱ���λ��    
Rect selection;//���ڱ������ѡ��ľ��ο�    
int trackObject = 0;

/*-------------------------------

�������� ��ʾ�켣

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

������

--------------------------------*/
void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)//ֻ�е�����������ȥʱ����Ч��Ȼ��ͨ��if�������Ϳ���ȷ����ѡ��ľ�������selection��    
	{
		selection.x = MIN(x, origin.x);//�������ϽǶ�������    
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);//���ο�    
		selection.height = std::abs(y - origin.y);//���θ�    

		selection &= Rect(0, 0, src.cols, src.rows);//����ȷ����ѡ�ľ���������ͼƬ��Χ��  
													//rectangle(src,selection,Scalar(0,0,255),2);  

	}
	switch (event)
	{
	case CV_EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);//���հ���ȥʱ��ʼ����һ����������    
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
	vector<vector<Point>> trajectorys;//�켣��¼  
	int objectCounter = 0;
	
	string videoName = "E:\\lab\\program\\xia\\xia\\15.mp4";
	VideoCapture cap(videoName);   //����Ƶ
	double fps = cap.get(CV_CAP_PROP_FPS);
	cap.set(CV_CAP_PROP_POS_FRAMES, 6700);
	//VideoCapture cap(0);//��Ĭ�ϵ�����ͷ    
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
			trajectorys[objectCounter].push_back(cen);//��[0]��ʼ��ʼ���켣��ʼ��  

			objectCounter++;//������Ŀ�������1  
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
		if (waitKey(30) == 27) //Esc���˳�    
		{
			stop = true;
		}
		//cap >> src;
	}
	return 0;
}