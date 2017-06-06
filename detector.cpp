#include "detector.h"

Detector::Detector(std::string fileName, QObject *parent) : QObject(parent)
{
    capture=new VideoCapture(fileName);
    (*capture)>>frame1;
}
Detector::Detector(int device, QObject *parent) : QObject(parent)
{
    capture=new VideoCapture(device);
    (*capture)>>frame1;
}


int Detector::searchTarget(Mat lableImg,
                           std::vector<cv::Vec<double, 3> > &target,//引用传出目标连通域
                           double roundToleranceCoeff,
                           int minArea,
                           int maxArea)
{


        //返回值含义：-1 函数输入参数错误； 1 没有连通域；0 正常返回
        //输入标记好的连通域矩阵，最小面积，最大面积和圆度容忍系数(0为标准的圆，越大越不像圆)，输出目标的位置和半径。

       // std::string stringTemp="";//缓存，用于push到displayInfo中
        displayInfo.clear();//类的全局变量，用于存储目标信息

        target.clear();
        if (lableImg.empty())
            //如果输入标记矩阵为空
        {

            displayInfo.push_back("ERROR searchTarget!!");

            //std::cout<<"ERROR searchTarget!!"<<std::endl;
            return -1;
        }



        double min,max;
        cv::minMaxIdx(lableImg,&min,&max);
        if(max==0)//如果没有连通域标记，函数结束
            {return 1;}

        //初始化向量容器
        std::vector<TargetTemplate> targetInfo(int(max+1));//前2个空着，从下标2开始对应


        displayInfo.push_back("The size of connected domain is:"+std::to_string(targetInfo.size()-2)+"\n");

        //std::cout<<"the size of vector is:"<<targetInfo.size()-2<<std::endl;



        int rows=lableImg.rows;
        int cols=lableImg.cols;

        /*坐标系说明
            0----j----->
            |
            |
            i
            |
        */

        for (int i=0;i<rows;i++)
        {
            int *data=lableImg.ptr<int> (i);
            for(int j=0;j<cols;j++)
            {
                if(data[j]<2)//如果不是连通域像素，则继续,否则应该是标记为data[j]的连通域
                    continue;
                targetInfo[data[j]].area+=1;//面积
                targetInfo[data[j]].posxSum+=j;//重心累加（x轴）
                targetInfo[data[j]].posySum+=i;
                if(j<targetInfo[data[j]].horiMin)
                    targetInfo[data[j]].horiMin=j;
                if(j>targetInfo[data[j]].horiMax)
                    targetInfo[data[j]].horiMax=j;

                if(i<targetInfo[data[j]].vertMin)
                    targetInfo[data[j]].vertMin=i;
                if(i>targetInfo[data[j]].vertMax)
                    targetInfo[data[j]].vertMax=i;




            }
        }

    std::vector<TargetTemplate> ::iterator itc= targetInfo.begin();
     itc+=2;
     int count=1;
     while (itc!=targetInfo.end() )
      {   displayInfo.push_back("预设的圆度是："+std::to_string(roundToleranceCoeff)+"\n");
          //std::cout<<"预设的圆度是："<<roundToleranceCoeff<<std::endl;
          int width,height;
          width  =( itc->horiMax - itc->horiMin);//这两个参数实际上表示了目标的“包围盒”
          height =( itc->vertMax - itc->vertMin);

          if(  isTrueTarget(width, height, itc->area, minArea, maxArea,roundToleranceCoeff) )
          {
              cv::Vec<double,3> targetStore;
              targetStore[0]=itc->posxSum/ (double)itc->area;
              targetStore[1]=itc->posySum/ (double)itc->area;
              targetStore[2]=sqrt(itc->area/3);
              target.push_back(targetStore);

              /*打印相关信息*/
              displayInfo.push_back("targetInfo NO."+std::to_string(count)+"\n");
              //std::cout<<"targetInfo NO."<<count<<std::endl;
              displayInfo.push_back("W:"+std::to_string(width)+" H:"+std::to_string(height)+"  Area:"+std::to_string(itc->area)+"\n");
              //std::cout<<"w "<<width<<"  h "<<height<<"  Area "<<itc->area<<std::endl;
              count++;
              displayInfo.push_back("X"+std::to_string(itc->posxSum/itc->area)+" ,Y"+std::to_string(itc->posySum/itc->area)+"\n");
              double roundDescrib=fabs((float)(width-height) /(width+height) )+fabs( (width*height)/(float)itc->area/(4/3.14159)-1);
              displayInfo.push_back("roundnessCoeff is: "+std::to_string(roundDescrib)+"\n");
              //std::cout<<"roundnessCoeff is: "<<roundDescrib<<std::endl;

          }



          itc++;
      }


     return 0;
 }



 void Detector::icvprCcaBySeedFill(const Mat &_binImg, Mat &_lableImg)
 {
     // connected component analysis (4-component)
         // use seed filling algorithm
         // 1. begin with a foreground pixel and push its foreground neighbors into a stack;
         // 2. pop the top pixel on the stack and label it with the same label until the stack is empty
         //
         // foreground pixel: _binImg(x,y) = 1
         // background pixel: _binImg(x,y) = 0


         if (_binImg.empty() ||
             _binImg.type() != CV_8UC1)
         {
             return ;
         }

         _lableImg.release() ;
         _binImg.convertTo(_lableImg, CV_32SC1) ;

         int label = 1 ;  // start by 2

         int rows = _binImg.rows - 1 ;
         int cols = _binImg.cols - 1 ;
         for (int i = 1; i < rows-1; i++)
         {
             int* data= _lableImg.ptr<int>(i) ;
             for (int j = 1; j < cols-1; j++)
             {
                 if (data[j] == 1)
                 {
                     std::stack<std::pair<int,int> > neighborPixels ;
                     neighborPixels.push(std::pair<int,int>(i,j)) ;     // pixel position: <i,j>
                     label++ ;  // begin with a new label
                     while (!neighborPixels.empty())
                     {
                         // get the top pixel on the stack and label it with the same label
                         std::pair<int,int> curPixel = neighborPixels.top() ;
                         int curX = curPixel.first ;
                         int curY = curPixel.second ;
                         _lableImg.at<int>(curX, curY) = label ;

                         // pop the top pixel
                         neighborPixels.pop() ;

                         // push the 4-neighbors (foreground pixels)
                         if (curY!=0  && _lableImg.at<int>(curX, curY-1) == 1)
                         {// left pixel
                             neighborPixels.push(std::pair<int,int>(curX, curY-1)) ;
                         }
                         if (curY!=cols &&_lableImg.at<int>(curX, curY+1) == 1)
                         {// right pixel
                             neighborPixels.push(std::pair<int,int>(curX, curY+1)) ;
                         }
                         if (curX!=0&& _lableImg.at<int>(curX-1, curY) == 1)
                         {// up pixel
                             neighborPixels.push(std::pair<int,int>(curX-1, curY)) ;
                         }
                         if (curX!=rows && _lableImg.at<int>(curX+1, curY) == 1)
                         {// down pixel
                             neighborPixels.push(std::pair<int,int>(curX+1, curY)) ;
                         }
                     }
                 }
             }
         }
 }


 int Detector::isTrueTarget(int width, int height, int area, const int minArea, const int maxArea, const double roundCoeff)
 {
     if (area<minArea || area>maxArea)
     {  displayInfo.push_back("Area NOT meet!\n");
        //std::cout<<"面积不符\n";
         return 0;
     }
     double roundDescrib=fabs((float)(width-height) /(width+height) )+fabs( (width*height)/(float)area/(4/3.14159)-1);
     //std::cout<<"roundnessCoeff is: "<<roundDescrib<<std::endl;
     if (roundDescrib>roundCoeff)
     {   displayInfo.push_back("Roundness NOT meet! \n");
         //std::cout<<"圆度不符\n";
         return 0;
     }

     return 1;
 }



 void Detector::process(QLabel & label)
 {
         frame2=frame1.clone();
         (*capture)>>frame1;
         frame1.copyTo(frameShow);
         GaussianBlur(frame1,frame1,Size(5,5),1); //高斯滤波
         if( frame1.empty())//视频读完后正常退出，防止异常结束
         {
            // cout<<"处理完毕"<<endl;
             emit fileEnd();
             return;
         }

         absdiff(frame1,frame2,frameTemp);//帧差后取绝对值
         split(frameTemp,frameSingleCh);//多通道分裂

         minMaxIdx(frameSingleCh[0],& minVal,& maxVal);//查找亮度最大值
         maxVal= maxVal>80? maxVal:80;//限制最小的二值化阀值，避免在无目标时无下限的阀值
         //cout<<maxVal;
         threshold(frameSingleCh[0],frameSingleCh[2],0.8*maxVal,1,THRESH_BINARY);//使用先前的最大亮度动态阀值二值化,前景记为1.

         //形态学滤波
         Mat element(5,5,CV_8U,Scalar(1));
         morphologyEx(frameSingleCh[2],frameSingleCh[0],MORPH_CLOSE,element);
         //namedWindow("Gauss",CV_WINDOW_AUTOSIZE);
         //imshow("Gauss",  frame1);



         icvprCcaBySeedFill(frameSingleCh[0],lableImg);

         //icvprLabelColor( lableImg,   frameShow);
         //lableImg.convertTo(lableImg, CV_8UC1);
         //minMaxIdx(lableImg,& minVal,& maxVal);
        // cout<<minVal<<"  "<<maxVal<<endl;

             //imwrite("..//capture//fffff.jpg",lableImg);
             //return 0;


         //使用自定义的函数进行连通域搜索
         std::vector<Vec<double,3> > circles;
         searchTarget(lableImg,
                      circles,
                      roundTolerance,//圆度容忍度，类的public全局变量，下2个参数也是
                      minArea,
                      maxArea);

         vector<Vec<double,3> >::const_iterator itc= circles.begin();
         while (itc!=circles.end()) {//将检测到的目标用圆标出

               cv::circle(frameShow,
               Point((*itc)[0], (*itc)[1]), // circle centre
               (*itc)[2]+3, // circle radius
               cv::Scalar(0,0,256), // color
               2); // thickness

               ++itc;
             }

         putText( frameShow, std::to_string(FrmNum) , Point(20,40), FONT_HERSHEY_DUPLEX,0.5, Scalar( 255, 255, 0 ) );
        // namedWindow("origion" );
        // imshow("origion",  frameShow);
         //namedWindow("processed");
         //imshow("processed", frameSingleCh[0]*50 );
         cvtColor(frameShow,frameShow,CV_BGR2RGB);
         QImage image1 = QImage((unsigned char*)(frameShow.data), frameShow.cols, frameShow.rows, QImage::Format_RGB888);
         label.setPixmap(QPixmap::fromImage(image1));
         label.resize(image1.size());
         label.show();

         FrmNum++;

}

/*
 void Detector::setSource(std::string fileName)
{
     capture=new VideoCapture(fileName);
     (*capture)>>frame1;
}

 void Detector::setSource(int device)
{
     capture=new VideoCapture(device);
     (*capture)>>frame1;
}
*/

 void Detector::outPutTargetInfo(QTextBrowser & textBrowser)
 {
     if(!displayInfo.empty())
         textBrowser.setPlainText("");

     for (int i = 0; i<this->displayInfo.size(); i++)
     {
         textBrowser.append(QString::fromStdString(this->displayInfo[i]) );
     }
    // textBrowser.append(" 1\n");
     //textBrowser.append(" 2\n");
 }
