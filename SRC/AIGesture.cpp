#include "AIGesture.h"
#include "GestrueInfo.h"
#include <QList>
#include <QTime>
#include <math.h>
#include <cmath>


CAIGesture::CAIGesture()
{
    pMainUI = NULL;
}

CAIGesture::~CAIGesture()
{

}

void CAIGesture::setMainUIPointer (HandGestureDialog *pDlg)
{
    pMainUI = pDlg;
}

void CAIGesture::ColorRegulate(IplImage* src,IplImage* dst)
{
    IplImage* R=cvCreateImage(cvGetSize(src),8,1);
    IplImage* G=cvCreateImage(cvGetSize(src),8,1);
    IplImage* B=cvCreateImage(cvGetSize(src),8,1);

    cvSplit(src,R,G,B,0);

    CvScalar aveR=cvAvg(R,NULL);
    CvScalar aveG=cvAvg(G,NULL);
    CvScalar aveB=cvAvg(B,NULL);

    float aGray=(float)(aveR.val[0]+aveG.val[0]+aveB.val[0])/3;
    float aR=(float)(aGray/aveR.val[0]);
    float aG=(float)(aGray/aveG.val[0]);
    float aB=(float)(aGray/aveB.val[0]);

    int width=src->width;
    int height=src->height;
    int stepR=R->widthStep/sizeof(uchar);
    int stepG=G->widthStep/sizeof(uchar);
    int stepB=B->widthStep/sizeof(uchar);
    uchar* dataR=(uchar*)R->imageData;
    uchar* dataG=(uchar*)G->imageData;
    uchar* dataB=(uchar*)B->imageData;

    for(int i=0;i<height;i++)
    {
        for(int j=0;j<width;j++)
        {
            dataR[i*stepR+j]=(unsigned char)(aR*dataR[i*stepR+j]);
            dataG[i*stepG+j]=(unsigned char)(aG*dataG[i*stepG+j]);
            dataB[i*stepB+j]=(unsigned char)(aB*dataB[i*stepB+j]);
        }
    }

    cvMerge(R,G,B,0,dst);

    cvReleaseImage(&R);
    cvReleaseImage(&G);
    cvReleaseImage(&B);
}

//Histgrom Normalization ,src is Input Image,dst is Output Image
void CAIGesture::EqualImage(IplImage* src,IplImage* dst)
{
    IplImage* gray=cvCreateImage(cvGetSize(src),8,1);
    cvCvtColor(src,gray,CV_BGR2GRAY);
    cvSaveImage("gray.jpg",gray);
    cvEqualizeHist(gray,gray);
    cvSaveImage("gray2.jpg",gray);
    cvCvtColor(gray,dst,CV_GRAY2BGR);//opencv�޷���ͼ������תΪRGBģʽ
    cvReleaseImage(&gray);
}

void CAIGesture::SkinDetect(IplImage* src,IplImage* dst)
{
    IplImage* hsv = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);//use to split to HSV
    IplImage* tmpH1 = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1);//Use To Skin Detect
    IplImage* tmpS1 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage* tmpH2 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage* tmpS3 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage* tmpH3 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage* tmpS2 = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage* H = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage* S = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage* V = cvCreateImage( cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage* src_tmp1=cvCreateImage(cvGetSize(src),8,3);

    cvSmooth(src,src_tmp1,CV_GAUSSIAN,3,3);	//Gaussian Blur
    cvCvtColor(src_tmp1, hsv, CV_BGR2HSV );//Color Space to Convert
    cvSplit(hsv,H,S,V,0);//To Split 3 channel

    /*********************Skin Detect**************/
    cvInRangeS(H,cvScalar(0.0,0.0,0,0),cvScalar(20.0,0.0,0,0),tmpH1);
    cvInRangeS(S,cvScalar(75.0,0.0,0,0),cvScalar(200.0,0.0,0,0),tmpS1);
    cvAnd(tmpH1,tmpS1,tmpH1,0);

    // Red Hue with Low Saturation
    // Hue 0 to 26 degree and Sat 20 to 90
    cvInRangeS(H,cvScalar(0.0,0.0,0,0),cvScalar(13.0,0.0,0,0),tmpH2);
    cvInRangeS(S,cvScalar(20.0,0.0,0,0),cvScalar(90.0,0.0,0,0),tmpS2);
    cvAnd(tmpH2,tmpS2,tmpH2,0);

    // Red Hue to Pink with Low Saturation
    // Hue 340 to 360 degree and Sat 15 to 90
    cvInRangeS(H,cvScalar(170.0,0.0,0,0),cvScalar(180.0,0.0,0,0),tmpH3);
    cvInRangeS(S,cvScalar(15.0,0.0,0,0),cvScalar(90.,0.0,0,0),tmpS3);
    cvAnd(tmpH3,tmpS3,tmpH3,0);

    // Combine the Hue and Sat detections
    cvOr(tmpH3,tmpH2,tmpH2,0);
    cvOr(tmpH1,tmpH2,tmpH1,0);

    cvCopy(tmpH1,dst);

    cvReleaseImage(&hsv);
    cvReleaseImage(&tmpH1);
    cvReleaseImage(&tmpS1);
    cvReleaseImage(&tmpH2);
    cvReleaseImage(&tmpS2);
    cvReleaseImage(&tmpH3);
    cvReleaseImage(&tmpS3);
    cvReleaseImage(&H);
    cvReleaseImage(&S);
    cvReleaseImage(&V);
    cvReleaseImage(&src_tmp1);
}

