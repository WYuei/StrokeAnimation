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
	//叉积
	double operator ^(const g_Point &b)const
	{
		return x * b.y - y * b.x;
	}
	//点积
	double operator *(const g_Point &b)const
	{
		return x * b.x + y * b.y;
	}
	//绕原点旋转角度B（弧度值），后x,y的变化
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
	//两直线相交求交点
	//第一个值为0表示直线重合，为1表示平行，为0表示相交,为2是相交
	//只有第一个值为2时，交点才有意义
	pair<int, g_Point> operator &(const g_Line &b)const
	{
		g_Point res = s;
		if (sgn((s - e) ^ (b.s - b.e)) == 0)
		{
			if (sgn((s - b.e) ^ (b.s - b.e)) == 0)
				return make_pair(0, res);//重合
			else return make_pair(1, res);//平行
		}
		double t = ((s - b.s) ^ (b.s - b.e)) / ((s - e) ^ (b.s - b.e));
		res.x += (e.x - s.x)*t;
		res.y += (e.y - s.y)*t;
		return make_pair(2, res);
	}
};

//*两点间距离
double dist(g_Point a, g_Point b)
{
	return sqrt((a - b)*(a - b));
}

//判断线段相交
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

//判断直线和线段相交
bool Seg_inter_line(g_Line l1, g_Line l2) //判断直线l1和线段l2是否相交
{
	return sgn((l2.s - l1.e) ^ (l1.s - l1.e))*sgn((l2.e - l1.e) ^ (l1.s - l1.e)) <= 0;
}

//点到直线距离,返回为result,是点到直线最近的点
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
