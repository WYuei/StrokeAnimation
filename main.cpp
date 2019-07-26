#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QLatin1String>
#include <opencv2/opencv.hpp>

#include<iostream>
#include<cstdio>
#include<vector>
#include<queue>


#include "mainwindow.h"

// #include"ControllerStruct.h"

using namespace std;
using namespace cv;

const int IMAGE_GROUP_NUM = 2;
const int IMAGE_PART_NUM = 6;
Mat src[IMAGE_GROUP_NUM][IMAGE_PART_NUM + 1];
Mat src_tensor[IMAGE_GROUP_NUM][IMAGE_PART_NUM + 1];
Mat src_eigen[IMAGE_GROUP_NUM][IMAGE_PART_NUM + 1];
Mat src_draft[IMAGE_GROUP_NUM][IMAGE_PART_NUM + 1];
Mat src_color[IMAGE_GROUP_NUM][IMAGE_PART_NUM + 1];
Mat mid_tensor[IMAGE_GROUP_NUM][IMAGE_PART_NUM + 1];
Mat mid_color[IMAGE_GROUP_NUM][IMAGE_PART_NUM + 1];
Mat out_eigen[IMAGE_PART_NUM + 1];
Mat out_tensor[IMAGE_PART_NUM + 1];
Mat out_color[IMAGE_PART_NUM + 1];
Mat out_draft[IMAGE_PART_NUM + 1];



Mat s_normal, s_intensity, s_style, s_reference, s_location;
Mat t_normal, t_intensity, t_location, out_style, out_mask;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("StrokeAnimation");
    w.setGeometry(100,50,960,600);
    w.show();

    QFile qss("://Qss.qss");
    qss.open(QFile::ReadOnly);
    qDebug("open success.");
    QString style=QLatin1String(qss.readAll());
    a.setStyleSheet(style);
    qss.close();

   /* VectorPicture vp;
    Mat img,tensor,tensor_colored,eigen,intensity;
    //读取"sample08/0.txt"的矢量图，并保存在vp内
    ReadVectorPic(vp, "F:/newQT/copyRight/StrokeAnimation/1.txt");

    //将矢量图vp转为位图img
    GenerateImageFromVector(vp, img);
    imshow("source image", img);

    //将矢量图vp转为tensor图，其中tensor用于存储笔画的方向信息
    GenerateDirectedFieldFromVector(vp, tensor);
    //对tensor图染色，使其能被可视化（其实这个tensor图暂时用不到）
    DirectedField2ColorImage(tensor, tensor_colored);
    imshow("tensor image", tensor_colored);

    //将tensor图转为eigen图
    GenerateEigenImage(tensor, eigen);
    //将eigen图转为intensity图
    GenerateIntensityImage(eigen, intensity);
    imshow("intensity image", intensity);

    //暂时没有生成normal图的代码
    waitKey();*/


    return a.exec();
}
