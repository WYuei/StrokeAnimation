
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>


#include <QDebug>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QTime>
#include <QDir>
#include <QFile>
#include <QPainter>
#include <QDialog>
#include <QCursor>
#include <QCoreApplication>
#include <ctime>
#include <fstream>

#include"curve_draft.h"
#include"ControllerStruct.h"

using namespace std;
#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    globalMark=0;
    numLine=0;
    //numpoint=0;
    KeyNum=-1;
    group_number=0;
    //初始化画布
    pix=QPixmap(ui->label->size());
    pix.fill(Qt::white);
    ui->label->setPixmap(pix);
    //界面显示
    ui->toolinformation_2->hide();
    //ui->FpsTimes->setValue(24);

    //绘制刻度尺
    scale_1.ShowText(ui->ScaleMark);
    //绘制关键帧按钮
    timebutton.CreateButton(ui->frame); 

    lineflag=false;
    pointNumOfLine=0;

    //声明按钮信号和槽函数
    int i=0;
    while(i!=214)
    {
        connect(timebutton.newbutton[i]->button,SIGNAL(clicked(bool)),this,SLOT(KeyButton_slots()));
        i++;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionSave_triggered() //存储单帧矢量图
{
    QString folderName = QFileDialog::getExistingDirectory(
                   this,tr("select the file folder"),
                   "./"); //打开文件夹
    string totalPath=folderName.toStdString(); //整体文件地址

    string keyPath=totalPath+"/output/"; //keyframe的地址

    int i=0;
    int key;
    while(i<215) //找到要存储第几帧的内容
    {
        if(timebutton.newbutton[i]->button->isChecked()==true&&timebutton.newbutton[i]->createSignal==false)
        {
            QMessageBox mesg;
            mesg.warning(this,"error","nothing to save!!!");
            return;
        }
        if(timebutton.newbutton[i]->button->isChecked()==true&&timebutton.newbutton[i]->createSignal==true)
        {
           key=i;
           break;
        }
        i++;
    }


    string saveFrameNum=to_string(key);
    qDebug()<<QString::fromStdString(saveFrameNum);
    ofstream outfile(keyPath+saveFrameNum+".txt");

    outfile<<600<<' '<<800<<endl;



    /*在导出时读取vp，为了后续做tensor啥的准备*/
    VectorPicture vp;
    vp.lines.clear();
    vp.rows=600;
    vp.cols=800;

    outfile<<timebutton.newbutton[key]->pointCount.size()<<endl;//总共有多少条线

    int num=0;//标记输出到第几个点点了
    for(int index=0;index<timebutton.newbutton[key]->pointCount.size();index++)//范围：line的数量
    {
        int count=timebutton.newbutton[key]->pointCount.at(index);//一条line内的坐标点的个数

        vector<Vec2f> line;

        outfile<<count<<endl;
        qDebug()<<count;
        for(int j=num;j<num+count;j++)
        {   outfile<<timebutton.newbutton[key]->point[j].y()<<' '<<timebutton.newbutton[key]->point[j].x()<<endl;

            Vec2f pt;
            pt[0]=timebutton.newbutton[key]->point[j].y();
            pt[1]=timebutton.newbutton[key]->point[j].x();
            line.push_back(pt);
        }
        num=num+count;

        vp.lines.push_back(line);
    }

    outfile.close();


    QPixmap pixmap(size());//新建窗体大小的pixmap
    QPainter painter(&pixmap);//将pixmap作为画布
    painter.fillRect(QRect(0,0, ui->label->size().width(), ui->label->size().height()), Qt::white);//设置绘画区域、画布颜色
    this->render(&painter);//将窗体渲染到painter，再由painter画到画布
    pixmap.copy(QRect(ui->centralWidget->x(),ui->centralWidget->y(),ui->label->size().width(),ui->label->size().height())).save(QString::fromStdString(keyPath+saveFrameNum+".png"));//不包含工具栏



}

void MainWindow::on_actionOpen_triggered()
{
    ui->label->clear();//clear the label
    //open the image

    //找到是第几帧要open，要先添加好关键帧
    int i=0;
    int key;
    while(i<215)
    {
        if(timebutton.newbutton[i]->button->isChecked()==true&&timebutton.newbutton[i]->createSignal==false)
        {
            QMessageBox mesg;
            mesg.warning(this,"error","Only A Keyframe Can Open a new Frame!!!");
            return;
        }
        if(timebutton.newbutton[i]->button->isChecked()==true&&timebutton.newbutton[i]->createSignal==true)
        {
           key=i;
           break;
        }
        i++;
    }

  //  QPixmap ppm=QPixmap(ui->label->size()); //生成画布
  //  ppm.fill(Qt::white);
    QPixmap ppm=timebutton.newbutton[key]->pix;
    QPainter pxp(&ppm);
    pxp.begin(this);
    QColor black(0,0,0);//设置颜色
    QPen pen(black);//定义画笔
    pxp.setPen(pen);


    QString fileName = QFileDialog::getOpenFileName(
                   this,tr("open image file"),
                   "./"); //打开文件
    qDebug()<<fileName;
    string path=fileName.toStdString();
    ifstream fin(path);
    int lineNum, pointNum;
    int ii=0;
    int rows,cols;
    fin>>rows>>cols;
   // fin >> vp.rows >> vp.cols;
    fin >> lineNum;
    while(lineNum--)
    {
        fin >> pointNum;//读取一条line有几个点
        timebutton.newbutton[i]->pointCount.push_back(pointNum);
        if (pointNum == 0)
            continue;
        int count=0;//一条线内第几个点
        timebutton.newbutton[key]->pointNumber+=pointNum;
        while (pointNum--)
        {
            count++;
            int x,y;
            fin>>y>>x;
            timebutton.newbutton[key]->point[ii]=QPoint(x,y);
            if(count>=2)
            {
                pxp.drawLine(timebutton.newbutton[key]->point[ii-1],timebutton.newbutton[key]->point[ii]);
            }
            ii++;
        }
    }
    ui->label->setPixmap(ppm);
    timebutton.newbutton[key]->pix=ppm;
    pix=ppm;
    fin.close();

}

void MainWindow::on_actionNew_triggered()
{
    //初始化label界面
    QPixmap newPix=QPixmap(ui->label->size());
    newPix.fill(Qt::white);
    pix=newPix;

    //删除所有的数据——初始化所有的数据
    int i=0;
    while(i<group_number)
    {
        //删除关键帧中的数据——初始化关键帧
        timebutton.newbutton[keyNum_group[i]]->pix=newPix;
        timebutton.newbutton[keyNum_group[i]]->pointNumber=0;
        memset(timebutton.newbutton[keyNum_group[i]]->point,0,sizeof(timebutton.newbutton[keyNum_group[i]]->point));
        timebutton.newbutton[keyNum_group[i]]->pointCount.clear();
        timebutton.newbutton[keyNum_group[i]]->button->setStyleSheet("border:1px solid rgb(172,255,127)");
        timebutton.newbutton[keyNum_group[i]]->createSignal=false;
        timebutton.newbutton[keyNum_group[i]]->button->setChecked(false);
        i++;
    }
    timebutton.newbutton[KeyNum]->button->setChecked(true);
    numLine=0;
    KeyNum=-1;
    group_number=0;
}

void MainWindow::on_actionTool_Animation_triggered(bool checked)
{
    if(checked==true)
    {
        ui->AnimationWindow->show();
        ui->actionAnimation->setChecked(true);
        ui->actionTool_Animation->setIconVisibleInMenu(true);
    }
    else
    {
        ui->AnimationWindow->hide();
        ui->actionAnimation->setChecked(false);
        ui->actionTool_Animation->setIconVisibleInMenu(false);
    }
}

void MainWindow::paintEvent(QPaintEvent *)
{
    QPainter pp(&pix);
    pp.drawLine(lastpoint,endpoint);
    lastpoint=endpoint;
    //QPainter painter(&pix);
    //painter.drawPixmap(ui->centralWidget->x(),ui->centralWidget->y(),pix);
    ui->label->setPixmap(pix);
    if(KeyNum>0)
    {
        timebutton.newbutton[KeyNum]->pix=pix;
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(buttonType==1)
        {
            numLine++;
            lastpoint=event->pos();
            lastpoint=lastpoint-QPoint(ui->centralWidget->x(),ui->centralWidget->y());
            endpoint=lastpoint;

            lineflag=true;
            pointNumOfLine=0;
        }


    }

}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()&&Qt::LeftButton)
    {
        if(buttonType==1 && lineflag==true)
        {
            //更改画布中鼠标的图标
            QPoint mousepos=event->pos();
            if(mousepos.rx()>0 &&mousepos.rx()<10 &&mousepos.ry()>0 &&mousepos.ry()<10)
            {
                QPixmap pixmouse(":/new/icon/Resource/icon/mousepen.png");
                QSize picsize(16,16);
                QPixmap scaledpixmouse=pixmouse.scaled(picsize,Qt::KeepAspectRatio);
                QCursor *myCursor=new QCursor(scaledpixmouse,0,-2);    //-1,-1表示热点位于图片中心
                this->setCursor(*myCursor);
            }

            endpoint=event->pos();
            endpoint=endpoint-QPoint(ui->centralWidget->x(),ui->centralWidget->y());//获取此时鼠标的坐标
            //linepoint[numpoint]=lastpoint;
            //qDebug()<<linepoint[numpoint].x()<<" "<<linepoint[numpoint].y();

            timebutton.newbutton[KeyNum]->point[timebutton.newbutton[KeyNum]->pointNumber]=endpoint;
            timebutton.newbutton[KeyNum]->pointNumber++;
            pointNumOfLine++;
            //numpoint++;
            update();
        }
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)
    {
        if(buttonType==1)
        {
            endpoint=event->pos();
            endpoint=endpoint-QPoint(ui->centralWidget->x(),ui->centralWidget->y());
            timebutton.newbutton[KeyNum]->Line_number++;
            qDebug()<<timebutton.newbutton[KeyNum]->Line_number;

            lineflag=false;
            timebutton.newbutton[KeyNum]->pointCount.push_back(pointNumOfLine);
            qDebug()<<KeyNum<<":"<<pointNumOfLine;
            update();
        }
    }
}

