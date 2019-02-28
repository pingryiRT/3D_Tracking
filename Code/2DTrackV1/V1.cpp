#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <unistd.h>
#include <math.h>

using namespace cv;
using namespace std;

#define numCells 3000

bool compCont(vector<Point> contour1, vector<Point> contour2){
	double i = contourArea(contour1,false);//always want area to be positive, dont want oriented area
	double j = contourArea(contour2,false);
	return (i > j);
}

struct rgb{
	char r;
	char g;
	char b;
};

int main(int argc, char** argv){
    //get image
    Size size(1000,700);
    String videoName = "chlamy 4.avi";
    VideoCapture capture(videoName);
		VideoCapture capture2(videoName);
    Mat frame;
    if(!capture.isOpened())
        throw "Error when reading steam_avi";
    namedWindow("raw",0);
    namedWindow("processed",0);

    vector<vector<Point> > contours;
    std::vector<cv::Vec4i> hierarchy;
    Point2f center;
    float radius = 0;

    vector<vector<Point> > cells(numCells);

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
		int c = 0;
    while(1)
    {
			  c++;
			  //for display
			  Mat processed(size.height,size.width, CV_8UC3, Scalar(255,255,255));
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
            circle(processed, center,40, Scalar(0,0,255),CV_FILLED);//default color when not tracked
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
							//if(j > 12)
						  //	cout << cells[12].size() << " " << cells[12].back().x << endl;
							unsigned long long a = pow(cells[j].back().x - center.x,2) + pow(cells[j].back().y - center.y,2);
              if(a < 2000 && cells[j].back().x != -1 && !present){
								//cout << "(" << cells[j].back().x << "," << cells[j].back().y << ")" << "(" << center.x << "," << center.y << ")" << endl;
                if(possibleIdx != -1){//collision: lost cells
									cout << "in" << endl;
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
              present[possibleIdx] = true;
            }
         }
				 for(int j = 0; j < present.size(); j++){
					 if(!present[j] && cells[j].size() != 0)
						 cells[j].push_back(end);
				 }
        imshow("processed",processed);
        waitKey(5); // waits to display frame
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

    //checking work
		/*capture2 >> frame;
		resize(frame,frame,size);
		imshow("raw",frame);
		cvtColor(frame,frame, COLOR_BGR2GRAY);
		GaussianBlur(frame,frame,Size(7,7),0,0);
		adaptiveThreshold(frame,frame,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,7,3);
		for(int i = 0; i < cells.size(); i++){
			circle(frame, cells[i][0],6, Scalar(125),CV_FILLED);
		}
		imshow("processed",frame);*/
    waitKey(0);
  	return 0;
}