//To Find The biggest Countour
void CAIGesture::FindBigContour(IplImage* src,CvSeq* (&contour),CvMemStorage* storage)
{
    CvSeq* contour_tmp,*contourPos;
    int contourcount=cvFindContours(src, storage, &contour_tmp, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
    if(contourcount==0)
        return;
    CvRect bndRect = cvRect(0,0,0,0);
    double contourArea,maxcontArea=0;
    for( ; contour_tmp != 0; contour_tmp = contour_tmp->h_next )
    {
        bndRect = cvBoundingRect( contour_tmp, 0 );
        contourArea=bndRect.width*bndRect.height;
        if(contourArea>=maxcontArea)//find Biggest Countour
        {
            maxcontArea=contourArea;
            contourPos=contour_tmp;
        }
    }
    contour=contourPos;
}

//Calculate The Center
void CAIGesture::ComputeCenter(CvSeq* (&contour),CvPoint& center,float& radius)
{
    CvMoments m;
    double M00,X,Y;
    cvMoments(contour,&m,0);
    M00=cvGetSpatialMoment(&m,0,0);
    X=cvGetSpatialMoment(&m,1,0)/M00;
    Y=cvGetSpatialMoment(&m,0,1)/M00;

    center.x=(int)X;
    center.y=(int)Y;

    /*******************tO find radius**********************/
    int hullcount;
    CvSeq* hull;
    CvPoint pt;
    double tmpr1,r=0;
    hull=cvConvexHull2(contour,0,CV_COUNTER_CLOCKWISE,0);
    hullcount=hull->total;
    for(int i=1;i<hullcount;i++)
    {
        pt=**CV_GET_SEQ_ELEM(CvPoint*,hull,i);//get each point
        tmpr1=sqrt((double)((center.x-pt.x)*(center.x-pt.x))+(double)((center.y-pt.y)*(center.y-pt.y)));//���������ĵ�Ĵ�С
        if(tmpr1>r)//as the max radius
            r=tmpr1;
    }
    radius=r;
}

void CAIGesture::GetFeature(IplImage* src,CvPoint& center,float radius,
                            float angle[FeatureNum][10],
float anglecha[FeatureNum][10],
float count[FeatureNum])
{
    int width=src->width;
    int height=src->height;
    int step=src->widthStep/sizeof(uchar);
    uchar* data=(uchar*)src->imageData;

    float R=0.0;
    int a1,b1,x1,y1,a2,b2,x2,y2;//the distance of the center to other point
    float angle1_tmp[200]={0},angle2_tmp[200]={0},angle1[50]={0},angle2[50]={0};//temp instance to calculate angule
    int angle1_tmp_count=0,angle2_tmp_count=0,angle1count=0,angle2count=0,anglecount=0;

    for(int i=0;i<FeatureNum;i++)//��FeatureNum�����������ȡ��Ҳ����5�㣩����
    {
        R=(i+4)*radius/9;
        for(int j=0;j<=3600;j++)
        {
            if(j<=900)
            {
                a1=(int)(R*sin(j*3.14/1800));//���Ҫ�Լ�ʵ�ʻ�һ��ͼ��������
                b1=(int)(R*cos(j*3.14/1800));
                x1=center.x-b1;
                y1=center.y-a1;
                a2=(int)(R*sin((j+1)*3.14/1800));
                b2=(int)(R*cos((j+1)*3.14/1800));
                x2=center.x-b2;
                y2=center.y-a2;
            }
            else
            {
                if(j>900&&j<=1800)
                {
                    a1=(int)(R*sin((j-900)*3.14/1800));
                    b1=(int)(R*cos((j-900)*3.14/1800));
                    x1=center.x+a1;
                    y1=center.y-b1;
                    a2=(int)(R*sin((j+1-900)*3.14/1800));
                    b2=(int)(R*cos((j+1-900)*3.14/1800));
                    x2=center.x+a2;
                    y2=center.y-b2;
                }
                else
                {
                    if(j>1800&&j<2700)
                    {
                        a1=(int)(R*sin((j-1800)*3.14/1800));
                        b1=(int)(R*cos((j-1800)*3.14/1800));
                        x1=center.x+b1;
                        y1=center.y+a1;
                        a2=(int)(R*sin((j+1-1800)*3.14/1800));
                        b2=(int)(R*cos((j+1-1800)*3.14/1800));
                        x2=center.x+b2;
                        y2=center.y+a2;
                    }
                    else
                    {
                        a1=(int)(R*sin((j-2700)*3.14/1800));
                        b1=(int)(R*cos((j-2700)*3.14/1800));
                        x1=center.x-a1;
                        y1=center.y+b1;
                        a2=(int)(R*sin((j+1-2700)*3.14/1800));
                        b2=(int)(R*cos((j+1-2700)*3.14/1800));
                        x2=center.x-a2;
                        y2=center.y+b2;
                    }
                }
            }

            if(x1>0&&x1<width&&x2>0&&x2<width&&y1>0&&y1<height&&y2>0&&y2<height)
            {
                if((int)data[y1*step+x1]==255&&(int)data[y2*step+x2]==0)
                {
                    angle1_tmp[angle1_tmp_count]=(float)(j*0.1);//�ӷ�ɫ���Ƿ�ɫ�ĽǶ�
                    angle1_tmp_count++;
                }
                else if((int)data[y1*step+x1]==0&&(int)data[y2*step+x2]==255)
                {
                    angle2_tmp[angle2_tmp_count]=(float)(j*0.1);//�ӷǷ�ɫ����ɫ�ĽǶ�
                    angle2_tmp_count++;
                }
            }
        }
        int j=0;
        for(j=0;j<angle1_tmp_count;j++)
        {
            if(angle1_tmp[j]-angle1_tmp[j-1]<0.2)//����̫С�ĽǶ�
                continue;
            angle1[angle1count]=angle1_tmp[j];
            angle1count++;
        }

        for(j=0;j<angle2_tmp_count;j++)
        {
            if(angle2_tmp[j]-angle2_tmp[j-1]<0.2)
                continue;
            angle2[angle2count]=angle2_tmp[j];
            angle2count++;
        }

        for(j=0;j<max(angle1count,angle2count);j++)
        {
            if(angle1[0]>angle2[0])
            {
                if(angle1[j]-angle2[j]<7)//����С��7�ȵĽǶȣ���Ϊ�˵���ָһ�㶼�������ֵ
                    continue;
                angle[i][anglecount]=(float)((angle1[j]-angle2[j])*0.01);//��ɫ�ĽǶ�
                anglecha[i][anglecount]=(float)((angle2[j+1]-angle1[j])*0.01);//�Ƿ�ɫ�ĽǶȣ�������ָ��ĽǶ�
                anglecount++;
            }
            else
            {
                if(angle1[j+1]-angle2[j]<7)
                    continue;
                anglecount++;
                angle[i][anglecount]=(float)((angle1[j+1]-angle2[j])*0.01);
                anglecha[i][anglecount]=(float)((angle2[j]-angle1[j])*0.01);
            }
        }

        if(angle1[0]<angle2[0])
            angle[i][0]=(float)((angle1[0]+360-angle2[angle2count-1])*0.01);
        else
            anglecha[i][0]=(float)((angle2[0]+360-angle1[angle1count-1])*0.01);

        count[i]=(float)anglecount;
        angle1_tmp_count=0,angle2_tmp_count=0,angle1count=0,angle2count=0,anglecount=0;
        for(j=0;j<200;j++)
        {
            angle1_tmp[j]=0;
            angle2_tmp[j]=0;
        }
        for(j=0;j<50;j++)
        {
            angle1[j]=0;
            angle2[j]=0;
        }
    }
}

void CAIGesture::OneGestureTrain(QDir GestureDir,CvFileStorage *fs,GestureStruct gesture)//�Ե���ͼƬ����ѵ��
{	
    IplImage* TrainImage=0;
    IplImage* dst=0;
    CvSeq* contour=NULL;
    CvMemStorage* storage;
    storage = cvCreateMemStorage(0);
    CvPoint center=cvPoint(0,0);
    float radius=0.0;
    float angle[FeatureNum][10]={0},anglecha[FeatureNum][10]={0},anglesum[FeatureNum][10]={0},anglechasum[FeatureNum][10]={0};
    float count[FeatureNum]={0},countsum[FeatureNum]={0};

    int FileCount=0;
    /*��ȡ��Ŀ¼�µ�����jpg�ļ�*/
    QFileInfoList list = GestureDir.entryInfoList();
    QString currentDirPath = GestureDir.absolutePath ();
    currentDirPath += "/";
    for(int k=2;k<list.size ();k++)
    {
        QFileInfo tempInfo = list.at (k);
        if(tempInfo.isFile () == true)
        {
            QString fileNamePath = currentDirPath + tempInfo.fileName ();
            TrainImage=cvLoadImage(fileNamePath.toStdString ().c_str(),1);
            if(TrainImage==NULL)
            {
                cout << "can't load image" << endl;
                cvReleaseMemStorage(&storage);
                cvReleaseImage(&dst);
                cvReleaseImage(&TrainImage);
                return;
            }
            if(dst==NULL&&TrainImage!=NULL)
                dst=cvCreateImage(cvGetSize(TrainImage),8,1);
            SkinDetect(TrainImage,dst);
            FindBigContour(dst,contour,storage);
            cvZero(dst);
            cvDrawContours( dst, contour, CV_RGB(255,255,255),CV_RGB(255,255,255), -1, -1, 8 );
            ComputeCenter(contour,center,radius);

            GetFeature(dst,center,radius,angle,anglecha,count);
            for(int j=0;j<FeatureNum;j++)
            {
                countsum[j]+=count[j];
                for(int k=0;k<10;k++)
                {
                    anglesum[j][k]+=angle[j][k];
                    anglechasum[j][k]+=anglecha[j][k];
                }
            }
            FileCount++;
            cvReleaseImage(&TrainImage);
        }
    }
    for(int i=0;i<FeatureNum;i++)
    {
        gesture.count[i]=countsum[i]/FileCount;
        for(int j=0;j<10;j++)
        {
            gesture.angle[i][j]=anglesum[i][j]/FileCount;
            gesture.anglecha[i][j]=anglechasum[i][j]/FileCount;
        }
    }
    cvStartWriteStruct(fs,gesture.angleName.c_str (),CV_NODE_SEQ,NULL);//��ʼд��yml�ļ�

    int i=0;
    for(i=0;i<FeatureNum;i++)
        cvWriteRawData(fs,&gesture.angle[i][0],10,"f");//д���ɫ�Ƕȵ�ֵ

    cvEndWriteStruct(fs);
    cvStartWriteStruct(fs,gesture.anglechaName.c_str (),CV_NODE_SEQ,NULL);

    for(i=0;i<FeatureNum;i++)
        cvWriteRawData(fs,&gesture.anglecha[i][0],10,"f");//д��Ƿ�ɫ�Ƕȵ�ֵ

    cvEndWriteStruct(fs);
    cvStartWriteStruct(fs,gesture.countName.c_str (),CV_NODE_SEQ,NULL);
    cvWriteRawData(fs,&gesture.count[0],FeatureNum,"f");//д���ɫ�Ƕȵĸ���
    cvEndWriteStruct(fs);

    cvReleaseMemStorage(&storage);
    cvReleaseImage(&dst);
}

void CAIGesture::Train(QProgressDialog *pBar)//��ָ��ѵ���ļ���������������ƽ���ѵ��
{
    QString curStr = QDir::currentPath ();
    QString fp1 = "InfoDoc/gestureFeatureFile.yml";
    fp1 = curStr + "/" + fp1;
    CvFileStorage *GestureFeature=cvOpenFileStorage(fp1.toStdString ().c_str (),0,CV_STORAGE_WRITE);
    FILE* fp;
    QString fp2 = "InfoDoc/gestureFile.txt";
    fp2 = curStr + "/" + fp2;
    fp=fopen(fp2.toStdString ().c_str (),"w");
    int FolderCount=0;

    /*��ȡ��ǰ��Ŀ¼��Ȼ��õ���ǰ����Ŀ¼*/
    QString trainStr = curStr;
    trainStr += "/TraningSample/";
    QDir trainDir(trainStr);
    GestureStruct gesture;
    QFileInfoList list = trainDir.entryInfoList();

    pBar->setRange(0,list.size ()-2);


    for(int i=2;i<list.size ();i++)
    {
        pBar->setValue(i-1);

        QFileInfo fileInfo = list.at (i);
        if(fileInfo.isDir () == true)
        {
            FolderCount++;

            QString tempStr = fileInfo.fileName ();
            fprintf(fp,"%s\n",tempStr.toStdString ().c_str ());
            gesture.angleName = tempStr.toStdString ()+"angleName";
            gesture.anglechaName = tempStr.toStdString ()+"anglechaName";
            gesture.countName = tempStr.toStdString ()+"anglecountName";

            tempStr = trainStr + tempStr + "/";
            QDir subDir(tempStr);
            OneGestureTrain(subDir,GestureFeature,gesture);
        }
    }
    pBar->autoClose ();
    delete pBar;
    pBar = NULL;
    fprintf(fp,"%s%d","Hand Gesture Number: ",FolderCount);
    fclose(fp);
    cvReleaseFileStorage(&GestureFeature);
}

//���沿���Ǿ�̬ͼƬ��ʶ��
void CAIGesture::Recognise(IplImage* src, string &result)
{
    QString curStr = QDir::currentPath ();
    FILE* fp;
    QString f1 = "InfoDoc/gestureFile.txt";
    f1 = curStr+"/"+f1;
    fp=fopen(f1.toStdString ().c_str (),"r");
    fseek(fp,-2,2);
    int GestureNum=0;
    fscanf(fp,"%d",&GestureNum);
    if(GestureNum==0)
    {
        fseek(fp,-1,2);
        fscanf(fp,"%d",&GestureNum);
    }
    rewind(fp);
    char* buff=new char[20];
    string* GestureName=new string[GestureNum];
    int i=0;
    for(i=0;i<GestureNum;i++)
    {
        fscanf(fp,"%s\n",buff);
        GestureName[i]=buff;
    }
    delete buff;
    fclose(fp);
    float angle[FeatureNum][10]={0},anglecha[FeatureNum][10]={0};//������ȡ��ɫɫ�ȵ��м����
    float Sbangle[FeatureNum][10]={0},Sbanglecha[FeatureNum][10]={0};//������ȡ�Ƿ�ɫɫ�ȵ��м����
    float angleresult[FeatureNum]={0};//������ȡ������м����
    float count[FeatureNum]={0};
    float Sbcount[FeatureNum]={0};
    int mask[10]={0},maskcount=0,mask1[10]={0},maskcount1=0;
    double mask_tmp=0,mask_tmp1=0;
    CvSeq* contour=NULL;
    CvMemStorage* storage;
    storage = cvCreateMemStorage(0);
    CvPoint center=cvPoint(0,0);
    float radius=0.0;
    IplImage* dst=cvCreateImage(cvGetSize(src),8,1);
    SkinDetect(src,dst);
    FindBigContour(dst,contour,storage);
    CvRect bndRect=cvBoundingRect( contour, 0 );
    cvZero(dst);
    cvDrawContours( dst, contour, CV_RGB(255,255,255),CV_RGB(255,255,255), -1, -1, 8 );
    ComputeCenter(contour,center,radius);
    cvRectangle(src,cvPoint(bndRect.x,bndRect.y),
                cvPoint(bndRect.x+bndRect.width,bndRect.y+bndRect.height),
                CV_RGB(244,0,0));
    GetFeature(dst,center,radius,angle,anglecha,count);
    QString f2 = "infoDoc/gestureFeatureFile.yml";
    f2 = curStr+"/"+f2;
    CvFileStorage *fs=cvOpenFileStorage(f2.toStdString ().c_str (),0,CV_STORAGE_READ);
    CvFileNode* filenode;
    for( i=0;i<GestureNum;i++)
    {
        string m_tmp = GestureName[i]+"anglecountName";
        filenode=cvGetFileNodeByName(fs,NULL,m_tmp.c_str ());
        cvReadRawData(fs,filenode,Sbcount,"f");//��ȡ���ݵ�Sbcount���������

        for(int j=0;j<FeatureNum;j++)
            mask_tmp+=fabs(Sbcount[j]-count[j])*fabs(Sbcount[j]-count[j]);//�ȶԷ�ɫ�Ƕȵĸ�������ƥ��

        if(mask_tmp<5)
        {
            mask[maskcount]=i;
            maskcount++;
        }
        mask_tmp=0;
    }

    for(i=0;i<maskcount;i++)
    {
        string m_tmp = GestureName[mask[i]]+"angleName";
        filenode=cvGetFileNodeByName(fs,NULL,m_tmp.c_str ());
        cvReadRawData(fs,filenode,Sbangle,"f");
        for(int j=0;j<FeatureNum;j++)
            for(int k=0;k<10;k++)
                mask_tmp1+=fabs(Sbangle[j][k]-angle[j][k])*fabs(Sbangle[j][k]-angle[j][k]);//�Է�ɫ�ǶȽ���ƥ��
        if(mask_tmp1<10)
        {
            mask1[maskcount1]=mask[i];
            maskcount1++;
        }
        mask_tmp1=0;
    }
    for(i=0;i<maskcount1;i++)
    {
        string m_tmp = GestureName[mask1[i]]+"anglechaName";
        filenode=cvGetFileNodeByName(fs,NULL,m_tmp.c_str ());
        cvReadRawData(fs,filenode,Sbanglecha,"f");
        for(int j=0;j<FeatureNum;j++)
            for(int k=0;k<10;k++)
                angleresult[i]+=(float)(fabs(Sbanglecha[j][k]-anglecha[j][k])*fabs(Sbanglecha[j][k]-anglecha[j][k]));//�ԷǷ�ɫ�ǶȽ���ƥ��
    }
    CvFont font;
    double hScale=0.7;
    double vScale=0.7;
    int lineWidth=1;
    cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX|CV_FONT_ITALIC, hScale,vScale,0,lineWidth);//��ʼ�����壬׼��д��ͼƬ�ϵ�
    if(maskcount==0||maskcount1==0)
    {
        cvReleaseFileStorage(&fs);
        return;
    }
    float angletmp=angleresult[0];
    int angletmp1=0;
    for(i=0;i<maskcount1;i++)
    {
        if(angleresult[i]<=angletmp)
        {
            angletmp=angleresult[i];
            angletmp1=mask1[i];
        }
    }
    result=GestureName[angletmp1];
    //cvPutText(src,GestureName[angletmp1].c_str (),cvPoint(20,20),&font,CV_RGB(255,0,0));//��ͼƬ�������ǰͼƬ���Ƶ�����
    cvReleaseFileStorage(&fs);
    cvReleaseImage(&dst);
    delete []GestureName;
}

