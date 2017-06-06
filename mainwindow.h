#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <detector.h>
#include <QTimer>
#include <QString>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QTimer *timer;
    Detector * dec = new Detector("null.avi" ) ;//-----自定义类的实现


    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_actionExit_triggered();
    void timerSlot();


    void on_buttonStop_clicked();

    void on_pushButton_2_clicked();

    //void on_dial_sliderMoved(int position);

    void on_dial_sliderMoved(int position);

    void on_horizontalSlider_sliderMoved(int position);

    void on_horizontalSlider_3_sliderMoved(int position);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