void MainWindow::on_actionPencil_triggered(bool checked)
{
    if(checked==true)
    {
        buttonType=1;
    }
    else if(buttonType==1)
    {
        buttonType=0;
    }
}

void MainWindow::on_actionAnimation_triggered(bool checked)
{
    if(checked==true)
    {
        ui->AnimationWindow->show();
        ui->actionTool_Animation->setChecked(true);
        ui->actionTool_Animation->setIconVisibleInMenu(true);
    }
    else
    {
        ui->AnimationWindow->hide();
        ui->actionTool_Animation->setChecked(false);
        ui->actionTool_Animation->setIconVisibleInMenu(false);
    }
}

void MainWindow::on_actionEraser_triggered()
{
    QPixmap eraser=QPixmap(ui->label->size());
    eraser.fill(Qt::white);
    pix=eraser;
    if(KeyNum!=-1)
    {
        timebutton.newbutton[KeyNum]->pix=eraser;
        timebutton.newbutton[KeyNum]->pointNumber=0;
        memset(timebutton.newbutton[KeyNum]->point,0,sizeof(timebutton.newbutton[KeyNum]->point));
        timebutton.newbutton[KeyNum]->pointCount.clear();
    }
    ui->actionEraser->setChecked(false);
}

void MainWindow::on_KeyAdd_clicked()
{
    int i=0;
    while(i<215)
    {
        if(timebutton.newbutton[i]->button->isChecked()==true&&timebutton.newbutton[i]->createSignal==true)
        {
            QMessageBox mesg;
            mesg.warning(this,"error","Can't Create a new Frame!!!");
            return;
        }
        if(timebutton.newbutton[i]->button->isChecked()==true&&timebutton.newbutton[i]->createSignal==false)
        {
            timebutton.newbutton[i]->button->setStyleSheet("border:1px solid rgb(100,100,100)");
            timebutton.newbutton[i]->createSignal=true;
            timebutton.newbutton[i]->pix=QPixmap(ui->label->size());
            timebutton.newbutton[i]->pix.fill(Qt::white);
            pix=timebutton.newbutton[i]->pix;

            KeyNum=i;//赋值当前关键帧编号
            keyNum_group[group_number]=i;//把存在的关键帧存储在一个数组中
            group_number++;//记录关键帧个数
            break;
        }
        i++;
    }

}

