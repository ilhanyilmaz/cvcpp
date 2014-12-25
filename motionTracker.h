#include <stdio.h>
#include <opencv2/opencv.hpp>

//import locationEstimator as le
#define OP_OPEN 2
#define OP_CLOSE 3  

using namespace cv;

class MotionTracker {
    public:
        MotionTracker(VideoCapture _cap, bool _showTracker=false, bool _showDiffImg=false) {
            /*if not calibrationFile == None :
                self.estimator = le.LocationEstimator(calibrationFile)
            if self.showDiffImage:
                self.createDiffImageWindow()    */
            kernelOpen = Size(2,2);
            kernelClose = Size(5,12);
            blurValue = 2;
            thresholdLower = 15;
            thresholdHigher = 40;
            //showPositions=false;
            showTracker = _showTracker;
            showDiffImg = _showDiffImg;
            cap = _cap;
            namedWindow("contours", CV_WINDOW_AUTOSIZE );
            namedWindow("diff image", CV_WINDOW_AUTOSIZE );
            namedWindow("tracker", CV_WINDOW_AUTOSIZE );
        }
        
        void findMovingObjects() {
            
            Mat tDiffImg, tDiffImgHigh, tDiffImgLow;
            Mat element;
            
            if(!diffImg.data)
                return;
            
            threshold(diffImg, tDiffImgHigh, thresholdHigher, 128, CV_THRESH_BINARY);
            threshold(diffImg, tDiffImgLow, thresholdLower, 127, CV_THRESH_BINARY);
            add(tDiffImgHigh, tDiffImgLow, tDiffImg);
            
            element = getStructuringElement( MORPH_ELLIPSE, kernelOpen);
            
            erode(tDiffImg, tDiffImg, element);
            dilate(tDiffImg, tDiffImg, element);
            
            element = getStructuringElement( MORPH_RECT, kernelClose);
            morphologyEx( tDiffImg, tDiffImg, OP_CLOSE, element );
            
            if(showDiffImg)
                imshow("diff image", tDiffImg);
                
            Mat canny_output;

            /// Detect edges using canny
            Canny( tDiffImg, canny_output, 100, 200, 3 );
            /// Find contours
            findContours( canny_output, contours, RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

            rectangles.clear();
            vector<Rect>::iterator it;
            it = rectangles.begin();
            for( int i = 0; i< contours.size(); i++ )
            {
                if(contourArea(contours[i])<10)
                    continue;
                //drawContours( frame, contours, i, 255 );
                
                Rect rect = boundingRect(contours[i]);
                //Mat conDiffImg = tDiffImgHigh(rect);
                rect = findNonZeroBoundary(tDiffImgHigh,rect);
                if(rect.width == 0 || rect.height == 0)
                    continue;
                it = rectangles.insert(it, rect);
                
            }
            /// Show in a window 
            imshow("contours", frame);
            
        }
        Rect findNonZeroBoundary(Mat img, Rect rect) {
            //imshow("roi", roi_);
            int x = rect.x;
            int y = rect.y;
            int w = rect.width;
            int h = rect.height;
            bool lFound = false;
            bool rFound = false;
            bool uFound = false;
            bool bFound = false;
            int l,r,u,b; // left right up bottom
            l=x;
            r=x+w-1;
            u=y;
            b=y+h-1;
            
            //printf("%i-%i / %i-%i\n",x,y,w,h);
                
            for(int i=0; (!lFound || !rFound) && i<w; i++) {
                if(!lFound)
                    l=x+i;
                if(!rFound)
                    r=x+w-i-1;
                //printf("x: %i-%i\n",l,r);
                
                if(countNonZero(img(Rect(l,y,1,h)))>0)
                    lFound = true;
                if(countNonZero(img(Rect(r,y,1,h)))>0)
                    rFound = true;
            }
            
            
            for(int i=0; (!uFound || !bFound)&& i<h; i++) {
                if(!uFound)
                    u=y+i;
                if(!bFound)
                    b=y+h-i-1;
                //printf("y: %i-%i\n",u,b);
                if(countNonZero(img(Rect(x,u,w,1)))>0)
                    uFound = true;
                if(countNonZero(img(Rect(x,b,w,1)))>0)
                    bFound = true;
            }
            
            if(!lFound || !rFound || !uFound || !bFound)
                printf("just shadow\n");
            else if(l==r || b==u)
                printf("just a line\n");
            else
                return Rect(l,u,r-l,b-u);
            
            return Rect(0,0,0,0);
        }
        
        void drawRectangles() {
            Mat trackerImg = frame.clone();
            for(int i=0; i<rectangles.size();i++) {
                rectangle(trackerImg, Point(rectangles[i].x,rectangles[i].y), Point(rectangles[i].x+rectangles[i].width,rectangles[i].y+rectangles[i].height),255);
                //printf("%i-%i / %i-%i\n",rectangles[i].x,rectangles[i].y,rectangles[i].width,rectangles[i].height);
            }
            imshow("tracker", trackerImg);
        }
        
        
        int findBiggestMovingObjectPos() {
            int maxArea = 0;
            int area;
            int pos;
            int x,y;
            for(int i=0; i<rectangles.size(); i++) {
                area = rectangles[i].width*rectangles[i].height;
                if(area>maxArea) {
                    maxArea=area;
                    pos = i;
                }
            }
            if(rectangles.size()==0)
                return -1;
            return pos;
        }
        
        Point findObject2dBottom(int pos) {
            Rect obj = rectangles[pos];
            return Point(obj.x+obj.width/2,obj.y+obj.height);
        }
        
        Point findObject2dCenter(int pos) {
            Rect obj = rectangles[pos];
            return Point(obj.x+obj.width/2,obj.y+obj.height/2);
        }
        
        void update() {}

    protected:
        Size kernelOpen;
        Size kernelClose;
        int blurValue;
        int thresholdLower;
        int thresholdHigher;
        vector<vector<Point> > contours;
        //vector<Vec4i> hierarchy;
        Mat frame;
        Mat diffImg;
        VideoCapture cap;
        //bool showPositions;
        bool showTracker;
        bool showDiffImg;
        vector<Rect> rectangles;
        

};


