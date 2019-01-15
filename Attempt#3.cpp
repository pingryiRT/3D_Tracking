//Successfully performs: finding cells, radial averaging (generating array with intensity values corresponding to each radius)
//Peak finding not implemented
//Cannot find larger cells in the background


//Runtime: .8 seconds / image For 3 minute clip of 60 frames / second, approx time: 144 minutes

//VALUES TO BE CHANGED TO REDUCE CODE SIZE: thresh for loop values, number of contours to be examined, maxRad
//OTHER VALUES TO BE CHANGED: conditional satement to determine if it is a cell or not
//change it to work with offscreen


#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <unistd.h>

using namespace cv;
using namespace std;
#define maxRad 200



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
vector<float> radialAveraging(Mat im, int ctrX, int ctrY, int maxRadius){//takes in ORIGINAL BLACK AND WHITE NON-THRESHOLDED cropped image, (x,y) is the center
  vector<float> rad(maxRadius);
  int offsetX, y, numpts = 0;
  long sum = 0;
  for(int r = 0; r < maxRadius; r++){//for every radius
    for(int y = ctrY-r; y <= ctrY+r; y++){
      offsetX = (int) (sqrt(r*r-(y-ctrY)*(y-ctrY))+0.5); //x coordinate at ctrX +/- offsetX
      //for debugging
      if(r == 20)
        circle(im, Point(ctrX + offsetX,y),1, Scalar(255),CV_FILLED);
      sum += (long)im.at<uchar>(y,ctrX + offsetX);
      numpts++;
      if(offsetX != 0){
        if(r == 20)
          circle(im, Point(ctrX - offsetX,y),1, Scalar(255),CV_FILLED);
        sum += (long)im.at<uchar>(y,ctrX - offsetX);
        numpts++;
      }
    }
    rad[r] = (float) sum / numpts;
    //cout << sum << " " << numpts << endl;
    sum = 0;
    numpts = 0;
  }
  namedWindow("contours",0);
  imshow("contours",im);
  return rad;
}
int main(int argc, char** argv){
    //get image
    vector<float> rad(maxRad);
    String imageName("unnamed.png");
    Mat og = imread(imageName, IMREAD_COLOR);
    Mat newIm=og.clone();
    cvtColor(og,og,COLOR_BGR2GRAY);//grayscale
    //namedWindow("gray",0);
    //imshow("gray",image);
    //make sure input is valid
    if( og.empty() )
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    //variables
    vector<vector<Point> > contours;//point has a x and y
    double maxValue = 255;//123 seems pretty good
    std::vector<cv::Vec4i> hierarchy;
    //for minenclosingcircle
    Point2f center;
    float radius = 0, minArea = 2000, PI = 3.14159;
    for(int thresh = 250; thresh >= 50; thresh -= 3){
      Mat image=og.clone();
      threshold(image,image,thresh,min(maxValue,thresh+30),THRESH_BINARY_INV);//first param input, 2nd param output
      findContours(image,contours,hierarchy,CV_RETR_LIST,RETR_TREE);//allows only key points to be added
      if(contours.size() > 0)
        sort(contours.begin(),contours.end(),compCont);
      for(int i = 0; i < contours.size() && i < 7; i++)
      {
         Scalar color = Scalar(125);
         minEnclosingCircle(contours[i],center,radius);
         vector<Point> approx;
         approxPolyDP(contours[i],approx,0.01*arcLength(contours[i],1),1);
         if(approx.size() > 8 && radius*radius*PI/contourArea(contours[i]) < 1.3 && contourArea(contours[i]) > minArea){//airy pattern: inner ring, 15% margin or error, previous values: 8, 1.3
          //cout << "(" << center.x << "," << center.y << "):" << endl;//ANSWERS
          circle(newIm, center,4, Scalar(125),CV_FILLED);
          int maxRadius = min(maxRad,min(min(center.x,center.y),min(og.cols-center.x,og.rows-center.y)));
          rad = radialAveraging(og, center.x, center.y,maxRadius);
         }
      }
    }
    namedWindow("test",0);
    imshow("test",og);
    namedWindow("contours",0);
    imshow("contours",newIm);
    waitKey(0);
  	return 0;
}
