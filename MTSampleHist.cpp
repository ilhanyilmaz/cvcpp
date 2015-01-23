#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "motionTrackerHist.h"
#include <string>

using namespace cv;

#define DEBUG 1

int main(int argc, char** argv )
{
    int keyCode;
    VideoCapture cap;
    if ( argc == 2 )
    {
        string filepath = argv[1];
        printf("%s\n",argv[1]);
        
        if(argv[1] == "homestream")
            cap.open("http://192.168.1.21:8080?action=stream");
        else if(argv[1] == "workstream")
            cap.open("http://192.168.254.116:8080?action=stream");
        else if(argv[1] == "androidstream")
            cap.open("http://192.168.43.116:8080?action=stream");
        else
            cap.open(argv[1]);
    }
    else
        cap.open(0);
    
    if(!cap.isOpened())
        return -1;
    
    //cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    //cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    
    if(DEBUG)
        namedWindow("capture", CV_WINDOW_AUTOSIZE );
    vector<string> posImages;
    posImages.push_back("./sample/captures/kitap.jpg");
    MotionTrackerHist mtb(cap, posImages, vector<string>(), false, false);
    
    Mat frame, gray;
    //Mat backImg, diffImg;
    Point biggestObjectRect;
    while(true) {
        cap >> frame;
        if(!frame.data)
            return -1;
        
        imshow("capture", frame);
        
        mtb.update(frame.clone());
        mtb.findMovingObjects();
        
        mtb.drawRectangles();
        int biggestObjectPos = mtb.findBiggestMovingObjectPos();
        if(biggestObjectPos != -1) {
            Point loc = mtb.findObject2dBottom(biggestObjectPos);
            if(DEBUG) {
                circle(frame, loc, 10, Scalar(255,0,0));
                imshow("capture", frame);
            }
            printf("%i-%i\n",loc.x, loc.y);
        }
        
        
        keyCode = waitKey(10);
        if(keyCode == 1048608) { // space
                
        }
        if(keyCode >= 0) {
            printf("%d", keyCode);
            break;
        }
    }
    

    return 0;
}
