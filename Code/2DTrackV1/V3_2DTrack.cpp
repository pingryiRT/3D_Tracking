//semi-automated (use for blurry images)

///ignore first value
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <unistd.h>
#include <math.h>

using namespace cv;
using namespace std;

#define numCells 1200

#define WIDTH 1200
#define HEIGHT 850

int min(int x, int y){
	if(x > y)
		return y;
	return x;
}

int max(int x, int y){
	if(x > y)
		return x;
	return y;
}

int main(int argc, char** argv){
	  ifstream in;
		ofstream out;
		out.open("Movie4Data.csv");
    //get image
    Size size(WIDTH,HEIGHT);
    String videoName = "Feb 22 Directed trial 2 cropped.mp4";//chlamy 4
    VideoCapture capture(videoName);
		VideoCapture capture2(videoName);
    Mat frame, fp, b2;
    if(!capture.isOpened())
        throw "Error when reading steam_avi";
    namedWindow("raw",0);
		//namedWindow("raw2",0);
		//namedWindow("raw3",0);
    //namedWindow("processed",0);
    vector<vector<Point> > cells(numCells);
		vector<int> startTime(numCells);//start time for each cell
		vector<int> endTime(numCells);
		int nUserCells = 8;
		cells[0].push_back(Point2f(300,840));
		startTime[0] = 28;
		endTime[0] = 70;
		cells[1].push_back(Point2f(5,820));
		startTime[1] = 10;
		endTime[1] = 50;
		cells[2].push_back(Point2f(50,835));
		startTime[2] = 27;
		endTime[2] = 42;
		cells[3].push_back(Point2f(10,812));
		startTime[3] = 40;
		endTime[3] = 70;
		cells[4].push_back(Point2f(350,840));
		startTime[4] = 54;
		endTime[4] = 85;
		cells[5].push_back(Point2f(350,840));
		startTime[5] = 65;
		endTime[5] = 78;
		cells[5].push_back(Point2f(100,840));
		startTime[5] = 65;
		endTime[5] = 90;
		cells[6].push_back(Point2f(295,245));
		startTime[6] = 23;
		endTime[6] = 32;
		cells[7].push_back(Point2f(160,835));
		startTime[7] = 73;
		endTime[7] = 90;
		capture >> frame;
		//capture >> frame;
		//capture >> frame;
		//capture >> frame;
		int frameNum = 0, a;
		//cells.push_back()
    while(1)
    {
			frameNum++;
			  cout << frameNum << endl;
			  //for display
			  Mat processed(size.height,size.width, CV_8UC3, Scalar(255,255,255));
        capture >> frame;
				capture2 >> fp;
        if(frame.empty())
            break;
        resize(frame,frame,size);
				cvtColor(frame,frame, COLOR_BGR2GRAY);
				for(int i = 0; i < nUserCells; i++){
					int minVal = 255;
					int bestX, bestY;
					if(frameNum >= startTime[i] && frameNum < endTime[i]){
						for(int x = max(5,cells[i].back().x-25); x < min(WIDTH-5,cells[i].back().x+25);x++){////////////NEED TO USE DIFFERENT VALUES DEPENDING ON PROXIMITY OF CELLS AND MAXIMUM JUMP: 25 for i = 5, 35 for everything else
							for(int y = max(5,cells[i].back().y-25); y < min(HEIGHT-5,cells[i].back().y+25);y++){
								float sum = 0;
								for(int x1 = x - 5; x1 < x + 5; x1++){
									for(int y1 = y - 5; y1 < y + 5; y1++){
										sum += frame.at<uchar>(y1,x1);
									}
								}
								sum /= 100.0;
								if(sum < minVal){
									//cout << x << " " << y << endl;
									bestX = x;
									bestY = y;
									minVal = sum;
								}
							}
						}
						cells[i].push_back(Point(bestX,bestY));
						//cout << "(" << bestX << "," << bestY << ")" << endl;
					}
				}
				cvtColor(frame,frame,COLOR_GRAY2RGB);
				//circle(frame, Point2f(160,835),2, 255,CV_FILLED);//175 840     70
				for(int i = 0; i < nUserCells; i++){
					if(frameNum > startTime[i] && frameNum < endTime[i]){
						if(i != 6)
							circle(frame, cells[i].back(),20, Scalar(0,0,255),3/*,CV_FILLED*/);
					}
				}
				imshow("raw",frame);
				/*if(frameNum == 73){
					cin >> a;
				}*/
        waitKey(1); // waits to display frame
    }
    for(int i = 0; i < nUserCells; i++){
			out << "Cell" << i << ":" << endl;
      for(int j = 1; j < cells[i].size(); j++){
				out << "," << cells[i][j].x << "," << cells[i][j].y << endl;
      }
    }
    //waitKey(0);
  	return 0;
}