//�������ƣ�CvBox2D����ṹ������һ���Ƕȱ����������Ժ���õ�
void CAIGesture::Follow(IplImage* src,CvRect& track_window,CvBox2D &track_box)
{
    CvConnectedComp track_comp;

    IplImage* hsv=0;
    IplImage* h=0;
    IplImage* s=0;
    IplImage* v=0;
    IplImage* mask=0;
    IplImage* mask_tmp1=0;
    IplImage* mask_tmp2=0;
    IplImage* mask_tmp3=0;
    IplImage* mask_tmp4=0;
    IplImage* backproject=0;

    int hdims = 150;  // ����HIST�ĸ�����Խ��Խ��ȷ
    float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;
    CvHistogram *hist = 0;

    hsv=cvCreateImage(cvGetSize(src),8,3);
    h=cvCreateImage(cvGetSize(src),8,1);
    s=cvCreateImage(cvGetSize(src),8,1);
    v=cvCreateImage(cvGetSize(src),8,1);

    mask=cvCreateImage(cvGetSize(src),8,1);//����ͼ��
    mask_tmp1=cvCreateImage(cvGetSize(src),8,1);
    mask_tmp2=cvCreateImage(cvGetSize(src),8,1);
    mask_tmp3=cvCreateImage(cvGetSize(src),8,1);
    mask_tmp4=cvCreateImage(cvGetSize(src),8,1);

    backproject=cvCreateImage(cvGetSize(src),8,1);
    hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );

    cvCvtColor(src,hsv,CV_BGR2HSV);
    cvSplit(hsv,h,s,v,0);

    cvInRangeS(h,cvScalar(0.0,0.0,0,0),cvScalar(12.0,0.0,0,0),mask_tmp1);
    cvInRangeS(s,cvScalar(75.0,0.0,0,0),cvScalar(200.0,0.0,0,0),mask_tmp2);
    cvAnd(mask_tmp1,mask_tmp2,mask_tmp1,0);

    cvInRangeS(h,cvScalar(0.0,0.0,0,0),cvScalar(13.0,0.0,0,0),mask_tmp2);
    cvInRangeS(s,cvScalar(20.0,0.0,0,0),cvScalar(90.0,0.0,0,0),mask_tmp3);
    cvAnd(mask_tmp2,mask_tmp3,mask_tmp2,0);

    cvInRangeS(h,cvScalar(170.0,0.0,0,0),cvScalar(180.0,0.0,0,0),mask_tmp3);
    cvInRangeS(s,cvScalar(15.0,0.0,0,0),cvScalar(90.,0.0,0,0),mask_tmp4);
    cvAnd(mask_tmp3,mask_tmp4,mask_tmp3,0);

    cvOr(mask_tmp3,mask_tmp2,mask_tmp2,0);
    cvOr(mask_tmp2,mask_tmp1,mask,0);

    cvCalcHist(&h,hist,0,mask);

    cvCalcBackProject(&h,backproject,hist);
    cvAnd(backproject,mask,backproject,0);



    cvCamShift(backproject,track_window,
               cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
               &track_comp, &track_box );

    //cvSetImageROI(mask,track_window);
    if(cvCountNonZero(mask)<40)
    {
        cout << "Trace Failed !" << endl;
        cvReleaseImage(&hsv);
        cvReleaseImage(&h);
        cvReleaseImage(&s);
        cvReleaseImage(&v);
        cvReleaseImage(&mask);
        cvReleaseImage(&mask_tmp1);
        cvReleaseImage(&mask_tmp2);
        cvReleaseImage(&mask_tmp3);
        cvReleaseImage(&mask_tmp4);
        cvReleaseImage(&backproject);
        cvReleaseHist(&hist);
        return;
    }
    cvResetImageROI(mask);

    track_window=track_comp.rect;

    cvRectangle(src,
                cvPoint(track_window.x,track_window.y),
                cvPoint(track_window.x+track_window.width,track_window.y+track_window.height),
                CV_RGB(255,255,0));

    cvReleaseImage(&hsv);
    cvReleaseImage(&h);
    cvReleaseImage(&s);
    cvReleaseImage(&v);
    cvReleaseImage(&mask);
    cvReleaseImage(&mask_tmp1);
    cvReleaseImage(&mask_tmp2);
    cvReleaseImage(&mask_tmp3);
    cvReleaseImage(&mask_tmp4);
    cvReleaseImage(&backproject);
    cvReleaseHist(&hist);
}

