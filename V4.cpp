//UNTESTED

//V3 + Peak finding
//Cannot find larger cells in the background
//get it to work w/ cut off radii


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
//length must be >= width
vector<int> findPeaks(vector<float> rad, int length){//can save "power value" to rule out bad ones (same for bad radii)
  vector<int> peakIndex(length);
  const int width = 7;//width of sample to take the mean of to look at
  if(length < width)
    return peakIndex;
  float minDiff = 8;//min difference between mean of the n values and the center value to be considered a peak
  float val;//computed mean
  int multiplier[width] = {3,4,1,0,1,4,3};//we can use different multipliers for small and large peak detection
  int num; //sum of multipliers, number to divide value by
  int count = 0;
  for(int i = 0; i < width; i++)
    num += multiplier[i];
  for(int i = 0; i < length - 1 - width; i++){
    for(int j = i; j <= width+i; j++)
      val += multiplier[i] * rad[i];
    val /= num;
    if(rad[i+width/2] > val + minDiff){
      peakIndex[count++] = i+width/2;
    }
  }
  return peakIndex;
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
    vector<int> peakIndices(maxRad);
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
          peakIndices = findPeaks(rad,maxRadius);
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













/*

void findPeaks(float * rad, int length, int * lPeakIndex, int * sPeakIndex){//can save "power value" to rule out bad ones (same for bad radii)
  if(length < 9)
    return
  const int sWidth = 5, lWidth = 9;//width of sample to take the mean of to look at
  float sMinDiff = 5, lMinDiff = 10;//min difference between mean of the n values and the center value to be considered a peak
  float sVal, lVal;//computed mean
  int sMultiplier[sWidth] = {3,1,0,1,3}, lMultiplier[lWidth] = {5,5,3,1,0,1,3,5,5};
  int sNum, lNum; //sum of multipliers, number to divide value by
  for(int i = 0; i < sWidth; i++){
    sNum += sMultiplier[i];
  for(int i = 0; i < length - 1 - sWidth; i++){
    for(int j = i; j <= sWidth+i; j++)
      sVal += sMultiplier[i] * rad[i];
    sVal /= sMultiplier;
    if(rad[i+sWidth/2] > sVal/sNum + sMinDiff){
      *sPeakIndex = i+sWidth/2;
      sPeakIndex++;
    }
  }
  for(int i = 0; i < length - 1 - lWidth; i++){
    for(int j = i; j <= lWidth+i; j++)//compute mean
      lVal += lMultiplier[i] * rad[i];
    lVal /= sMultiplier;
    if(rad[i+lWidth/2] > lVal/lNum + lMinDiff){
      *lPeakIndex = i+lWidth/2;//add peak
      lPeakIndex++;
    }
  }
}

*/
