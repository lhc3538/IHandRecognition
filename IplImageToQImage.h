//IplImageToQImage.h


#ifndef QTIPL_H
#define QTIPL_H

#include <iostream>
#include <cstring>
#include <qimage.h>
#include "cv.h"
#include <QVector>


using std::string;
using std::iostream;
//const int QImage_IgnoreEndian = 2;

void IplImageToQImage(const IplImage * iplImage,QImage *qimage,uchar *qImageBuffer,double mini=0.0, double maxi=0.0);
void QImageToIplImage(const QImage * qImage,IplImage *charIplImageBuffer);

#endif
/////////////////////////