void CAIGesture::RecogniseSeq(CvPoint pt[8], float width, float height, string &result)
{
    float x_min_tmp=(float)(pt[0].x);//�˶��켣x�������Сֵ
    float x_max_tmp=(float)(pt[0].x);//�˶��켣x��������ֵ
    float y_min_tmp=(float)(pt[0].y);//�˶��켣y�������Сֵ
    float y_max_tmp=(float)(pt[0].y);//�˶��켣y��������ֵ
    for(int i=0;i<8;i++)
    {
        if(pt[i].x<x_min_tmp)
            x_min_tmp=(float)(pt[i].x);
        if(pt[i].x>x_max_tmp)
            x_max_tmp=(float)(pt[i].x);
        if(pt[i].y<y_min_tmp)
            y_min_tmp=(float)(pt[i].y);
        if(pt[i].y>y_max_tmp)
            y_max_tmp=(float)(pt[i].y);
    }
    if(x_max_tmp-x_min_tmp>2*width&&y_max_tmp-y_min_tmp<height)
    {
        if(pt[7].x-pt[0].x>0)
        {
            result="����";
        }
        else
        {
            result="����";
        }
    }
    else if(x_max_tmp-x_min_tmp<width&&y_max_tmp-y_min_tmp>height)
    {
        if(pt[7].y-pt[0].y>0)
        {
            result="����";
        }
        else
        {
            result="����";
        }
    }
}

