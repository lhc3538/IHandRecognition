#include "handgesturedialog.h"
#include "ui_handgesturedialog.h"
#include <QProgressDialog>

using namespace std;
using namespace cv;

CvVideoWriter* videowr;
bool record;
uchar* qImageBuffer;

HandGestureDialog::HandGestureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HandGestureDialog)
{
    ui->setupUi(this);

    frame = NULL;
    isRun = 1;
    time_intervals = 100;
    timer = new QTimer(this);
    imag = new QImage();
    gesturecount = 0;
    status_switch = Nothing;
    pt = NULL;
    r = 0;
    afterSkin = NULL;
    connect(timer, SIGNAL(timeout()), this, SLOT(readFarme()));
    ui->pushButton_CloseCamera->setDisabled (true);
    ui->pushButton_ShowPause->setDisabled (true);
    ui->pushButton_SnapImage->setDisabled (true);

    client=new QTcpSocket;
    qImageBuffer=(uchar *)(malloc(320 * 240 *4 *sizeof(uchar)));
    connect(client,SIGNAL(connected()),this,SLOT(sendrequest()));
    connect(client,SIGNAL(readyRead()),this,SLOT(read()));
    connect(client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketerror()));

}

HandGestureDialog::~HandGestureDialog()
{
    delete ui;
    delete client;
    free(qImageBuffer);
}

void HandGestureDialog::StartRecongizeHand (IplImage *img)
{
    // Create a string that contains the exact cascade name
    // Contains the trained classifer for detecting hand
    const char *cascade_name="hand.xml";
    // Create memory for calculations
    static CvMemStorage* storage = 0;
    // Create a new Haar classifier
    static CvHaarClassifierCascade* cascade = 0;
    // Sets the scale with which the rectangle is drawn with
    int scale = 1;
    // Create two points to represent the hand locations
    CvPoint pt1, pt2;
    // Looping variable
    int i;
    // Load the HaarClassifierCascade
    cascade = (CvHaarClassifierCascade*)cvLoad( cascade_name, 0, 0, 0 );
    // Check whether the cascade has loaded successfully. Else report and error and quit
    if( !cascade )
    {
        fprintf( stderr, "ERROR: Could not load classifier cascade\n" );
        return;
    }

    // Allocate the memory storage
    storage = cvCreateMemStorage(0);

    // Create a new named window with title: result
    cvNamedWindow( "result", 1 );

    // Clear the memory storage which was used before
    cvClearMemStorage( storage );

    // Find whether the cascade is loaded, to find the hands. If yes, then:
    if( cascade )
    {
        // There can be more than one hand in an image. So create a growable sequence of hands.
        // Detect the objects and store them in the sequence
        CvSeq* hands = cvHaarDetectObjects( img, cascade, storage,
                                            1.1, 2, CV_HAAR_DO_CANNY_PRUNING,
                                            cvSize(40, 40) );

        // Loop the number of hands found.
        for( i = 0; i < (hands ? hands->total : 0); i++ )
        {
            // Create a new rectangle for drawing the hand
            CvRect* r = (CvRect*)cvGetSeqElem( hands, i );

            // Find the dimensions of the hand,and scale it if necessary
            pt1.x = r->x*scale;
            pt2.x = (r->x+r->width)*scale;
            pt1.y = r->y*scale;
            pt2.y = (r->y+r->height)*scale;

            // Draw the rectangle in the input image
            cvRectangle( img, pt1, pt2, CV_RGB(230,20,232), 3, 8, 0 );
        }
    }

    // Show the image in the window named "result"
    cvShowImage( "result", img );
    cvWaitKey (30);
}

//void HandGestureDialog::readFarme()
//{
//    frame = cvQueryFrame(cam);
//    QImage image((const uchar*)frame->imageData,
//                 frame->width,
//                 frame->height,
//                 QImage::Format_RGB888);
//    image = image.rgbSwapped();
//    image = image.scaled(320,240);
//    ui->label_CameraShow->setPixmap(QPixmap::fromImage(image));
//    gesture.SkinDetect (frame,afterSkin);

//    /*next to opencv*/

//    if(status_switch == Recongnise)
//    {
//        // Flips the frame into mirror image
//        cvFlip(frame,frame,1);

//        // Call the function to detect and draw the hand positions
//        StartRecongizeHand(frame);
//    }
//}

void HandGestureDialog::on_pushButton_OpenCamera_clicked()
{
//    cam = cvCreateCameraCapture(0);
//    timer->start(time_intervals);
//    frame = cvQueryFrame(cam);
    client->connectToHost("192.168.0.60",8080);

    ui->pushButton_OpenCamera->setDisabled (true);
    ui->pushButton_CloseCamera->setEnabled (true);
    ui->pushButton_ShowPause->setEnabled (true);
    ui->pushButton_SnapImage->setEnabled (true);
    afterSkin = cvCreateImage (cvSize(320,240),IPL_DEPTH_8U,1);
}

