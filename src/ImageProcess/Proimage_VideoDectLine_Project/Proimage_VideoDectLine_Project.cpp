// Proimage_VideoDectLine_Project.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#define PI 3.1415926

class LineFinder{  
private:  
	// 直线对应的点参数向量   
	std::vector<cv::Vec4i> lines;  
	//步长   
	double deltaRho;  
	double deltaTheta;  
	// 判断是直线的最小投票数   
	int minVote;  
	// 判断是直线的最小长度   
	double minLength;  
	// 同一条直线上点之间的距离容忍度   
	double maxGap;  
public:  
	//初始化   
	LineFinder() : deltaRho(1), deltaTheta(PI/180),  
		minVote(10), minLength(0.), maxGap(0.) {}  
	// 设置步长   
	void setAccResolution(double dRho, double dTheta) {  
		deltaRho= dRho;  
		deltaTheta= dTheta;  
	}  
	// 设置最小投票数   
	void setMinVote(int minv) {  
		minVote= minv;  
	}  
	// 设置最小线段长度和线段间距容忍度   
	void setLineLengthAndGap(double length, double gap) {  
		minLength= length;  
		maxGap= gap;  
	}  

	//寻找线段   
	std::vector<cv::Vec4i> findLines(cv::Mat& binary) {  
		lines.clear();  
		cv::HoughLinesP(binary,lines, deltaRho, deltaTheta, minVote,minLength, maxGap);  
		return lines;   
	}  

	// 画线段   
	void drawDetectedLines(cv::Mat &image, cv::Scalar color=cv::Scalar(0,255,0)) {  
		std::vector<cv::Vec4i>::const_iterator it2=lines.begin();  
		while (it2!=lines.end()) {  
			cv::Point pt1((*it2)[0],(*it2)[1]);  
			cv::Point pt2((*it2)[2],(*it2)[3]);  
			cv::line( image, pt1, pt2, color);  
			++it2;  
		}  
	}  
};  


int _tmain(int argc, char* argv[])
{
	CvCapture* pCapture = NULL;
	if(argc == 2)
	{
		char* _tempname = "e:\\201505280048_22.mp4";
		if( !(pCapture = cvCaptureFromFile(_tempname)))     
		{   
			fprintf(stderr, "Can not open video file %s\n", argv[1]);   
			return -2;     
		}
	}
	if (argc == 1)
	{
		if( !(pCapture = cvCaptureFromCAM(1)))     
		{   
			fprintf(stderr, "Can not open camera.\n");   
			return -2;     
		} 
	}

	IplImage* pFrame = NULL;
	int countx=0;
	while (pFrame =cvQueryFrame(pCapture))  
	{ 
		countx++;
		IplImage* img1 = cvCreateImage(cvGetSize(pFrame), IPL_DEPTH_8U, 1);//创建目标图像 
		cvCvtColor(pFrame,img1,CV_BGR2GRAY);//cvCvtColor(src,des,CV_BGR2GRAY) 
		//边缘检测   
		cv::Mat result(img1); 
		cv::Mat contours;  
		cv::Canny (result,contours,50,150);
		img1 =&IplImage(contours);	


		int nVer = 1;
		int nHor = 2;
		IplConvKernel* VerKer;
		IplConvKernel* HorKer;
		VerKer = cvCreateStructuringElementEx(1,nVer,0,nVer/2,CV_SHAPE_RECT);
		HorKer = cvCreateStructuringElementEx(nHor,1,nHor/2,0,CV_SHAPE_RECT);
		cvDilate(img1,img1,VerKer);
		cvDilate(img1,img1,HorKer);

		cvMorphologyEx(img1, img1, NULL, NULL, CV_MOP_CLOSE);

		cvSaveImage("a.jpg",img1);

		cv::Mat image(pFrame);
		LineFinder finder;  
		finder.setMinVote (600);  
		finder.setLineLengthAndGap (680,500);
		std::vector<cv::Vec4i> li;
		li = finder.findLines (contours); 
		finder.drawDetectedLines (image); 
		imwrite("123.jpg",image);
		//选择第一条直线 
		//黑色的图像  

// 		for(int i = 0; i < li.size();i++)
// 		{
// 			int n= i;
// 			cv::Mat oneLine(image.size(),CV_8U,cv::Scalar(0));  
// 			cv::Mat oneLineInv;  
// 			//白线  
// 			line(oneLine,cv::Point(li[n][0],li[n][1]),cv::Point(li[n][2],li[n][3]),cv::Scalar(255),5);  
// 			//将轮廓与白线按位与  
// 			bitwise_and(contours,oneLine,oneLine); 		
// 			threshold(oneLine,oneLineInv,128,255,cv::THRESH_BINARY_INV);
// 			//把点集中的点插入到向量中  
// 			std::vector<cv::Point> points;  
// 			//遍历每个像素  
// 			for(int y = 0; y < oneLine.rows;y++)  
// 			{  
// 				uchar* rowPtr = oneLine.ptr<uchar>(y);  
// 				for(int x = 0;x < oneLine.cols;x++)  
// 				{  
// 					if(rowPtr[x])  
// 					{  
// 						points.push_back(cv::Point(x,y));  
// 					}  
// 				}  
// 			}  
// 			//储存拟合直线的容器  
// 			cv::Vec4f line;  
// 			//直线拟合函数  
// 			fitLine(cv::Mat(points),line,CV_DIST_L12,0,0.01,0.01);  
// 			//画一个线段  
// 			int x0= line[2];  
// 			int y0= line[3];  
// 			int x1= x0-200*line[0];  
// 			int y1= y0-200*line[1]; 
// 			if(y0 == y1 /*|| x0 == x1*/)
// 			{
// 				cv::line(image,cv::Point(x0,y0),cv::Point(x1,y1),cv::Scalar(0,255,0),1); 	
// 				imwrite("123.jpg",image);
// 			}
// 		}
// 		
	}
	return 0;
}