void CAIGesture::SaveImage(IplImage* src)
{
    /*����ͼƬ������ʱ������Ϊ�ļ��Ĵ洢��*/
    QTime t = QTime::currentTime();
    QString imgname = t.toString();
    imgname.append (".jpg");
    cvSaveImage(imgname.toStdString ().c_str (),src);
}

void CAIGesture::RecogniseResult(string str[], string &result)
{
    string tmp;
    int count=0;
    for(int i=0;i<8;i++)
    {
        tmp=str[i];
        for(int j=0;j<8;j++)
        {
            if(tmp==str[j])
                count++;
            if(count>6)
            {
                result=str[j];
                break;
            }
        }
        if(count>6)
            break;
        count=0;
    }
}

/*����������Ϊ���ã�Ŀǰ��û�еõ�Ӧ��*/
void CAIGesture::SaveForTrain(IplImage* src, CvPoint pt[8], string &result)//����һ��������Ϊѵ���ã�Ŀǰ��û���ϣ������Ժ�����ϣ�
{
    IplImage* tmp=cvCreateImage(cvGetSize(src),8,1);
    cvZero(tmp);
    for(int i=1;i<8;i++)
    {
        cvCircle(tmp,pt[i-1],10,CV_RGB(255,255,255),3,8,0);
        cvLine(tmp,pt[i-1],pt[i],CV_RGB(255,255,255),3,8,0);
    }
    cvCircle(tmp,pt[7],10,CV_RGB(255,255,255),3,8,0);
    QTime t = QTime::currentTime();
    QString imgname = t.toString();
    imgname.append (".jpg");
    cvSaveImage(imgname.toStdString ().c_str (),tmp);
}