void MainWindow::on_KeyDelete_clicked()
{
    int k=KeyNum;
    int j=0;
    int i=0;
    QPixmap p=QPixmap(ui->label->size());
    p.fill(Qt::white);

    if(timebutton.newbutton[KeyNum]->button->isChecked()==false)
    {
        QMessageBox mesg;
        mesg.warning(this,"error","The Frame is empty!!!Can't be Deleted!!!");
        return;
    }
    while(i<group_number)
    {
        if(keyNum_group[i]==KeyNum)
            break;
        i++;
    }
    if(timebutton.newbutton[KeyNum]->button->isChecked()==true)
    {
        timebutton.newbutton[KeyNum]->button->setStyleSheet("border:1px solid rgb(172,255,127)");
        timebutton.newbutton[KeyNum]->createSignal=false;
        timebutton.newbutton[KeyNum]->pointNumber=0;
        timebutton.newbutton[KeyNum]->pix=p;
        timebutton.newbutton[KeyNum]->button->setChecked(false);
        timebutton.newbutton[KeyNum]->Line_number=0;
    }
    //删除关键帧以后，界面的显示情况
    if(group_number>1)
    {
        j=i;
        group_number--;
        while(j<group_number)
        {
            keyNum_group[j]=keyNum_group[j+1];
            j++;
        }
        while(k>=0)
        {
            k--;
            if(timebutton.newbutton[k]->createSignal==true)
            {
                KeyNum=k;
                pix=timebutton.newbutton[k]->pix;
                timebutton.newbutton[k]->button->setChecked(true);
                break;
            }
        }
        if(KeyNum!=k)
        {
            k=KeyNum;
            while(k<215)
            {
                k++;
                if(timebutton.newbutton[k]->createSignal==true)
                {
                    KeyNum=k;
                    pix=timebutton.newbutton[k]->pix;
                    timebutton.newbutton[k]->button->setChecked(true);
                    break;
                }
            }
        }
    }
    else if(group_number==1)
    {
        pix=p;

        group_number=0;
        KeyNum=-1;

    }
}

