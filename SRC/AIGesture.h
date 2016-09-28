#ifndef AIGESTURE_H
#define AIGESTURE_H

#include "GestureStruct.h"
#include <QProgressDialog>

class HandGestureDialog;
class CAIGesture  
{
public:
	CAIGesture();
	virtual ~CAIGesture();

    void setMainUIPointer(HandGestureDialog* pDlg);

private:
    HandGestureDialog* pMainUI;

public:
	void SkinDetect(IplImage* src,IplImage* dst);
	//��ɫ���,����srcΪ����ͼ��dstΪ���ͼ��

	void FindBigContour(IplImage* src,CvSeq* (&contour),CvMemStorage* storage);
	//������������srcΪ����ͼ��contour���������,storageΪopencvһ���ڴ�ṹ

	void ComputeCenter(CvSeq* (&contour),CvPoint& center,float &radius);
	//����������Ƶ����ģ�contourΪ����������centerΪ������ģ�radiusΪ����뾶

	void GetFeature(IplImage* src,
		CvPoint& center,
		float radius,
		float angle[FeatureNum][10],
		float anglecha[FeatureNum][10],
		float count[FeatureNum]);
	//��ȡ���Ƶ�������srcΪ����ͼ��centerΪ�������ģ�radiusΪ����뾶��angle,anglecha,countΪ�������

    void OneGestureTrain(QDir GestureDir,CvFileStorage *fs,GestureStruct gesture);
	//�Ե���ͼ�����ѵ����GesturePathΪ��������ѵ���ļ��У�fsΪһ���ļ��洢����gesture��һ�����ƽṹ

    void Train(QProgressDialog* pBar);
	//ѵ��������FolderPathΪѵ���������ļ���·��

    void Recognise(IplImage* src,string& result);
	//ʶ��,srcΪ����ͼ��resultΪ������

	void Follow(IplImage* src,CvRect& track_window,CvBox2D &track_box);
	//���Ƹ���,srcΪ����ͼ��track_windowΪһ�����پ��Σ�track_boxΪһ�����������Ŀǰû���ϣ������Ժ������

    void RecogniseSeq(CvPoint pt[8],float width,float height,string &result);
	//��һ�����н���ʶ��pt[8]�������8���㣬resultΪ����Ľ����flagΪ�ж��Ƿ�Ϊ��һ�Σ���Ϊֻ������ʶ�����Ϊ������
	void ColorRegulate(IplImage* src,IplImage* dst);
	//������ɫУ��������Ч�������Ǻܺã�srcΪ����ͼ��dstΪ���ͼ��

	void EqualImage(IplImage* src,IplImage*dst);
	//ֱ��ͼ���⻯,srcΪ����ͼ��dstΪ���ͼ��

	void SaveImage(IplImage* src);
	//����ͼ������ѵ��

    void RecogniseResult(string str[8],string &result);
	//��ȡ��str[8]�г��ִ�������6���ַ������浽result��;

	/*����������Ϊ���ã�Ŀǰ��û�еõ�Ӧ��*/
	void CalcHist(IplImage* src,CvRect &rect,CvHistogram* (&hist));
	//����rect�ڵķ�ɫ���ֵ�ֱ��ͼ

    void Follow(IplImage* src,CvRect& track_window,CvHistogram* &hist,bool& Flag);
	//�����ƽ��и���

    void SaveForTrain(IplImage* src,CvPoint pt[8],string &result);
	//��һ��������н��б��棬����ѵ����������HMMģ����һ�£�������HMMģ�Ϳ���������Ӧ��ָ������

    void Location(IplImage* src[8],MyRect& rect);
};

#endif
