
#include "motionTracker.h"
#include "backgroundExtractor.h"

class MotionTrackerBack: public MotionTracker
{
    public:
        
        MotionTrackerBack(VideoCapture _cap, int _threshold=15, int _perfection=100, bool _showTracker=false, bool _showDiffImg=false, bool _showBackImg=true):MotionTracker(_cap, _showTracker, _showDiffImg) {
            
            perfection = _perfection;
            be = BackgroundExtractor(_threshold, perfection, _showBackImg);
        }
        
        void update(Mat _frame) {
            frame = _frame.clone();
            blur(frame, frame, Point(blurValue, blurValue));
            
            backImg = be.loop(frame);           // get latest background image from background extractor
            absdiff(backImg, frame, diffImg);   // find out how much pixels are changed
        }
    
    
        /*def setParameter(self, parameter, value, value2=None):        
            if parameter == 'show_back_image':
                self.backExtr.showBackImage = value
                if value :
                    self.backExtr.createTrackbars()
            else :
                super(self.__class__, self).setParameter(parameter, value, value2)
        */    
        
    private:
        int perfection;
        BackgroundExtractor be;
        Mat backImg;
        
};
    
    