void MainWindow::KeyButton_slots()
{
    int i=0;
    int n;
    while(i<group_number)
    {
        n=keyNum_group[i];
        if(timebutton.newbutton[n]->button->isChecked()==true)
        {
            KeyNum=n;//找到关键帧的索引

            /*每次点到某帧就进行重绘*/
            QPixmap ppm=QPixmap(ui->label->size());//新建画布
            ppm.fill(Qt::white);

            QPainter pxp(&ppm);//绑定画家
            pxp.begin(this);
            QColor black(0,0,0);//设置颜色
            QPen pen(black);//定义画笔
            pxp.setPen(pen);
            int num=0;
            for(int j=0;j<timebutton.newbutton[KeyNum]->pointCount.size();j++)//线的条数
            {
                int count=timebutton.newbutton[KeyNum]->pointCount.at(j);//第j条线有多少个点点
                for(int k=1;k<count;k++)
                    pxp.drawLine(timebutton.newbutton[KeyNum]->point[num+k-1],timebutton.newbutton[KeyNum]->point[num+k]);
                num+=count;

            }
            timebutton.newbutton[KeyNum]->pix=ppm;
            pix=timebutton.newbutton[n]->pix;
            ui->label->setPixmap(timebutton.newbutton[n]->pix);

        }
        i++;
    }
}

void MainWindow::WaitSec(double s) //延时
{
    QTime t;
    t.start();
    while(t.elapsed()<1000*s)
    {
        QCoreApplication::processEvents();
    }

}

void MainWindow::on_AnimationCreate_clicked()
{
    //生成中间帧的函数
}

