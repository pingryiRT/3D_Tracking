///working code
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
int * radialAveraging(Mat im, int ctrX, int ctrY){//takes in ORIGINAL BLACK AND WHITE NON-THRESHOLDED cropped image, (x,y) is the center
  int maxRadius = min(min(ctrX,ctrY),min(im.cols-ctrX,im.rows-ctrY));
  int rad[maxRadius];
  int offsetX, y, numpts = 0, sum = 0;
  for(int r = 0; r < maxRadius; r++){//for every radius
    for(int y = ctrY-r; y <= ctrY+r; y++){
      offsetX = sqrt(r*r-(y-ctrY)*(y-ctrY)); //x coordinate at ctrX +/- offsetX
      sum += im.at<int>(ctrX + offsetX,y);
      numpts++;
      if(offsetX != 0){
        sum += im.at<int>(ctrX - offsetX,y);
        numpts++;
      }
    }
    rad[r] = sum / numpts;
  }
  return rad;
}

int main(int argc, char** argv){
    //get image
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
    //Mat newIm = Mat::zeros(og.rows, og.cols, CV_64F);
    std::vector<cv::Vec4i> hierarchy;
    //for minenclosingcircle
    Point2f center;
    float radius = 0, minArea = 2000, PI = 3.14159;

    for(int thresh = 280; thresh >= 50; thresh -= 2){
      Mat image=og.clone();
      threshold(image,image,thresh,min(maxValue,thresh+30),THRESH_BINARY_INV);//first param input, 2nd param output
      findContours(image,contours,hierarchy,CV_RETR_LIST,RETR_TREE);//allows only key points to be added
      if(contours.size() > 0)
        sort(contours.begin(),contours.end(),compCont);
      for(int i = 0; i < contours.size() && i < 15; i++)
       {
         Scalar color = Scalar(125);
         minEnclosingCircle(contours[i],center,radius);
         vector<Point> approx;
         approxPolyDP(contours[i],approx,0.01*arcLength(contours[i],1),1);
         //cout << (hierarchy[i][3] != -1) << (approx.size() > 8) << (contourArea(contours[i]) > minArea ) << endl;
         if(/*hierarchy[i][3] != -1 && */approx.size() > 8 && radius*radius*PI/contourArea(contours[i]) < 1.3/*abs(PI*radius*radius - contourArea(contours[i])) < 0.15*contourArea(contours[i])*/  && contourArea(contours[i]) > minArea){//airy pattern: inner ring, 15% margin or error
          //cout << "(" << center.x << "," << center.y << endl;
          //drawContours(newIm, contours, (int)i, color);
          circle(newIm, center,4, Scalar(125),CV_FILLED);
          //cout << "vals:" << radius*radius*PI/contourArea(contours[i]) << endl;//<< abs(PI*radius*radius - contourArea(contours[i])) << " " << 0.15*contourArea(contours[i]) << endl;
          //cout << approx.size() << " " << radius*radius*PI/contourArea(contours[i]) << endl;
          cout << contourArea(contours[i]) << endl;
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