    /*def getDilatedDiffImage(self, frame):

        threshold = cv2.morphologyEx(self.diffImage, cv2.MORPH_OPEN, self.KERNEL_OPEN)
        threshold = cv2.morphologyEx(threshold, cv2.MORPH_CLOSE, self.KERNEL_CLOSE)
        #threshold = cv2.erode(self.diffImage,self.KERNEL_OPEN,iterations = 1)
        #threshold = cv2.dilate(threshold,self.KERNEL_CLOSE,iterations = 1)
        return cv2.bitwise_and(frame, frame, mask = threshold)
		
    """def getObjectPositions(self):
        if self.contours == None :
            return
        posImage = np.zeros((500,500,3), np.uint8)
        #i=1
        for contour in self.contours:
            area = cv2.contourArea(contour)
            #print area
            if area < 10:
                continue
            x,y,w,h = cv2.boundingRect(contour)
            objPos = self.estimator.get3dCoordinates(x+w/2, y+h)
            cv2.circle(posImage, (int(objPos[0]),int(objPos[1])), 10, (0,255,0),-1)
            #if i==1 :
                #print 'obj{0}= {1}-{2}'.format(i, str(x+w/2), str(y+h))
                #print self.estimator.get3dCoordinates(x+w/2, y+h)
            #i+=1
        if self.showPositions:
            cv2.imshow("positions", posImage)
    """

    
    def drawRectangles(self):
        if self.rectangles == None:
            return self.frame
        for rect in self.rectangles:
            cv2.rectangle(self.frame, (rect[0],rect[1]), (rect[2],rect[3]), (0,255,0))
        if self.showTracker:
			cv2.imshow('tracker', self.frame)
        return self.frame
                
    def drawContours(self):
        if self.contours == None:
            return self.frame
        for contour in self.contours:
            area = cv2.contourArea(contour)
            #print area
            if area < 10:
                continue
            x,y,w,h = cv2.boundingRect(contour)
            cv2.rectangle(self.frame, (x,y), (x+w,y+h), (0,255,0))
            #cv2.drawContours(img,contours,-1,(0,255,0),3)
        if self.showTracker:
			cv2.imshow('tracker', self.frame)
        return self.frame
    
    def update2dPoints(self):
        if self.contours == None :
            return None
        
        self.estimator.getContours3dCoordinates(self.contours)
        
    def setParameter(self, parameter, value, value2=None):
        
        if parameter == 'blur':
            self.blurValue = value
        elif parameter == 'threshold':
            self.THRESHOLD = value
        elif parameter == 'threshold2':
            self.THRESHOLD2 = value
        elif parameter == 'm_open':
            self.KERNEL_OPEN = np.ones((value,value),np.uint8)
            #print self.KERNEL_OPEN
        elif parameter == 'm_close':
            self.KERNEL_CLOSE = np.ones((value2,value),np.uint8)
            #print self.KERNEL_CLOSE
        elif parameter == 'show_positions':
            self.showPositions = value
            self.estimator.showPositions = value
            self.estimator.initWindow()
        elif parameter == 'show_diff_image':
            self.showDiffImage = value
        elif parameter == 'show_tracker':
            self.showTracker = value
            
    def createDiffImageWindow(self):
    
        cv2.namedWindow('diff image')
        cv2.createTrackbar('blur','diff image',self.blurValue, 10, np.uint)
        cv2.createTrackbar('m_open','diff image',len(self.KERNEL_OPEN[:1]), 5, np.uint)
        j,i=self.KERNEL_CLOSE.shape
        cv2.createTrackbar('m_close_x','diff image',i, 20, np.uint)
        cv2.createTrackbar('m_close_y','diff image',j, 20, np.uint)
        cv2.createTrackbar('threshold','diff image',self.THRESHOLD, 100, np.uint)
        cv2.createTrackbar('threshold2','diff image',self.THRESHOLD2, 100, np.uint)

    def diffImageWindowSettings(self):
		
        if not self.showDiffImage:
			return
			
        value = cv2.getTrackbarPos('blur','diff image')
        if value == 0:
            value = 1
        if value != self.blurValue :
            self.setParameter('blur', value)
                                   
        value = cv2.getTrackbarPos('threshold','diff image')
        if value != self.THRESHOLD :
            self.setParameter('threshold', value)

        value = cv2.getTrackbarPos('threshold2','diff image')
        if value != self.THRESHOLD2 :
            self.setParameter('threshold2', value)
            
        j,i=self.KERNEL_OPEN.shape
        value = cv2.getTrackbarPos('m_open','diff image')
        if value == 0:
            value = 1
        if value != i :
            self.setParameter('m_open', value)
        
        value = cv2.getTrackbarPos('m_close_x','diff image')
        if value == 0:
            value = 1
        v2 = cv2.getTrackbarPos('m_close_y','diff image')
        if v2 == 0:
            v2 = 1
        self.setParameter('m_close', value, value2=v2)
        
        
    def checkSettings(self):
    
        if self.showDiffImage :
            self.diffImageWindowSettings()
    
    */
            

    
