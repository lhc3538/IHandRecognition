#ifndef HANDGESTUREDIALOG_H
#define HANDGESTUREDIALOG_H

#include <iostream>
#include <QTimer>
#include <QtNetwork/QTcpSocket>
#include <QString>
#include <QMutex>
#include "cv.h"
#include "highgui.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/ml/ml.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include <QImage>
#include "IplImageToQImage.h"

#include "SRC/GestureStruct.h"
#include "SRC/AIGesture.h"
#include "SRC/GestrueInfo.h"

using namespace std;
using namespace cv;

namespace Ui {
class HandGestureDialog;
}

class HandGestureDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit HandGestureDialog(QWidget *parent = 0);
    ~HandGestureDialog();
    void StartRecongizeHand(IplImage* img);

private slots:
//    void readFarme();
    void on_pushButton_OpenCamera_clicked();
    void on_pushButton_SnapImage_clicked();
    void on_pushButton_CloseCamera_clicked();
    void on_pushButton_ShowPause_clicked();
    void on_pushButton_StartTrain_clicked();
    void on_pushButton_StartRecongnise_clicked();
    void on_comboBox_ShowDelay_activated(int index);
    void on_comboBox_ImageMode_activated(int index);

    void sendrequest();
    void read();
    void socketerror();

private:
    Ui::HandGestureDialog *ui;
    CAIGesture gesture;
    int isRun;
    /**CAMERA PARAMETER**/
    IplImage  *frame;
    IplImage* afterSkin;
    QTimer    *timer;
    int time_intervals;
    QImage    *imag;

    /**gesture Recongnisize**/
    string gesturename;
    CvRect track_window;
    CvBox2D track_box;
    CvMemStorage* storage;
    CvSeq* pt_seq;
    string result[8];
    int gesturecount;
    CvPoint2D32f center;//用来储存手势的质心
    float r;//手势的半径
    CvPoint* pt;
    string com_result;
    MyRect rect;
    IplImage* src_tmp[8];
    enum Switch {Nothing,Recongnise,CommandGesture,Locate}status_switch;

    QTcpSocket *client;
    QByteArray block1;
    QMutex mutex;
};

#endif // HANDGESTUREDIALOG_H
