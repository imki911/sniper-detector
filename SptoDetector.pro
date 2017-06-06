#-------------------------------------------------
#
# Project created by QtCreator 2017-04-25T09:27:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SptoDetector
TEMPLATE = app
CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    detector.cpp

HEADERS  += mainwindow.h \
    detector.h

FORMS    += mainwindow.ui

INCLUDEPATH+=D:\OpenCv2.4\opencv2410\build\include
                    D:\OpenCv2.4\opencv2410\build\include\opencv2


LIBS+=  D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_calib3d2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_contrib2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_core2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_features2d2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_flann2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_gpu2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_highgui2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_imgproc2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_legacy2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_ml2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_objdetect2410.dll.a\
        D:\OpenCv2.4\opencv2410\buildGCC\lib\libopencv_video2410.dll.a

RESOURCES += \
    img1.qrc
