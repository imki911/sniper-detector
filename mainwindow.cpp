#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <detector.h>

//#include <opencv/cv.h>
//#include <opencv2/opencv.hpp>
//#include<iostream>
//#include <QTimer>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    timer = new QTimer(this);

    ui->statusBar->showMessage("Initialization is completed. Ready!");
    ui->buttonStop->setEnabled(false);
    //dec->process(  *(ui->label)  );
    connect(timer, SIGNAL(timeout()), this, SLOT(timerSlot()) ) ;//信号槽的连接

//    QPalette background;
//        background.setBrush(QPalette::Background, QBrush(QPixmap("./img1.jpg")));
//        this->setPalette(background);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked() //摄像机按钮
{
    dec=new Detector(0);
    ui->statusBar->showMessage("From Camera, working...");
    ui->pushButton_2->setEnabled(false);
    ui->buttonStop->setEnabled(true);
    timer->start(40);


}

void MainWindow::on_actionExit_triggered()
{

    close();
}

void MainWindow::timerSlot()
{
    dec->process(* (ui->label));
    dec->outPutTargetInfo( * ui->textBrowser);

}

void MainWindow::on_buttonStop_clicked() //停止按钮
{
    timer->stop( );

    dec->capture->release();
    ui->label->clear();

    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->statusBar->showMessage("Stoped.");
}

void MainWindow::on_pushButton_2_clicked() //本地文件按钮
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr(" Open file"),//标题
                                                    "",//默认路径
                                                    tr("Vedio Files(*.avi *.mp4)"));//过滤器
    dec=new Detector(fileName.toStdString());
    connect(dec, SIGNAL(fileEnd()), this, SLOT(on_buttonStop_clicked() )) ;
    //信号的连接放在这里而不是MainWindows构造中，因为对象指针刷新了,fileEnd信号在process函数中发出，当读取结束后触发停止按钮
    timer->start(40);

    ui->pushButton->setEnabled(false);
    ui->buttonStop->setEnabled(true);
    ui->statusBar->showMessage("From Disk File, Working...");


}




//目标判断相关的参数由slider控制
void MainWindow::on_dial_sliderMoved(int position)
{
    //std::cout<<position<<std::endl;
    dec->roundTolerance=(position/100.0);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position)
{
    dec->minArea=(position);
}

void MainWindow::on_horizontalSlider_3_sliderMoved(int position)
{
    dec->maxArea=(position);

}