void MainWindow::on_AnimationAction_clicked()
{
    int count_Number=0;
    int i=0;
    QPixmap p;
    p=timebutton.newbutton[KeyNum]->pix;
    //如果没有关键帧的时候
    if(group_number==0)
    {
        QMessageBox mesg;
        mesg.warning(this,"error","There ara not enough KeyFrames!!!");
        return;
    }
    else
    {
        while(i!=214 && count_Number!=group_number)
        {
            if(count_Number==0)
            {
                if(timebutton.newbutton[i]->createSignal==true)
                {

                    ui->label->setPixmap(timebutton.newbutton[i]->pix);
                    //pix=timebutton.newbutton[i]->pix;
                    count_Number++;
                    WaitSec(0.1);//等待1秒，跳转到下一个界面
                    qDebug()<<i;
                }
            }
            else if(count_Number>0)//寻找第一个显示的界面
            {
                if(timebutton.newbutton[i]->createSignal==true)
                {
                    //pix=timebutton.newbutton[i]->pix;
                    ui->label->setPixmap(timebutton.newbutton[i]->pix);
                    count_Number++;
                    WaitSec(0.1);//等待1秒，跳转到下一个界面
                    qDebug()<<i;
                }
                else
                {
                    WaitSec(0.1);
                }
            }
            i++;
        }
    }
    ui->label->setPixmap(p);
    pix=p;
}

void MainWindow::on_actionLines_triggered()
{
    int i=0;
    QPoint last,end;
    QPixmap ppm=timebutton.newbutton[KeyNum]->pix;

    last=timebutton.newbutton[KeyNum]->point[0]+QPoint(5,5);
    end=timebutton.newbutton[KeyNum]->point[1]+QPoint(5,5);
    while(i<timebutton.newbutton[KeyNum]->pointNumber-1)
    {
        QPainter pxp(&ppm);
        pxp.drawLine(last,end);
        last=end;
        if(2*i<timebutton.newbutton[KeyNum]->pointNumber-1)
        {
            end=timebutton.newbutton[KeyNum]->point[i+2]+QPoint(3,3);
        }
        else
        {
            end=timebutton.newbutton[KeyNum]->point[i+2]+QPoint(-3,-5);
        }
        ui->label->setPixmap(ppm);
        timebutton.newbutton[KeyNum]->pix=ppm;
        qDebug()<<last<<" "<<KeyNum;
        i++;
    }

    i=4;
    last=timebutton.newbutton[KeyNum]->point[4]+QPoint(-10,-5);
    end=timebutton.newbutton[KeyNum]->point[5]+QPoint(-5,-10);
    while(i<timebutton.newbutton[KeyNum]->pointNumber-5)
    {
        QPainter pxp(&ppm);
        pxp.drawLine(last,end);
        last=end;
        if(2*i<timebutton.newbutton[KeyNum]->pointNumber-1)
        {
            end=timebutton.newbutton[KeyNum]->point[i+2]+QPoint(-5,-10);
        }
        else
        {
            end=timebutton.newbutton[KeyNum]->point[i+2]+QPoint(-10,-5);
        }
        ui->label->setPixmap(ppm);
        timebutton.newbutton[KeyNum]->pix=ppm;
        qDebug()<<last<<" "<<KeyNum;
        i++;
    }

    i=5;
    last=timebutton.newbutton[KeyNum]->point[5]+QPoint(-4,-8);
    end=timebutton.newbutton[KeyNum]->point[7]+QPoint(-7,-7);
    while(i<timebutton.newbutton[KeyNum]->pointNumber-2)
    {
        QPainter pxp(&ppm);
        pxp.drawLine(last,end);
        last=end;
        if(2*i<timebutton.newbutton[KeyNum]->pointNumber-1)
        {
            end=timebutton.newbutton[KeyNum]->point[i+4]+QPoint(-4,-8);
        }
        else
        {
            end=timebutton.newbutton[KeyNum]->point[i+4]+QPoint(2,7);
        }
        ui->label->setPixmap(ppm);
        timebutton.newbutton[KeyNum]->pix=ppm;
        qDebug()<<last<<" "<<KeyNum;
        i=i+2;
    }
    pix=timebutton.newbutton[KeyNum]->pix;
    ui->actionLines->setChecked(false);
}

