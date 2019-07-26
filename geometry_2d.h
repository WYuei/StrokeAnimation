#pragma once
#include<cmath>
#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
const double eps = 1e-8;

int sgn(double x)
{
	if (fabs(x) < eps)return 0;
	if (x < 0)return -1;
	else return 1;
}

struct g_Point
{
	double x, y, val;
	bool operator <(const g_Point& p)
	{
		return val > p.val;
	}
	g_Point() {}
	g_Point(Vec2f p)
	{
		x = p[0];
		y = p[1];
	}
	g_Point(double _x, double _y)
	{
		x = _x;
		y = _y;
	}
	g_Point(double _x, double _y, double _v)
	{
		x = _x;
		y = _y;
		val = _v;
	}
	g_Point operator -(const g_Point &b)const
	{
		return g_Point(x - b.x, y - b.y);
	}
	//���
	double operator ^(const g_Point &b)const
	{
		return x * b.y - y * b.x;
	}
	//���
	double operator *(const g_Point &b)const
	{
		return x * b.x + y * b.y;
	}
	//��ԭ����ת�Ƕ�B������ֵ������x,y�ı仯
	void transXY(double B)
	{
		double tx = x, ty = y;
		x = tx * cos(B) - ty * sin(B);
		y = tx * sin(B) + ty * cos(B);
	}
};

struct g_Line
{
	g_Point s, e;
	g_Line() {}
	g_Line(g_Point _s, g_Point _e)
	{
		s = _s;
		e = _e;
	}
	//��ֱ���ཻ�󽻵�
	//��һ��ֵΪ0��ʾֱ���غϣ�Ϊ1��ʾƽ�У�Ϊ0��ʾ�ཻ,Ϊ2���ཻ
	//ֻ�е�һ��ֵΪ2ʱ�������������
	pair<int, g_Point> operator &(const g_Line &b)const
	{
		g_Point res = s;
		if (sgn((s - e) ^ (b.s - b.e)) == 0)
		{
			if (sgn((s - b.e) ^ (b.s - b.e)) == 0)
				return make_pair(0, res);//�غ�
			else return make_pair(1, res);//ƽ��
		}
		double t = ((s - b.s) ^ (b.s - b.e)) / ((s - e) ^ (b.s - b.e));
		res.x += (e.x - s.x)*t;
		res.y += (e.y - s.y)*t;
		return make_pair(2, res);
	}
};

//*��������
double dist(g_Point a, g_Point b)
{
	return sqrt((a - b)*(a - b));
}

//�ж��߶��ཻ
bool inter(g_Line l1, g_Line l2)
{
	return
        fmax(l1.s.x, l1.e.x) >= fmin(l2.s.x, l2.e.x) &&
        fmax(l2.s.x, l2.e.x) >= fmin(l1.s.x, l1.e.x) &&
        fmax(l1.s.y, l1.e.y) >= fmin(l2.s.y, l2.e.y) &&
        fmax(l2.s.y, l2.e.y) >= fmin(l1.s.y, l1.e.y) &&
		sgn((l2.s - l1.e) ^ (l1.s - l1.e))*sgn((l2.e - l1.e) ^ (l1.s - l1.e)) <= 0 &&
		sgn((l1.s - l2.e) ^ (l2.s - l2.e))*sgn((l1.e - l2.e) ^ (l2.s - l2.e)) <= 0;
}

//�ж�ֱ�ߺ��߶��ཻ
bool Seg_inter_line(g_Line l1, g_Line l2) //�ж�ֱ��l1���߶�l2�Ƿ��ཻ
{
	return sgn((l2.s - l1.e) ^ (l1.s - l1.e))*sgn((l2.e - l1.e) ^ (l1.s - l1.e)) <= 0;
}

//�㵽ֱ�߾���,����Ϊresult,�ǵ㵽ֱ������ĵ�
g_Point PointToLine(g_Point P, g_Line L)
{
	g_Point result;
	double t = ((P - L.s)*(L.e - L.s)) / ((L.e - L.s)*(L.e - L.s));
	result.x = L.s.x + (L.e.x - L.s.x)*t;
	result.y = L.s.y + (L.e.y - L.s.y)*t;
	return result;
}

g_Point NearestPointToLineSeg(g_Point P, g_Line L)
{
	g_Point result;
	double t = ((P - L.s)*(L.e - L.s)) / ((L.e - L.s)*(L.e - L.s));
	if (t >= 0 && t <= 1)
	{
		result.x = L.s.x + (L.e.x - L.s.x)*t;
		result.y = L.s.y + (L.e.y - L.s.y)*t;
	}
	else
	{
		if (dist(P, L.s) < dist(P, L.e))
			result = L.s;
		else result = L.e;
	}
	return result;
}
