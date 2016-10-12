#ifndef FACEDETECT_H
#define FACEDETECT_H
#include "opencv2/objdetect.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/core/utility.hpp"

#include "opencv2/videoio/videoio_c.h"
#include "opencv2/highgui/highgui_c.h"

#include <cctype>
#include <iostream>
#include <iterator>
#include <stdio.h>
using namespace std;
using namespace cv;
class FaceDetect
{
private:
    string cascadeName;
    CascadeClassifier cascade;
    CascadeClassifier nestedCascade;
    double scale;
    bool tryflip;

public:
    FaceDetect();
    void detectAndDraw( Mat& img );
};

#endif // FACEDETECT_H