void MainWindow::on_actionRandomLine_triggered()
{
    qsrand(time(NULL));
    int n=timebutton.newbutton[KeyNum]->pointNumber;//关键帧中的点数量
    int m=1;//随机选取关键帧中的点
    int i=1;
    int b1=0,b2=0,x=0,y=0;
    QPoint last,end;
    QPixmap ppm=timebutton.newbutton[KeyNum]->pix;

    while(m<n-5)
    {
        x=timebutton.newbutton[KeyNum]->point[m].x()-timebutton.newbutton[KeyNum]->point[m-1].x();
        y=timebutton.newbutton[KeyNum]->point[m].y()-timebutton.newbutton[KeyNum]->point[m-1].y();
        if(y==0)
        {
            b1=5;
            last=timebutton.newbutton[KeyNum]->point[m]+QPoint(0,b1);
        }
        else if(y>0)
        {
            b1=(-5)*x/y;
            last=timebutton.newbutton[KeyNum]->point[m]+QPoint(5,b1);
        }
        else
        {
            b1=5*x/y;
            last=timebutton.newbutton[KeyNum]->point[m]+QPoint(-5,b1);
        }

        x=timebutton.newbutton[KeyNum]->point[m+1].x()-timebutton.newbutton[KeyNum]->point[m].x();
        y=timebutton.newbutton[KeyNum]->point[m+1].y()-timebutton.newbutton[KeyNum]->point[m].y();
        if(y==0)
        {
            b2=1;
            end=timebutton.newbutton[KeyNum]->point[m+1]+QPoint(0,b2);
        }
        else if(y>0)
        {
            b2=((-5)*x/y)+4;
            end=timebutton.newbutton[KeyNum]->point[m+1]+QPoint(5,b2);
        }
        else
        {
            b2=(5*x/y)+4;
            end=timebutton.newbutton[KeyNum]->point[m+1]+QPoint(-5,b2);
        }
        while(i<5)
        {
            QPainter pxp(&ppm);
            pxp.drawLine(last,end);
            last=end;
            b2=b2+4;
            end=timebutton.newbutton[KeyNum]->point[m+1+i]+QPoint(5,b2);
            ui->label->setPixmap(ppm);
            timebutton.newbutton[KeyNum]->pix=ppm;
            //qDebug()<<last<<" "<<KeyNum;
            i++;
        }
        m=m+2;
        i=1;
    }
    pix=timebutton.newbutton[KeyNum]->pix;
    ui->actionRandomLine->setChecked(false);
}

void MainWindow::on_actionBlack_triggered()
{
    QPixmap ppm=QPixmap(ui->label->size());
    ppm.fill(Qt::white);
    QPoint last,end;
    int a=0;
    last=timebutton.newbutton[KeyNum]->point[0];
    end=timebutton.newbutton[KeyNum]->point[1];
    while(a<timebutton.newbutton[KeyNum]->pointNumber-1)
    {
        QPainter pxp(&ppm);
        QColor Black(0,0,0);//设置颜色
        QPen pen(Black);//定义画笔
        pxp.setPen(pen);
        pxp.drawLine(last,end);
        last=end;
        end=timebutton.newbutton[KeyNum]->point[a+2];
        ui->label->setPixmap(ppm);
        timebutton.newbutton[KeyNum]->pix=ppm;
        a++;
    }
    if(timebutton.newbutton[KeyNum]->Line_number!=1)
    {
        on_actionRandomLine_triggered();
    }
    timebutton.newbutton[KeyNum]->pix_b=ppm;
    timebutton.newbutton[KeyNum]->pix=ppm;
    ui->actionBlack->setChecked(false);
}

void MainWindow::on_actionWhite_triggered()
{
    QPixmap ppm=QPixmap(ui->label->size());
    ppm.fill(Qt::white);
    QPoint last,end;
    int a=0;
    last=timebutton.newbutton[KeyNum]->point[0];
    end=timebutton.newbutton[KeyNum]->point[1];
    while(a<timebutton.newbutton[KeyNum]->pointNumber-1)
    {
        QPainter pxp(&ppm);
        QColor White(255,255,255);//设置颜色
        QPen pen(White);//定义画笔
        pxp.setPen(pen);
        pxp.drawLine(last,end);
        last=end;
        end=timebutton.newbutton[KeyNum]->point[a+2];
        ui->label->setPixmap(ppm);
        timebutton.newbutton[KeyNum]->pix=ppm;
        a++;
    }
    if(timebutton.newbutton[KeyNum]->Line_number!=1)
    {
        on_actionRandomLine_triggered();
    }
    timebutton.newbutton[KeyNum]->pix_w=ppm;
    timebutton.newbutton[KeyNum]->pix=ppm;
    ui->actionWhite->setChecked(false);
}