void CAIGesture::Follow(IplImage* src, CvRect& track_window, CvHistogram* &hist, bool &Flag)//��ָ����ֱ��ͼ��������
{
    CvConnectedComp track_comp;
    CvBox2D track_box;

    IplImage* hsv=0;//HSVͼ��ռ�
    IplImage* h=0;//HSVͼ��ռ��е�H����
    IplImage* s=0;
    IplImage* v=0;
    IplImage* mask=0;
    IplImage* mask_tmp1=0;
    IplImage* mask_tmp2=0;
    IplImage* mask_tmp3=0;
    IplImage* mask_tmp4=0;
    IplImage* backproject=0;

    hsv=cvCreateImage(cvGetSize(src),8,3);
    h=cvCreateImage(cvGetSize(src),8,1);
    s=cvCreateImage(cvGetSize(src),8,1);
    v=cvCreateImage(cvGetSize(src),8,1);
    mask=cvCreateImage(cvGetSize(src),8,1);//����ͼ��
    mask_tmp1=cvCreateImage(cvGetSize(src),8,1);
    mask_tmp2=cvCreateImage(cvGetSize(src),8,1);
    mask_tmp3=cvCreateImage(cvGetSize(src),8,1);
    mask_tmp4=cvCreateImage(cvGetSize(src),8,1);
    backproject=cvCreateImage(cvGetSize(src),8,1);

    cvCvtColor(src,hsv,CV_BGR2HSV);
    cvSplit(hsv,h,s,v,0);

    cvInRangeS(h,cvScalar(0.0,0.0,0,0),cvScalar(12.0,0.0,0,0),mask_tmp1);
    cvInRangeS(s,cvScalar(75.0,0.0,0,0),cvScalar(200.0,0.0,0,0),mask_tmp2);
    cvAnd(mask_tmp1,mask_tmp2,mask_tmp1,0);

    cvInRangeS(h,cvScalar(0.0,0.0,0,0),cvScalar(13.0,0.0,0,0),mask_tmp2);
    cvInRangeS(s,cvScalar(20.0,0.0,0,0),cvScalar(90.0,0.0,0,0),mask_tmp3);
    cvAnd(mask_tmp2,mask_tmp3,mask_tmp2,0);

    cvInRangeS(h,cvScalar(170.0,0.0,0,0),cvScalar(180.0,0.0,0,0),mask_tmp3);
    cvInRangeS(s,cvScalar(15.0,0.0,0,0),cvScalar(90.,0.0,0,0),mask_tmp4);
    cvAnd(mask_tmp3,mask_tmp4,mask_tmp3,0);

    cvOr(mask_tmp3,mask_tmp2,mask_tmp2,0);
    cvOr(mask_tmp2,mask_tmp1,mask,0);

    if(Flag)//��FlagΪTRUE�Ļ����ȸ��ٺ���ֱ��ͼ�����ֱ��ͼ����һ�����ٵ�ֱ��ͼ
    {
        cvAnd(backproject,mask,backproject,0);
        cvCalcBackProject(&h,backproject,hist);
    }
    else//��FlagΪFALSE�Ļ�������ֱ��ͼ��ֱ���˳����൱��һ������Ϊ��ʼ���õģ������ٶ���
    {
        cvCalcBackProject(&h,backproject,hist);
        cvAnd(backproject,mask,backproject,0);
        Flag=true;
        cvReleaseImage(&hsv);
        cvReleaseImage(&h);
        cvReleaseImage(&s);
        cvReleaseImage(&v);
        cvReleaseImage(&mask);
        cvReleaseImage(&mask_tmp1);
        cvReleaseImage(&mask_tmp2);
        cvReleaseImage(&mask_tmp3);
        cvReleaseImage(&mask_tmp4);
        cvReleaseImage(&backproject);
        return;
    }

    cvCamShift(backproject,track_window,
               cvTermCriteria( CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1 ),
               &track_comp, &track_box );
    track_window=track_comp.rect;
    if(cvCountNonZero(mask)<40)//��Ŀ�������Сʱ��Ϊ���ٶ�ʧ��
    {
        cout << "Target Trace Failed !" << endl;
        cvReleaseImage(&hsv);
        cvReleaseImage(&h);
        cvReleaseImage(&s);
        cvReleaseImage(&v);
        cvReleaseImage(&mask);
        cvReleaseImage(&mask_tmp1);
        cvReleaseImage(&mask_tmp2);
        cvReleaseImage(&mask_tmp3);
        cvReleaseImage(&mask_tmp4);
        cvReleaseImage(&backproject);
        track_window=cvRect(150,150,20,20);
        return;
    }

    cvReleaseImage(&hsv);
    cvReleaseImage(&h);
    cvReleaseImage(&s);
    cvReleaseImage(&v);
    cvReleaseImage(&mask);
    cvReleaseImage(&mask_tmp1);
    cvReleaseImage(&mask_tmp2);
    cvReleaseImage(&mask_tmp3);
    cvReleaseImage(&mask_tmp4);
    cvReleaseImage(&backproject);
}

