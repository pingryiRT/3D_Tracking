//semi-automated (use for blurry images)

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <unistd.h>
#include <math.h>
#include <string>

using namespace cv;
using namespace std;


//doesn't work w/ zeroes
int main(int argc, char** argv){
		//int good[19] = {12,17,18,27,32,33,35,36,44,45,47,52,53,54,60,61,65,69,76};
		int goodIdx = 0;
	  ifstream in;
		ofstream out;
		in.open("RandomMotilityData.csv");
    out.open("AbrigedRandomMotility2.csv");
    string s;
		char x,y;
		int cellCount = 0;
		int num1, num2, prevNum1 = -1, prevNum2 = -1;
    for(int i = 0; i < 10000; i++){
      in >> x;
			if(x == 'C'){
				in >> s;
				/*while(cellCount != good[goodIdx]){
					x = 'A';
					while(x != 'C'){
						in >> x;
						in >> s;
					}
					cellCount++;
				}*/
				goodIdx++;
				out << "Cell " << cellCount << endl;
				cellCount++;
				prevNum1 = -1;
				continue;
			}
			num1 = x - '0';
			while(true){
				in >> x;
				if(x == ',')
					break;
				num1 = num1 * 10 + x - '0';
			}
			in >> num2;
			if(num1 != prevNum1 || num2 != prevNum2){
				if(prevNum1 == -1)//first # for cell --> don't print velocity
					out << "," << num1 << "," << num2 << endl;
				else{
					int v1 = num1-prevNum1, v2 = num2-prevNum2;
					out << "," << num1 << "," << num2 << "," << v1 << "," << v2 << endl;
				}
			}
			prevNum1 = num1;
			prevNum2 = num2;
			if(goodIdx == 20)
				break;
    }
    /*for(int i = 0; i < nUserCells; i++){
			out << "Cell" << i << ":" << endl;
      for(int j = 1; j < cells[i].size(); j++){
				out << "," << cells[i][j].x << "," << cells[i][j].y << endl;
      }
    }*/
  	return 0;
}
