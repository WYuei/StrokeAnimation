#ifndef KEYBUTTON_H
#define KEYBUTTON_H

#include <QObject>
#include <QRadioButton>
#include <QPixmap>
#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QPoint>

class KeyButton :public QWidget
{
public:
    KeyButton(int number);

    QRadioButton *button;
    QPixmap pix;
    QPixmap pix_b;
    QPixmap pix_w;
    QPixmap pix_g;

    QPixmap oripix;

    QPixmap guidance0;
    QPixmap guidance1;
    QPixmap guidance3;

    QVector<int> pointCount;//一条线有多少个点点

    bool createSignal;//是否已经被创建
    int Num; //按钮编号
    QPoint point[5000];//动态存储QPoint数据
    int pointNumber;
    int Line_number;

protected:
    //void mousePressEvent(QMouseEvent *event);


};

#endif // KEYBUTTON_H
