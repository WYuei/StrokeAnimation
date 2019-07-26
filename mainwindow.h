#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QImage>
#include <QMouseEvent>
#include <QPushButton>
#include <QMdiSubWindow>

#include "scalemark.h"
#include "timebutton.h"



namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *event);//鼠标按下事件处理函数
    void mouseMoveEvent(QMouseEvent *event);//鼠标移动事件处理函数
    void mouseReleaseEvent(QMouseEvent *event);//鼠标释放事件处理函数


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int globalMark;// a mark for image save
    QImage globalImg;// for save image
    int numLine;
    //QPoint linepoint[5000];
    //int numpoint;

  //  VectorPicture vp;

    int KeyNum;//关键帧编号
    int keyNum_group[500];
    int group_number;

    ScaleMark scale_1;
    TimeButton timebutton;

    void WaitSec(double s);

private slots:
    void on_actionSave_triggered();

    void on_actionOpen_triggered();

    void on_actionNew_triggered();

    void on_actionTool_Animation_triggered(bool checked);

    void on_actionPencil_triggered(bool checked);

    void on_actionAnimation_triggered(bool checked);

    void on_actionEraser_triggered();

    void on_KeyAdd_clicked();

    void on_KeyDelete_clicked();

    void KeyButton_slots();

    void on_AnimationCreate_clicked();

    void on_AnimationAction_clicked();

    void on_actionLines_triggered();

    void on_actionRandomLine_triggered();

    void on_actionBlack_triggered();

    void on_actionWhite_triggered();

    void on_actionGreen_triggered();

    //void on_KeyAdd_clicked(bool checked);

    void on_actionOpen_All_triggered();

    void on_comboBox_activated(const QString &arg1);

    void on_actionSave_All_triggered();

private:
    Ui::MainWindow *ui;
    int buttonType;//按钮类型
    bool lineflag; //判断在绘制的时候是否是同一条线
    int pointNumOfLine;
    QPixmap pix;//全局变量保存绘制的图片
    QPoint lastpoint;
    QPoint endpoint;
    qreal scale;
    QPushButton *button;
};

#endif // MAINWINDOW_H

