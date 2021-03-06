/*
	*This cpp implements the simplest License Plate locate
	*simple image is stored images directory
	*
	*
	*website: http://www.eefocus.com/jefby1990/blog/
	*email: jef1990@gmail.com
	*Author : jefby
	*Date:	2013/5/27
*/
//	opencv 2.0 header file
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
//	#include "opencv2/core/core.hpp"
//	for function printf 
#include <stdio.h> 
//	#include <map>
#include <vector>
#include <cmath>
//	#include <iostream>	

using namespace std;
using namespace cv;

//	some variable defines 
//	img_src defines the picture which reads through imread
//	img_gray is gray image of img_src
//	grad_x is the image which uses the sobel grad in x direction
//	grad_y is the image which uses the sobel grad in y direction
//	grad is the mixture of 0.5 grad_x and 0.5 grad_y
//	img_bin_thre is the picture which use the OTSU threshold binarization 
Mat img_src;
Mat img_gray;
Mat grad_x,grad_y,abs_grad_x,abs_grad_y,grad;
//	Mat grad;
Mat img_bin_thre;
Mat img_get_rect;
Mat img_lines;


int main(int argc,char**argv)
{
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
//	check the number of parameter
	if(argc !=2)
	{	
		printf("please follow like this\n");
		printf("exe[] img_name\n");
		return -1;
	}
//	img_src = imread(argv[1],CV_LOAD_IMAGE_COLOR);
//	reads image
	img_src = imread(argv[1]);
//	check whether read operation is ok or not 
	if(img_src.data == NULL)
	{	
		printf("could not open or find the image!\n");
		return -1;
	}
//	namedWindow("src Windows",CV_WINDOW_NORMAL);
//	namedWindow("gray Windows",CV_WINDOW_NORMAL);
//	imshow(const string & winname,InputArray mat)
//	show the original image
//	imshow("src Windows",img_src);
//	convert to the gray 
//	cvtColor(img_src,img_gray,CV_BGR2GRAY);
//	show the gray image
//	imshow("gray Windows",img_gray);

//	use Gaussian blur to reduce the noise
	GaussianBlur(img_src,img_src,Size(3,3),0,0,BORDER_DEFAULT);
	namedWindow("Gaussian",CV_WINDOW_NORMAL);
//	show the image after gaussion blur 
	imshow("Gaussian",img_src);
//	wait for user 
//	waitKey(0);

//	convert source image to gray image
	cvtColor(img_src,img_gray,CV_BGR2GRAY);
//	sobel in x direction
	Sobel(img_gray,grad_x,ddepth,1,0,3,scale,delta,BORDER_DEFAULT);
	convertScaleAbs(grad_x,abs_grad_x);
//	show the result	
	namedWindow("GradientX",CV_WINDOW_NORMAL);
	imshow("GradientX",abs_grad_x);

//	use sobel in y direction
	Sobel(img_gray,grad_y,ddepth,0,1,3,scale,delta,BORDER_DEFAULT);
	convertScaleAbs(grad_y,abs_grad_y);
//	namedWindow("GradientY",CV_WINDOW_NORMAL);
//	show the result
//	imshow("GradientY",abs_grad_y);
//	add weight,and 
	addWeighted(abs_grad_x,0.5,abs_grad_y,0.5,0,grad);
//	namedWindow("addweight",CV_WINDOW_NORMAL);
//	imshow("addweight",grad);

//	use threshold to binarition
	threshold(grad,img_bin_thre,0,255,THRESH_BINARY|THRESH_OTSU);
	namedWindow("thresh_otsu",CV_WINDOW_NORMAL);
	imshow("thresh_otsu",img_bin_thre);

/*
//	first Dilate,second erode
	Mat element = getStructuringElement(MORPH_RECT,Size(2*1+1,2*1+1),Point(1,1));
	dilate(img_bin_thre,img_bin_thre,element);
	namedWindow("dilated",CV_WINDOW_NORMAL);
	imshow("dilated",img_bin_thre);
	erode(img_bin_thre,img_bin_thre,element);
	namedWindow("erode",CV_WINDOW_NORMAL);
	imshow("erode",img_bin_thre);
*/
	Mat element = getStructuringElement(MORPH_RECT,Size(2*1+1,2*1+1),Point(-1,-1));
	for(int i = 0;i < 3; i++)
	{
//		erode(img_bin_thre,img_bin_thre,element);
//		erode(img_bin_thre,img_bin_thre,element);
		morphologyEx(img_bin_thre,img_bin_thre,MORPH_OPEN,element);
//		dilate(img_bin_thre,img_bin_thre,element);
//		erode(img_bin_thre,img_bin_thre,element);
		morphologyEx(img_bin_thre,img_bin_thre,MORPH_CLOSE,element);
//		erode(img_bin_thre,img_bin_thre,element);
//		dilate(img_bin_thre,img_bin_thre,element);

		
//		erode(img_bin_thre,img_bin_thre,element);	
	}	

	namedWindow("Morpho",CV_WINDOW_NORMAL);
	imshow("Morpho",img_bin_thre);

//	find contour
//	define 
	vector<Vec4i> hierarchy;
	vector< vector<Point> >contours;
//	use function
	findContours(img_bin_thre,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE,Point(0,0));
//	please change min and the max area value based on reality
	int min_area = 100000;
	int max_area = 300000;
	Rect mRect;
	int tempArea;
//	define the color drawing contour
	Scalar color = Scalar(255,255,0);
	Mat drawing = Mat::zeros(img_bin_thre.size(),CV_8UC1);
	for(int i = 0;i < contours.size();i++)
	{
//	get the minimum rectangle of the contours
		mRect = boundingRect(contours[i]);
//	computer the square of mRect
		tempArea = mRect.height * mRect.width;
//	for debug
//		printf("tempArea.height:%d\ttempArea.width:%d\ttempArea.area=%d\n",mRect.height,mRect.width,tempArea);
//	filter area which meet the requirement
		if(((double)mRect.width/(double)mRect.height) > 2.0 && (tempArea > min_area) && ((double)mRect.width/(double)mRect.height < 4) && (tempArea < max_area))
//	draw contours
		{
			drawContours(drawing,contours,i,color,2,8,hierarchy);
			getRectSubPix(img_bin_thre,Size(mRect.width,mRect.height),Point(mRect.x+mRect.width/2,mRect.y\
					 +mRect.height/2),img_get_rect);	
		}
	}
	namedWindow("contours",CV_WINDOW_NORMAL);
	imshow("contours",drawing);
	namedWindow("getRect",CV_WINDOW_NORMAL);
	imshow("getRect",img_get_rect);

	waitKey(0);
	return 0;
}
