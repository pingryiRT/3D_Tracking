#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
using namespace cv;
using namespace std;

int min(int a, int b){
  if(a < b)
    return a;
  else
    return b;
}
bool compCont(vector<Point> contour1, vector<Point> contour2){
	double i = contourArea(contour1,false);//always want area to be positive, dont want oriented area
	double j = contourArea(contour2,false);
	return (i > j);
}


int main(int argc, char** argv){
    //get image
    String imageName("unnamed.png");
    Mat og = imread(imageName, IMREAD_COLOR);
    Mat newIm=og.clone();
    cvtColor(og,og,COLOR_BGR2GRAY);//grayscale
    if( og.empty() )
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    //variables
    vector<vector<Point> > contours;
    double maxValue = 255;
    std::vector<cv::Vec4i> hierarchy;
    Point2f center;
    float radius = 0, minArea = 10, PI = 3.14159;

    for(int thresh = 280; thresh >= 50; thresh -= 2){//check multiple thresh ranges
      Mat image=og.clone();
      threshold(image,image,thresh,maxValue,THRESH_BINARY_INV);
      findContours(image,contours,hierarchy,CV_RETR_LIST,RETR_TREE);
      if(contours.size() > 0)
        sort(contours.begin(),contours.end(),compCont);
      for(int i = 0; i < contours.size() && i < 15; i++)
       {
         Scalar color = Scalar(125);
         minEnclosingCircle(contours[i],center,radius);
         vector<Point> approx;
         approxPolyDP(contours[i],approx,0.01*arcLength(contours[i],1),1);
         if(approx.size() > 8 && radius*radius*PI/contourArea(contours[i]) < 2 && contourArea(contours[i]) > minArea){
          //cout << "(" << center.x << "," << center.y << endl;
          circle(newIm, center,4, Scalar(125),CV_FILLED);
        }
       }
     }
    namedWindow("contours",0);
    imshow("contours",newIm);
    waitKey(0);
  	return 0;
}
