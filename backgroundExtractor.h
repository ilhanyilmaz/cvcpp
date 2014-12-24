#include <stdio.h>
#include <opencv2/opencv.hpp>
#ifndef NULL
#define NULL 0
#endif

using namespace cv;

class BackgroundExtractor
{
    public:
        BackgroundExtractor() {
            //frameDistance = 1;
            thresholdValue = 10;
            perfection = 50;
            //percentage = 0.995;
            //minFixedPixel = 1000;
            //nonZeroPoints = 1;
        }
        
        void feed(Mat image) {
            if(!backImage.data) {
                
                //diffImage = Mat(image.rows,image.cols,CV_8U);
                backImage = image.clone();
                
                //minFixedPixel = int(image.rows*image.cols*percentage);
                return;
            }
            absdiff(backImage, image, diffImage);
            
            Mat threshold1, threshold255, fixedPoints255, notFixedPoints255, nonChanged, nonChangedPlus1;
            Mat newFixedPoints255, totalFixedPoints255, totalNonFixedPoints255;
            Mat tempMat, oldImageMask, newImageMask;
            Mat newBackImagePoints, oldBackImagePoints;
            
            threshold(diffImage, threshold1, thresholdValue, 1, CV_THRESH_BINARY_INV);
            threshold(diffImage, threshold255, thresholdValue, 255, CV_THRESH_BINARY_INV);
            
            if(!checkMat.data) {
                checkMat = threshold1.clone();
                return;
            }
            
            threshold(checkMat, fixedPoints255, perfection, 255, CV_THRESH_BINARY);
            bitwise_not(fixedPoints255, notFixedPoints255);
            
            bitwise_and(checkMat, threshold255, nonChanged);
            add(threshold1, nonChanged, nonChangedPlus1);
            
            threshold(nonChangedPlus1, newFixedPoints255, perfection, 255, CV_THRESH_BINARY);
            bitwise_or(fixedPoints255, newFixedPoints255, totalFixedPoints255);
            bitwise_not(totalFixedPoints255, totalNonFixedPoints255);
            
            bitwise_or(nonChangedPlus1, totalFixedPoints255, tempMat);
            threshold(tempMat, oldImageMask, 1, 255, CV_THRESH_BINARY);
            bitwise_not(oldImageMask, newImageMask);
            
            bitwise_and(image, image, newBackImagePoints, newImageMask);
            bitwise_and(backImage, backImage, oldBackImagePoints, oldImageMask);
            
            add(newBackImagePoints, oldBackImagePoints, backImage);
            
            bitwise_or(nonChangedPlus1, totalFixedPoints255, checkMat);
            //nonZeroPoints = countNonZero(totalFixedPoints255);
            
            
        }

    public:
        //int frameDistance;
        int thresholdValue;
        int perfection;
        //float percentage;
        //int minFixedPixel;
        Mat backImage;
        Mat checkMat;
        Mat diffImage;
        //int nonZeroPoints;
};
