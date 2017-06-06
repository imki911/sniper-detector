#ifndef DETECTOR_H
#define DETECTOR_H

#include <QObject>
#include <QImage>
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextBrowser>
//#include "ui_mainwindow.h"

//custom Include
//1.openCv Library
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

//2.Stand C++ Library
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <stack>
#include <cmath>
using namespace cv;
//using namespace std;
//QT_BEGIN_NAMESPACE

typedef struct
    {
        int horiMin=640,horiMax=0; //水平方向的最小最大坐标
        int vertMin=480,vertMax=0; //垂直方向的最小最大坐标
        int area=0;
        double posxSum = 0,posySum;//目标坐标的加权和
        //int hori,vert = 0;//目标在水平和垂直方向的长度
    }TargetTemplate;



class Detector : public QObject
{
    Q_OBJECT
private:

    int FrmNum=0;
    Mat frame1,frame2,frameTemp;
    Mat frameSingleCh[3];
    Mat frameShow;
    Mat lableImg;//连通域的标记矩阵
    std::vector<std::string> displayInfo;

    /*目标检测相关参数*/
    double maxVal = 80;//二值化前搜索到的最大亮度
    double minVal = 10;


    int isTrueTarget(int width,
                     int height,
                     int area,
                     const int minArea,
                     const int maxArea,
                     const double roundCoeff );

    void icvprCcaBySeedFill(const  Mat& _binImg,  Mat& _lableImg);

    int searchTarget(  Mat lableImg,
                    std::vector< Vec<double,3> > & target,

                    double roundToleranceCoeff=0.5 ,//圆度容忍系数(缺省值在声明中定义)
                    int minArea=10 ,//最小目标面积
                    int maxArea=200 );//最大目标面积



public:
    int minArea=10;
    int maxArea=200;
    double roundTolerance=0.5;

    explicit Detector(QObject *parent = 0);

    Detector(std::string fileName, QObject *parent=0) ;
    Detector(int device, QObject *parent=0) ;


    VideoCapture  * capture;//视频输入对象


    //void setSource(std::string fileName);
    // void setSource(int device);
    void process(QLabel &label);
    void outPutTargetInfo(QTextBrowser & textBrowser);

  signals:
    void fileEnd();

public slots:




};




#endif // DETECTOR_H
