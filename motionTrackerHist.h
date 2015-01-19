
#include "motionTracker.h"

class MotionTrackerHist: public MotionTracker
{
    public:
        
        MotionTrackerHist(VideoCapture _cap, vector<string> posHistFile=vector<string>(), vector<string> negHistFile=vector<string>(), bool _showTracker=false, bool _showDiffImg=false):MotionTracker(_cap, _showTracker, _showDiffImg) {
            
            Mat image;

            
            if(!posHistFile.empty()) {
                for(int i=0; i<posHistFile.size(); i++) {
                    printf("opening positive file: %s\n", posHistFile[i].c_str());
                    image = imread(posHistFile[i], CV_LOAD_IMAGE_COLOR);   // Read the file
                    addHistogram(image, "+");
                }
            }
            if(!negHistFile.empty()) {
                for(int i=0; i<posHistFile.size(); i++) {
                    printf("opening negative file: %s\n", posHistFile[i].c_str());
                    image = imread(posHistFile[i], CV_LOAD_IMAGE_COLOR);   // Read the file
                    addHistogram(image, "-");
                }
            }
        }
        
        void update(Mat _frame) {
            Mat diffImageRGB;
            frame = _frame.clone();
            blur(frame, frame, Point(blurValue, blurValue));
            
            diffImageRGB = removeBackground(frame);
            cvtColor(diffImageRGB, diffImg, CV_BGR2GRAY);
            
        }
    
        void addHistogram(Mat crop, string posNeg) {
            
            Mat hsv, hist;
            MatND roiHist;
            
            cvtColor(crop, hsv, CV_BGR2HSV);
            //inRange(hsv, Scalar(0, 10,10), Scalar(180,256,256), hsv);
            
            
            int hbins = 180, sbins = 256;
            int histSize[] = {hbins, sbins};
            // hue varies from 0 to 179, see cvtColor
            float hranges[] = { 0, 180 };
            // saturation varies from 0 (black-gray-white) to
            // 255 (pure spectrum color)
            float sranges[] = { 0, 256 };
            const float* ranges[] = { hranges, sranges };
            
            // we compute the histogram from the 0-th and 1-st channels
            int channels[] = {0, 1};

            calcHist( &hsv, 1, channels, Mat(), // do not use mask
             roiHist, 2, histSize, ranges,
             true, // the histogram is uniform
             false );
            
            
            normalize(roiHist, roiHist,0,255,NORM_MINMAX, -1, Mat() );
            if(posNeg == "+")
                posHistograms.push_back(roiHist);
            else
                negHistograms.push_back(roiHist);
                
        }  
        Mat removeBackground(Mat image) {
            int discValue = 10;
            int thresholdValue = 1;
            Mat hsvt;
            cvtColor(image, hsvt, CV_BGR2HSV);
            MatND roiHist;
            MatND backproj;
            Mat disc;
            Mat thresh;
            Mat mats[3];
            Mat mask;
            float hue_range[] = { 0, 180 };
            float sat_range[] = { 0, 256 };
            const float* ranges[] = { hue_range, sat_range };
            const int channels[] = {0, 1};
            
            for(int i=0; i<posHistograms.size(); i++) {
                roiHist = posHistograms[i];
                
                calcBackProject( &hsvt, 1, channels, roiHist, backproj, ranges, 1, true );
                //imshow("dst", backproj);
                
                disc = getStructuringElement(MORPH_ELLIPSE,Size(discValue,discValue));
                filter2D(backproj, backproj, -1,disc);
                
                threshold(backproj, thresh, thresholdValue, 255, THRESH_BINARY);
                mats[0] = thresh;
                mats[1] = thresh;
                mats[2] = thresh;
                
                merge(mats, 3, mask);
                bitwise_and(image,mask,image);
            }
            for(int i=0; i<negHistograms.size(); i++) {
                roiHist = negHistograms[i];
                
                calcBackProject( &hsvt, 1, channels, roiHist, backproj, ranges, 1, true );
                //imshow("dst", backproj);
                
                disc = getStructuringElement(MORPH_ELLIPSE,Size(discValue,discValue));
                filter2D(backproj, backproj, -1,disc);
                
                threshold(backproj, thresh, thresholdValue, 255, THRESH_BINARY_INV);
                mats[0] = thresh;
                mats[1] = thresh;
                mats[2] = thresh;
                
                merge(mats, 3, mask);
                bitwise_and(image,mask,image);
            }
            
            //if(showDiffImg)
                //imshow("diffImg", image);
                
            return image;
        }
    private:
        vector<MatND> negHistograms;
        vector<MatND> posHistograms;
};
    
    