void CAIGesture::CalcHist(IplImage* src,CvRect& rect,CvHistogram* &hist)//��һ��ָ����Rect��ֱ��ͼ
{
    IplImage* hsv=0;
    IplImage* h=0;
    IplImage* s=0;
    IplImage* v=0;

    int hdims = 150;
    float hranges_arr[] = {0,180};
    float* hranges = hranges_arr;

    hsv=cvCreateImage(cvGetSize(src),8,3);
    h=cvCreateImage(cvGetSize(src),8,1);
    s=cvCreateImage(cvGetSize(src),8,1);
    v=cvCreateImage(cvGetSize(src),8,1);

    hist = cvCreateHist( 1, &hdims, CV_HIST_ARRAY, &hranges, 1 );

    cvCvtColor(src,hsv,CV_BGR2HSV);
    cvSplit(hsv,h,s,v,0);

    cvSetImageROI( h, rect );
    cvCalcHist(&h,hist,0,NULL);
    cvResetImageROI(h);

    cvReleaseImage(&hsv);
    cvReleaseImage(&h);
    cvReleaseImage(&s);
    cvReleaseImage(&v);
}

void CAIGesture::Location(IplImage* src[8], MyRect &rect)
{
    IplImage* dst=cvCreateImage(cvGetSize(src[0]),8,1);
    CvSeq* contour_tmp;
    CvMemStorage* storage=cvCreateMemStorage(0);
    CvRect bndRect = cvRect(0,0,0,0);
    MyRect tmpRect;
    string result;
    QList<GestrueInfo> tmpList;
    QList<GestrueInfo>::iterator pos;
    GestrueInfo tmpinfo;
    for(int i=0;i<8;i++)
    {
        SkinDetect(src[i],dst);
        int contourcount=cvFindContours(dst, storage, &contour_tmp, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_NONE );
        if(contourcount==0)
            return;
        if(tmpList.isEmpty ())
        {
            for( ; contour_tmp != 0; contour_tmp = contour_tmp->h_next )
            {
                bndRect = cvBoundingRect( contour_tmp, 0 );
                if(bndRect.width>20&&bndRect.height>20)
                {
                    cvSetImageROI(src[i],bndRect);
                    Recognise(src[i],result);
                    if(result.empty ())
                        result="NULL";
                    cvResetImageROI(src[i]);
                    MyPoint p1 = MyPoint(bndRect.x,bndRect.y);
                    MyPoint p2 = MyPoint(bndRect.x+bndRect.width,bndRect.y+bndRect.height);
                    tmpRect.initPointRect(p1,p2);
                    tmpinfo.SetRect(tmpRect);
                    tmpinfo.AddGestureName(result);
                    tmpList.push_back (tmpinfo);
                }
            }
        }
        else
        {
            for( ; contour_tmp != 0; contour_tmp = contour_tmp->h_next )
            {
                pos=tmpList.begin ();
                bndRect = cvBoundingRect( contour_tmp, 0 );
                MyPoint p1 = MyPoint(bndRect.x,bndRect.y);
                MyPoint p2 = MyPoint(bndRect.x+bndRect.width,bndRect.y+bndRect.height);
                tmpRect.initPointRect(p1,p2);
                while(!tmpList.isEmpty ()&&pos!=tmpList.end ())
                {
                    if(bndRect.width>20&&bndRect.height>20)
                    {
                        MyRect posRect;
                        posRect.initPointRect((*pos).GetRect().TopLeft(),(*pos).GetRect().BottomRight());
                        if((*pos).IsTheSamePosition(posRect,tmpRect))
                        {
                            cvSetImageROI(src[i],bndRect);
                            Recognise(src[i],result);
                            if(result.empty ())
                                result="NULL";
                            cvResetImageROI(src[i]);
                            (*pos).AddGestureName(result);
                        }
                    }
                    pos++;
                }
            }
        }
    }
    pos=tmpList.begin();
    while(pos!=tmpList.end ())
    {
        string result;
        RecogniseResult((*pos).GetGestureName(),result);
        if(!result.empty ())
        {
            cout << result << endl;
            rect=(*pos).GetRect();
            break;
        }
        pos++;
    }
    if(result.empty ())
        rect.SetRectEmpty();
}