void MainWindow::on_actionGreen_triggered()
{
    QPixmap ppm=QPixmap(ui->label->size());
    ppm.fill(Qt::white);
    QPoint last,end;
    int a=0;
    last=timebutton.newbutton[KeyNum]->point[0];
    end=timebutton.newbutton[KeyNum]->point[1];
    while(a<timebutton.newbutton[KeyNum]->pointNumber-1)
    {
        QPainter pxp(&ppm);
        QColor Green(0,255,0);//设置颜色
        QPen pen(Green);//定义画笔
        pxp.setPen(pen);
        pxp.drawLine(last,end);
        last=end;
        end=timebutton.newbutton[KeyNum]->point[a+2];
        ui->label->setPixmap(ppm);
        timebutton.newbutton[KeyNum]->pix=ppm;
        a++;
    }
    if(timebutton.newbutton[KeyNum]->Line_number!=1)
    {
        on_actionRandomLine_triggered();
    }
    timebutton.newbutton[KeyNum]->pix_g=ppm;
    timebutton.newbutton[KeyNum]->pix=ppm;
    ui->actionGreen->setChecked(false);
}

void MainWindow::on_actionOpen_All_triggered()
{   ui->label->clear();

    QString folderName = QFileDialog::getExistingDirectory(
                   this,tr("select the file folder"),
                   "./"); //打开文件夹
    string totalPath=folderName.toStdString(); //整体文件地址

    string keyPath=totalPath+"/keyframe"; //keyframe的地址

    QDir dir(QString::fromStdString(keyPath));

    int index;//索引
    QString title;
    QFileInfoList list;
    QFileInfo fileInfo;
    list=dir.entryInfoList();

    for(index=0;index<list.size();index++)
    {
        fileInfo=list.at(index);//获取第index的文件
        if(fileInfo.suffix()=="txt")
        {   title=fileInfo.baseName();
            int i=title.toInt(); //读取的文件获得是第i帧
            string keyRealPath=fileInfo.filePath().toStdString();//文件的全部地址


            //让第i帧成为关键帧
            timebutton.newbutton[i]->button->setStyleSheet("border:1px solid rgb(100,100,100)");
            timebutton.newbutton[i]->createSignal=true;
            timebutton.newbutton[i]->pix=QPixmap(ui->label->size());
            timebutton.newbutton[i]->pix.fill(Qt::white);
            pix=timebutton.newbutton[i]->pix;

            keyNum_group[group_number]=i;//把存在的关键帧存储在一个数组中
            group_number++;//记录关键帧个数

            //开始绘图了！

             QPixmap ppm=timebutton.newbutton[i]->pix;
             QPainter pxp(&ppm);
             pxp.begin(this);
             QColor black(0,0,0);//设置颜色
             QPen pen(black);//定义画笔
             pxp.setPen(pen);

             ifstream fin(keyRealPath);//把文件中的内容读出来
             int lineNum, pointNum;
             int ii=0;
             int rows,cols;
             fin>>rows>>cols;
             fin >> lineNum;
             while(lineNum--)
             {
                 fin >> pointNum;//读取一条line有几个点
                 timebutton.newbutton[i]->pointCount.push_back(pointNum);
                 if (pointNum == 0)
                     continue;
                 int count=0;//一条线内第几个点
                 timebutton.newbutton[i]->pointNumber+=pointNum;
                 while (pointNum--)
                 {
                     count++;
                     int x,y;
                     fin>>y>>x;
                     timebutton.newbutton[i]->point[ii]=QPoint(x,y);
                     if(count>=2)
                     {
                         pxp.drawLine(timebutton.newbutton[i]->point[ii-1],timebutton.newbutton[i]->point[ii]);
                     }
                     ii++;
                 }
             }
             ui->label->setPixmap(ppm);
             timebutton.newbutton[i]->oripix=ppm;
             timebutton.newbutton[i]->pix=ppm;
             pix=ppm;
             fin.close();}

    }

    string guiPath[3]={"/guidance0","/guidance1","/guidance3"};//设置三个guidance的文件夹路径

    for(int folderNum=0;folderNum<3;folderNum++)
    {
        string guidanceFolderPath=totalPath+guiPath[folderNum];
        QDir temp(QString::fromStdString(guidanceFolderPath));//找到第folderNum个guidance的文件夹地址
        QString title;
        QFileInfoList list;
        QFileInfo fileInfo;
        list=temp.entryInfoList();
        for(index=0;index<list.size();index++)
        {
            fileInfo=list.at(index);//获取第index的guidance图
            title=fileInfo.baseName();
            int i=title.toInt(); //读取的guidance的获得是第i帧
            if(fileInfo.suffix()=="png" && timebutton.newbutton[i]->createSignal==true) //如果是关键帧的话就读取guidance
            {    QImage img(fileInfo.filePath());
                 img=img.scaled(ui->label->width(),ui->label->height(),Qt::KeepAspectRatio);//Qt::KeepAspectRatio用于自适应大小，不变形
                 ui->label->setPixmap(QPixmap::fromImage(img));
                 if(img.isNull())
                 {
                     QMessageBox::information(this,"Message","Fail to open the File!!!");
                     return ;
                 }
                 qDebug()<<folderNum<<":"<<i<<":"<<fileInfo.filePath();
                 switch(folderNum)
                 {
                    case 0:
                     timebutton.newbutton[i]->guidance0=QPixmap::fromImage(img);
                     break;
                    case 1:
                     timebutton.newbutton[i]->guidance1=QPixmap::fromImage(img);
                     break;
                    case 2:
                     timebutton.newbutton[i]->guidance3=QPixmap::fromImage(img);
                     break;
                    default:
                     break;
                 }

            }

        }
    }
    pix=timebutton.newbutton[0]->oripix;
}

