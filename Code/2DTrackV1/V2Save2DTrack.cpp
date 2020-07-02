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

bool compCont(vector<Point> contour1, vector<Point> contour2){
	double i = contourArea(contour1,false);//always want area to be positive, dont want oriented area
	double j = contourArea(contour2,false);
	return (i > j);
}

void imgSubt(Mat curr, Mat last){
	for(int i = 0; i < curr.rows; i++)
		for(int j = 0; j < curr.cols; j++){
			if((long)curr.at<uchar>(i,j) - 100 >= (long)last.at<uchar>(i,j)) //definately not cell: cells dark --> lower value
				curr.at<uchar>(i,j) = 0;
			else{
				curr.at<uchar>(i,j) = (long)last.at<uchar>(i,j) - (long)curr.at<uchar>(i,j) + 100;
			}
		}
}

struct rgb{
	char r;
	char g;
	char b;
};

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
		out << "a1,b1,c1" << endl;
		out << "a2" << endl;
    //get image
    Size size(WIDTH,HEIGHT);
    String videoName = "Feb 22 Directed trial 2 cropped.mp4";//chlamy 4
    VideoCapture capture(videoName);
		VideoCapture capture2(videoName);
    Mat frame, fp, b2;
    if(!capture.isOpened())
        throw "Error when reading steam_avi";
    namedWindow("raw",0);
		namedWindow("raw2",0);
		namedWindow("raw3",0);
    //namedWindow("processed",0);

    vector<vector<Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    Point2f center;
    float radius = 0;

    vector<vector<Point> > cells(numCells);
		vector<int> startTime(numCells);//start time for each cell
		vector<int> endTime(numCells);
		int nUserCells = 2;
		cells[0].push_back(Point2f(300,840));
		startTime[0] = 28;
		endTime[0] = 70;
		cells[1].push_back(Point2f(5,820));
		startTime[1] = 10;
		endTime[1] = 70;
		//end of tracking a single cell
    Point2f end(-1, -1);
    int count = 0;

		//for display coloring
		rgb colors[numCells];
		for(int i = 0; i < numCells; i++){
			colors[i].r = (rand() % 200) + 50;
			colors[i].g = (rand() % 200)  + 50;
			colors[i].b = (rand() % 200) + 50;
		}
		capture2 >> fp;
		resize(fp,fp,size);
		cvtColor(fp,fp, COLOR_BGR2GRAY);
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

				/*use w/ background subtractino
				resize(fp,fp,size);
				cvtColor(fp,fp, COLOR_BGR2GRAY);
				//b2 = frame.clone();
				imgSubt(frame,fp);
				//fp = b2;
				imshow("raw",frame);
				GaussianBlur(frame,frame,Size(7,7),0,0);
				//GaussianBlur(frame,frame,Size(7,7),0,0);
				//GaussianBlur(frame,frame,Size(7,7),0,0);
				imshow("raw2",frame);
        threshold(frame,frame,80,255,THRESH_BINARY);//130 255
				imshow("raw3",frame);*/
				/*use without background subtraction*/
				for(int i = 0; i < nUserCells; i++){
					int minVal = 255;
					int bestX, bestY;
					if(frameNum > startTime[i]){
						for(int x = max(5,cells[i].back().x-20); x < min(WIDTH-5,cells[i].back().x+20);x++){
							for(int y = max(5,cells[i].back().y-20); y < min(HEIGHT-5,cells[i].back().y+20);y++){
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
				for(int i = 0; i < nUserCells; i++){
					if(frameNum > startTime[i] && frameNum < endTime[i])
					//cout << "a(" << cells[i].back().x << "," << cells[i].back().y << ")" << endl;
						circle(frame, cells[i].back(),2, 255,CV_FILLED);
				}
				imshow("raw",frame);
				if(frameNum >= 9){
					cout << "in" << endl;
					cin >> a;
				}
				/*GaussianBlur(frame,frame,Size(7,7),0,0);
				GaussianBlur(frame,frame,Size(7,7),0,0);
				GaussianBlur(frame,frame,Size(7,7),0,0);
				imshow("raw2",frame);
        adaptiveThreshold(frame,frame,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,5,2);//second to last number is kernel size, last is constant to subtract
				//3 1     7 4      5 2
				imshow("raw3",frame);*/
        /*findContours(frame,contours,hierarchy,CV_RETR_LIST,RETR_TREE);//allows only key points to be added
        if(contours.size() > 0)
          sort(contours.begin(),contours.end(),compCont);
		  	//get cells that disappeared
		  	vector<bool> present(cells.size(),false);
	  		for(int i = 0; i < cells.size(); i++){
  				if(cells[0].size() == 0 || cells[0].back().x == -1){
 	  				present[i] = true;///already registered cell has disappeared, or cell has not yet been registered
   				}
	  		}
        //main loop to process each contour
        for(int i = 0; i < contours.size() && contourArea(contours[i]) > 5; i++)
         {
            minEnclosingCircle(contours[i],center,radius);
						//cout << center << endl;
            circle(processed, center,6, Scalar(0,0,255),CV_FILLED);//default color when not tracked
            int possibleIdx = -1;
            //now processing each current cell
            for(int j = 0; j < cells.size(); j++){
              if(cells[j].size() == 0){
								if(possibleIdx == -1){
	                cells[j].push_back(center);
									present[j] = true;
									circle(processed, center,6, Scalar(0,255,0),CV_FILLED);
							  }
								break;
              }
							unsigned long long a = pow(cells[j].back().x - center.x,2) + pow(cells[j].back().y - center.y,2);
              if(a < 2000 && cells[j].back().x != -1){
                if(possibleIdx != -1){//collision: lost cells
									//cout << "in" << endl;
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
							circle(processed, center,6, Scalar(255,0,0),CV_FILLED);
							cout << "test" << endl;
              present[possibleIdx] = true;
            }
         }
				 for(int j = 0; j < present.size(); j++){
					 if(!present[j] && cells[j].size() != 0)
						 cells[j].push_back(end);
				 }
        imshow("processed",frame);
				imshow("processed2",processed);*/
        waitKey(1); // waits to display frame
    }
		//print out x coordinates
    cout << cells.size() << endl;
    for(int i = 0; i < cells.size(); i++){
			cout << "Cell" << i << ":" << endl;
      for(int j = 0; j < cells[i].size(); j++){
        if(cells[i][j].x == -1)
          break;
        //cout << "(" << cells[i][j].x << "," << cells[i][j].y << ")" << endl;
				cout << cells[i][j].x << endl;
      }
    }
		//print out y coordinates
		for(int i = 0; i < cells.size(); i++){
			cout << "Cell" << i << ":" << endl;
			for(int j = 0; j < cells[i].size(); j++){
				if(cells[i][j].x == -1)
					break;
				//cout << "(" << cells[i][j].x << "," << cells[i][j].y << ")" << endl;
				cout << cells[i][j].y << endl;
			}
		}
    waitKey(0);
  	return 0;
}
