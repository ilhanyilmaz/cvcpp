#include <stdio.h>
#include <opencv2/opencv.hpp>
//#include "backgroundExtractor.h"
#include "motionTrackerBack.h"
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
        //printf("%s\n",argv[1]);
        cap.open(filepath);
        //printf("usage: DisplayImage.out <Image_Path>\n");
    }
    else
        cap.open(0);
    
    if(!cap.isOpened())
        return -1;
    
    //cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    //cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    
    if(DEBUG)
        namedWindow("capture", CV_WINDOW_AUTOSIZE );
    
    //BackgroundExtractor be(10,100,true);
    MotionTrackerBack mtb(cap, 5, 100);
    
    Mat frame, gray;
    //Mat backImg, diffImg;
    Point biggestObjectRect;
    while(true) {
        cap >> frame;
        if(!frame.data)
            return -1;
        cvtColor(frame, gray, CV_BGR2GRAY);
        
        mtb.update(gray);
        mtb.findMovingObjects();
        mtb.drawRectangles();
        int biggestObjectPos = mtb.findBiggestMovingObjectPos();
        if(biggestObjectPos != -1) {
            Point loc = mtb.findObject2dBottom(biggestObjectPos);
            if(DEBUG) {
                circle(frame, loc, 10, Scalar(255,0,0));
                imshow("capture", frame);
            }
            //printf("%i-%i\n",loc.x, loc.y);
        }
        /*if(diffImg.data)
            imshow("capture", diffImg);
          */  
        
        //if(DEBUG)
        keyCode = waitKey(30);
        if(keyCode == 1048608) { // space
                
        }
        if(keyCode >= 0) {
            printf("%d", keyCode);
            break;
        }
    }
    

    return 0;
}