void MainWindow::on_comboBox_activated(const QString &arg1)//下拉框选择
{
    int i=0;
    int key;
    while(i<215)
    {
        if(timebutton.newbutton[i]->button->isChecked()==true&&timebutton.newbutton[i]->createSignal==false) //找到是哪一帧被选择了
        {
            QMessageBox mesg;
            mesg.warning(this,"error","this frame isnt key frame");
            return;
        }
        if(timebutton.newbutton[i]->button->isChecked()==true&&timebutton.newbutton[i]->createSignal==true)
        {
           key=i;
           break;
        }
        i++;
    }

    string selectStr=arg1.toStdString();

    if (selectStr=="original")
    {    pix=timebutton.newbutton[key]->oripix;
    }
    else
        if(selectStr=="guidance0")
            pix=timebutton.newbutton[key]->guidance0;
        else if(selectStr=="guidance1")
            pix=timebutton.newbutton[key]->guidance1;
            else
                 pix=timebutton.newbutton[key]->guidance3;

}

void MainWindow::on_actionSave_All_triggered()
{
    QString folderName = QFileDialog::getExistingDirectory(
                   this,tr("select the file folder"),
                   "./"); //打开文件夹
    string totalPath=folderName.toStdString(); //整体文件地址

    string keyPath=totalPath+"/output/"; //keyframe的地址

    for (int keyIndex = 0; keyIndex < group_number; keyIndex++) {
          int i=keyNum_group[keyIndex];

          string saveFrameNum=to_string(i);

          ofstream outfile(keyPath+saveFrameNum+".txt");

          outfile<<600<<' '<<800<<endl;
          outfile<<timebutton.newbutton[i]->pointCount.size()<<endl;//总共有多少条线

          int num=0;//标记输出到第几个点点了
          for(int index=0;index<timebutton.newbutton[i]->pointCount.size();index++)//范围：line的数量
          {
              int count=timebutton.newbutton[i]->pointCount.at(index);//一条line内的坐标点的个数
              outfile<<count<<endl;
              for(int j=num;j<num+count;j++)
                  outfile<<timebutton.newbutton[i]->point[j].y()<<' '<<timebutton.newbutton[i]->point[j].x()<<endl;
              num=num+count;
          }

          outfile.close();

    }


}
