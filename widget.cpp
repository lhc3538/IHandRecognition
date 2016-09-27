//test v8.0
#include "widget.h"
#include "ui_widget.h"

using namespace std;
using namespace cv;

CvVideoWriter* videowr;
bool record;
uchar* qImageBuffer;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    client=new QTcpSocket;
    qImageBuffer=(uchar *)(malloc(640 * 480 *4 *sizeof(uchar)));

    ui->Image->setStyleSheet("border-width: 1px;border-style: solid;border-color: rgb(0, 0, 110);");
    ui->statuslabel->setStyleSheet("border-width: 1px;border-style: solid;border-color: rgb(0, 0, 110);");

    connect(client,SIGNAL(connected()),this,SLOT(sendrequest()));
    connect(client,SIGNAL(readyRead()),this,SLOT(read()));
    connect(client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(socketerror()));
}

Widget::~Widget()
{
    delete ui;
    delete client;
    free(qImageBuffer);
}

void Widget::on_connect_clicked()
{
    ui->statuslabel->setText(tr("connecting..."));
    client->connectToHost(ui->lineEdit->text(),ui->port->text().toInt());
}

void Widget::on_stop_clicked()
{
    client->abort();
    block1.clear();
    ui->statuslabel->setText(tr("stoped"));
}

void Widget::on_record_clicked()
{
    CvSize size;
    size.width=640;
    size.height=480;
    QTime time;
    time=time.currentTime();//获得当前时间，如:08:37:34
    QString Tstr=time.toString()+".avi";//转换成QString类型，如08:37:34.avi
    Tstr=Tstr.replace(QChar(':'),QLatin1String("-"));//将:换成-，因为windows下文件名不能含:
    const char* filen=Tstr.toStdString().data();//转换成const char*型的文件名
    videowr=cvCreateVideoWriter(filen,CV_FOURCC('M','J','P','G'),20,size,1);
    record=true;
}

void Widget::on_stoprecord_clicked()
{
    record=false;
    cvReleaseVideoWriter(&videowr);
}

void Widget::sendrequest()
{
    QByteArray block("GET /?action=stream\n\n");
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    client->write(block);
}

void Widget::socketerror()
{
    ui->statuslabel->setText(tr("socket error!"));
}

void Widget::read()
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
                //***put time on the IplImage***/
                QTime timer;
                timer=timer.currentTime();
                QString str=timer.toString();
                QByteArray ba1=str.toLatin1();
                const char* str2=ba1.data();
                //Mat mat(img);
                //putText(mat,str2,Point(0,30),3,1,Scalar(255,0,0),1,8,false);
                //IplImage temp = &mat.operator IplImage();
                //img=&temp;

                //***IplImage --- QImage and display
//                cvNamedWindow("Image",1);
//                //显示图像
//                cvShowImage("Image",pImg);

                QImage* qImage=new QImage(qImageBuffer, img->width, img->height, QImage::Format_RGB32);
                IplImageToQImage(img,qImage,qImageBuffer);
                QPixmap pix;
                pix= pix.fromImage(*qImage);
                ui->Image->setPixmap(pix);
                delete qImage;
                //***record the IplImage***/
                if(record==true)
                    cvWriteFrame(videowr,img);//录制视频

                cvReleaseImage(&img);//与cvCreateImage配对使用
                ba.clear();
            }
            block1.remove(0,EOIPos+1);
        }
    }
    mutex.unlock();
}

void Widget::on_help_clicked()
{
    qDebug("help...\n");
}
