#include <stdio.h>
#include <opencv2/opencv.hpp>

#ifndef NULL
#define NULL 0
#endif

using namespace cv;

class BackgroundExtractor
{
    public:
        BackgroundExtractor( int _threshold=10, int _perfection=100, bool _showBackImg=false) {
            //frameDistance = 1;
            thresholdValue = _threshold;
            perfection = _perfection;
            showBackImg = _showBackImg;
            percentage = 0.9;
            //minFixedPixel = 1000;
            nonZeroPoints = 1;
        }
        
        void createWindow() {
            namedWindow("capture", CV_WINDOW_AUTOSIZE );
        }
        
        void checkWindowSetting() {
            
        }
        
        Mat loop(Mat image) {
            
            if(!isDone())
                feed(image);
            return backImg;
        }
        
        void feed(Mat image) {
            
            int _perfection;
            float nonZero, nonZeroRatio;
            
            Mat threshold1, threshold255, nonChanged;
            Mat newBestsMask, oldBestsMask, newBestRuns, oldBestRuns;
            Mat unstablePoints;
            Mat newBackImgPoints, oldBackImgPoints;
            
            if(!backImg.data) {
                
                backImg = image.clone();
                prevImg = image.clone();
                bestRun = Mat(image.rows,image.cols, CV_8U,1);
                currentRun = Mat(image.rows,image.cols,CV_8U, 1);
                //minFixedPixel = int(image.rows*image.cols*percentage);
                if(showBackImg) {
                    createWindow();
                    imshow("backImage", backImg);
                }
                return;
            }
            
            if(showBackImg)
                checkWindowSetting();
            
            absdiff(prevImg, image, diffImage);
            
            
            
            threshold(diffImage, threshold1, thresholdValue, 1, CV_THRESH_BINARY_INV);
            threshold(diffImage, threshold255, thresholdValue, 255, CV_THRESH_BINARY_INV);
            
            nonZero = countNonZero(threshold1);
            nonZeroRatio = nonZero / (image.rows*image.cols);
            _perfection = perfection;
            
            if(nonZeroRatio < percentage) {
                _perfection = 30;
                printf("perfection = 10\n");
            }
            
            bitwise_and(currentRun, threshold255, nonChanged);
            add(threshold1, nonChanged, currentRun);
            compare(currentRun, bestRun, newBestsMask, CV_CMP_GE);
            compare(currentRun, bestRun, oldBestsMask, CV_CMP_LT);

            bitwise_and(currentRun, currentRun, newBestRuns, newBestsMask);
            bitwise_and(bestRun, bestRun, oldBestRuns, oldBestsMask);
            add(newBestRuns, oldBestRuns, bestRun);

            bitwise_and(image, image, newBackImgPoints, newBestsMask);
            bitwise_and(backImg, backImg, oldBackImgPoints, oldBestsMask);
            add(newBackImgPoints, oldBackImgPoints, backImg);

            compare(bestRun, _perfection, stablePoints, CV_CMP_GT);
            bitwise_not(stablePoints, unstablePoints);
            bitwise_and(stablePoints, _perfection, stablePoints);
            bitwise_and(unstablePoints, bestRun, unstablePoints);
            add(stablePoints, unstablePoints, bestRun);
            
            nonZeroPoints = countNonZero(stablePoints);
            prevImg = image.clone();
            if(showBackImg)
                imshow("backImage", backImg);
        }
        
        bool isDone() {
            
            if(!backImg.data)
                return false;
                
            float nonZeroRatio = (float)nonZeroPoints / (backImg.rows*backImg.cols);
            return (nonZeroRatio<percentage)? false : true;
        }

    private:
        bool showBackImg;
        //int frameDistance;
        int thresholdValue;
        int perfection;
        float percentage;
        //int minFixedPixel;
        Mat stablePoints;
        Mat prevImg;
        Mat backImg;
        Mat bestRun;
        Mat currentRun;
        Mat diffImage;
        int nonZeroPoints;
};