void HandGestureDialog::on_pushButton_SnapImage_clicked()
{
    if(frame != NULL)
    {
        QImage image((const uchar*)frame->imageData,
                     frame->width,
                     frame->height,
                     QImage::Format_RGB888);
        image = image.rgbSwapped();
        image = image.scaled(160,120);
        ui->label_ShowSnap->setPixmap(QPixmap::fromImage(image));
    }
}

void HandGestureDialog::on_pushButton_CloseCamera_clicked()
{
//    timer->stop();
//    cvReleaseCapture(&cam);
//    cam = NULL;
    client->abort();
    block1.clear();

    ui->pushButton_OpenCamera->setEnabled (true);
    ui->pushButton_CloseCamera->setDisabled (true);
    ui->pushButton_ShowPause->setDisabled (true);
    ui->pushButton_SnapImage->setDisabled (true);
}

void HandGestureDialog::on_pushButton_ShowPause_clicked()
{
    if(isRun == 1)
    {
        timer->stop();
        ui->pushButton_ShowPause->setText (tr("继续拍摄"));
    }
    else
    {
        timer->start(time_intervals);
        ui->pushButton_ShowPause->setText (tr("暂停"));
    }
    isRun *= (-1);
}

void HandGestureDialog::on_pushButton_StartTrain_clicked()
{
    QProgressDialog* process = new QProgressDialog(this);
    process->setWindowTitle ("Traning Model");
    process->setLabelText("Processing...");
    process->setModal(true);
    process->show ();
    gesture.setMainUIPointer (this);
    gesture.Train(process);
    QMessageBox::about (this,tr("完成"),tr("手势训练模型完成"));
}

void HandGestureDialog::on_pushButton_StartRecongnise_clicked()
{
//    if(cam==NULL)
//    {
//        QMessageBox::warning (this,tr("Warning"),tr("Please Check Camera !"));
//        return;
//    }

    status_switch = Nothing;

    status_switch = Recongnise;
}

void HandGestureDialog::on_comboBox_ShowDelay_activated(int index)
{
    /*from ZERO*/
    time_intervals = index*100;
    if(timer == NULL)
        return ;
    if(timer->isActive ())
    {
        timer->stop ();
        timer->start (time_intervals);
    }
}

void HandGestureDialog::on_comboBox_ImageMode_activated(int index)
{
    /*some kind image to convert*/
}

void HandGestureDialog::sendrequest()
{
    QByteArray block("GET /?action=stream\n\n");
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    client->write(block);
}

void HandGestureDialog::socketerror()
{
    //ui->statuslabel->setText(tr("socket error!"));
    cout << "socket error!" << endl;
}

void HandGestureDialog::read()
{
    if(client->bytesAvailable()<3000)
        return;

    QByteArray tmpBlock;
    static const unsigned char SOIData[]={0xff,0xd8};
    static const unsigned char EOIData[]={0xff,0xd9};

    QByteArray SOIstr=QByteArray::fromRawData((char*)SOIData,sizeof(SOIData));
    QByteArray EOIstr=QByteArray::fromRawData((char*)EOIData,sizeof(EOIData));

    int SOIPos=0;
    int EOIPos=0;

    mutex.lock();
    tmpBlock=client->readAll();
    block1.append(tmpBlock);//保存实际读到的数据

    if((SOIPos=block1.indexOf(SOIstr))!= -1)
    {
        if((EOIPos=block1.indexOf(EOIstr))!= -1)
        {
            EOIPos += 2;
            if(EOIPos>SOIPos)
            {
                QByteArray ba;
                ba=block1.mid(SOIPos,EOIPos-SOIPos);

                QImage image;
                image.loadFromData(ba);
                //***QImage --- IplImage*******/
                CvSize size;
                size.width=image.width();
                size.height=image.height();
                IplImage* img=cvCreateImage(size,IPL_DEPTH_8U, 3);//与cvReleaseImage配对使用
                QImageToIplImage(&image,img);

                //***send img to hand gesture***/
                QImage image2((const uchar*)img->imageData,
                             img->width,
                             img->height,
                             QImage::Format_RGB888);
                image2 = image2.rgbSwapped();
                image2 = image2.scaled(320,240);
                ui->label_CameraShow->setPixmap(QPixmap::fromImage(image2));
                gesture.SkinDetect (img,afterSkin);

                /*next to opencv*/

                if(status_switch == Recongnise)
                {
                    // Flips the frame into mirror image
                    cvFlip(img,img,1);

                    // Call the function to detect and draw the hand positions
                    StartRecongizeHand(img);
                }

                cvReleaseImage(&img);//与cvCreateImage配对使用
                ba.clear();
            }
            block1.remove(0,EOIPos+1);
        }
    }
    mutex.unlock();
}
