#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "backgroundExtractor.h"

using namespace cv;

int main(int argc, char** argv )
{
    /*if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }*/
    VideoCapture cap(0);
    
    if(!cap.isOpened())
        return -1;
    
    //cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
    //cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
    
    namedWindow("capture", CV_WINDOW_AUTOSIZE );
    
    BackgroundExtractor be(10,100,true);
    Mat frame;
    Mat gray;
    
    while(true) {
        cap >> frame;
        if(!frame.data)
            return -1;
        cvtColor(frame, gray, CV_BGR2GRAY);
        be.feed(gray);
        
        //if(be.backImage.data)
        //    imshow("capture", be.backImage);
        if(waitKey(30) >= 0) break;
    }
    

    return 0;
}
