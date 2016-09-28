#ifndef GESTURESTRUCT_H
#define GESTURESTRUCT_H

#include <QFileDialog>
#include <QMessageBox>
#include <QString>

#include <time.h>
#include <ctime>

#include <iostream>
#include <opencv/cv.h>
#include <opencv/cvaux.h>
#include <opencv/cvwimage.h>
#include <opencv/cxcore.h>

#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>

using namespace std;
using namespace cv;

const int FeatureNum=5;//��������������Ȧ��

#define LEFT    0x01
#define RIGHT   0x02
#define COMEON  0x04
#define GO      0x8

class MyPoint
{
private:
    int m_x,m_y;
public:
    MyPoint(int x,int y)
    {
        m_x = x;
        m_y = y;
    }
    int X(){return m_x;}
    int Y(){return m_y;}
};

typedef struct _MyRect
{
    int m_x,m_y;
    int m_width,m_height;

    void initRect(int x,int y,int width,int height)
    {
        m_x = x;m_y = y;m_width = width;m_height = height;
    }

    void initPointRect (MyPoint top_left,MyPoint right_bottom)
    {
        m_x = top_left.X ();
        m_y = top_left.Y ();
        m_width = right_bottom.X ()-top_left.X ();
        m_height = right_bottom.Y ()-top_left.Y ();
    }

    void SetRectEmpty()
    {
        m_x = 0,m_y = 0;
        m_width = 0,m_height = 0;
    }

    bool IsRectEmpty()
    {
        if(m_width == 0 || m_height == 0)
            return true;
        return false;
    }

    MyPoint TopLeft()
    {
        MyPoint temp(m_x,m_y);
        return temp;
    }

    MyPoint BottomRight()
    {
        MyPoint temp(m_x+m_width,m_y+m_height);
        return temp;
    }

    int vertex_X(){return m_x;}
    int vertex_Y(){return m_y;}
    int Width(){return m_width;}
    int Height(){return m_height;}
}MyRect;

struct GestureStruct//����һ�����ƽṹ
{
    float angle[FeatureNum][10];//��ɫ�Ƕ�
    float anglecha[FeatureNum][10];//�Ƿ�ɫ�Ƕ�
    float count[FeatureNum];//�Ƕȷ�ɫ�ĸ���
    string GestureName;//��������
    string angleName;//�Ƕȵ����֣�����д��yml�ļ���Ϊһ���ڵ㣬����ʶ���ʱ��ͨ��ʶ������ȡ����
    string anglechaName;//�Ƕȵ����֣�����д��yml�ļ���Ϊһ���ڵ㣬����ʶ���ʱ��ͨ��ʶ������ȡ����
    string countName;//�Ƕȵĸ������������Ϊ���ٸ���ָ����ʵ���ϲ�ֻ������),����д��yml�ļ���Ϊһ���ڵ㣬����ʶ���ʱ��ͨ��ʶ������ȡ����
};

#endif
