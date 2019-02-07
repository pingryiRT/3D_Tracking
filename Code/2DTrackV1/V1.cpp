

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <unistd.h>
#include <math.h>

using namespace cv;
using namespace std;

bool compCont(vector<Point> contour1, vector<Point> contour2){
	double i = contourArea(contour1,false);//always want area to be positive, dont want oriented area
	double j = contourArea(contour2,false);
	return (i > j);
}

int main(int argc, char** argv){
    //get image
    Size size(1000,700);
    String videoName = "chlamy 2.avi";
    VideoCapture capture(videoName);
    Mat frame;
    if(!capture.isOpened())
        throw "Error when reading steam_avi";
    namedWindow("raw",0);
    namedWindow("processed",0);

    vector<vector<Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    Point2f center;
    float radius = 0;

    vector<vector<Point> > cells(300);

    Point2f end(-1, -1);
    int count = 0;
    while(1)
    {
        cout << count++ << endl;
        if(count == 50){
          cout << "breaking" << endl;
          break;
        }
        capture >> frame;
        if(frame.empty())
            break;
        resize(frame,frame,size);
        imshow("raw",frame);
        cvtColor(frame,frame, COLOR_BGR2GRAY);
        GaussianBlur(frame,frame,Size(7,7),0,0);
        adaptiveThreshold(frame,frame,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,7,3);
        findContours(frame,contours,hierarchy,CV_RETR_LIST,RETR_TREE);//allows only key points to be added
        if(contours.size() > 0)
          sort(contours.begin(),contours.end(),compCont);
        //main loop to process each contour
        for(int i = 0; i < contours.size() && contourArea(contours[i]) > 15; i++)
         {
            minEnclosingCircle(contours[i],center,radius);
            circle(frame, center,6, Scalar(125),CV_FILLED);
            int possibleIdx = -1;
            //get cells that disappeared
            vector<bool> present(cells.size(),false);
            for(int i = 0; i < cells.size(); i++){
              if(cells[0].size() > 0 && cells[0].back().x != -1){
                present[i] = true;
              }
            }
            //now processing each current cell
            for(int j = 0; j < cells.size(); j++){
              if(cells[j].size() == 0){
                cout << cells[j];
                cells[j].push_back(center);
                break;
              }
              if(pow(cells[j].back().x + center.x,2) + pow(cells[j].back().y + center.y,2) < 100){//10^2
                if(possibleIdx != -1){//collision: lost cells
                  cells[j].push_back(end);
                  cells[possibleIdx].push_back(end);
                  present[j] = true;
                  present[possibleIdx] = true;
                  possibleIdx = -1;
                  break;
                }
                possibleIdx = j;
              }
            }
            if(possibleIdx != -1){
              cells[possibleIdx].push_back(center);
              present[possibleIdx] = true;
            }
            for(int j = 0; j < present.size(); j++){
              if(!present[j])
                cells[j].push_back(end);
            }
         }
        imshow("processed",frame);
        waitKey(5); // waits to display frame
    }
    cout << cells.size() << endl;
    for(int i = 0; i < cells.size(); i++){
      cout << "Cell" << i << ":" << endl;
      for(int j = 0; j < cells[i].size(); j++){
        if(cells[i][j].x == -1)
          break;
        cout << "(" << cells[i][j].x << "," << cells[i][j].y << ")" << endl;
      }
    }
    waitKey(0);
  	return 0;
}
