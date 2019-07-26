#pragma once
#include<vector>
#include<fstream>
#include"my_math.h"
#include"geometry_2d.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QString>
#include <QLatin1String>

struct VectorPicture
{
	int rows, cols;//矢量图的长宽
	double aveCurveLength, varCurveLength, aveCurveBlending, minCurveDistance;//风格参数
	vector<vector<Vec2f>> lines;//lines[i]表示第i条曲线，lines[i][j]表示第i条曲线上第j个点
};

//计算矢量图风格参数，暂时无用
void CalculateVectorPictureCharacteristics(VectorPicture& vp)
{
	vector<double> lenVec;
	for (int lineId = 0; lineId < vp.lines.size(); lineId++)
	{
		double len = 0;
		for (int pointId = 1; pointId < vp.lines[lineId].size(); pointId++)
		{
			Vec2f v1 = vp.lines[lineId][pointId];
			Vec2f v2 = vp.lines[lineId][pointId - 1];
			len += sqrt((v1[0] - v2[0])*(v1[0] - v2[0]) + (v1[1] - v2[1])*(v1[1] - v2[1]));
		}
		lenVec.push_back(len);
	}
	double sumLen = 0;
	for (int i = 0; i < lenVec.size(); i++)
		sumLen += lenVec[i];
	double aveLen = sumLen / lenVec.size();
	double varLen = 0;
	for (int i = 0; i < lenVec.size(); i++)
		varLen += (lenVec[i] - aveLen)*(lenVec[i] - aveLen);
	varLen /= lenVec.size();

	double blending = 0;
	for (int i = 0; i < vp.lines.size(); i++)
	{
		Vec2f v = vp.lines[i][0] - vp.lines[i][vp.lines[i].size() - 1];
		blending += lenVec[i] / sqrt(v[0] * v[0] + v[1] * v[1]);
	}
	blending /= lenVec.size();

	vector<double> ldisVec;
	for (int lineId1 = 0; lineId1 < vp.lines.size(); lineId1++)
	{
		for (int lineId2 = lineId1 + 1; lineId2 < vp.lines.size(); lineId2++)
		{
			double ldis = 1e9;
			for (int pointId1 = 1; pointId1 < vp.lines[lineId1].size(); pointId1++)
			{
				for (int pointId2 = 1; pointId2 < vp.lines[lineId2].size(); pointId2++)
				{
					g_Line l1 = g_Line(g_Point(vp.lines[lineId1][pointId1 - 1]), g_Point(vp.lines[lineId1][pointId1]));
					g_Line l2 = g_Line(g_Point(vp.lines[lineId2][pointId2 - 1]), g_Point(vp.lines[lineId2][pointId2]));
					if (inter(l1, l2))
						ldis = 0;
					else
						ldis = min(ldis, min(
							dist(l1.s, NearestPointToLineSeg(l1.s, l2)),
							dist(l1.e, NearestPointToLineSeg(l1.e, l2))));
				}
			}
			ldisVec.push_back(ldis);
		}
	}
	sort(ldisVec.begin(), ldisVec.end());
	double minCurveDistance = 1;
	int cnt = 0;
	for (int i = 0; i < ldisVec.size(); i++)
	{
		if (ldisVec[i] < eps)
			continue;
		minCurveDistance = ldisVec[(ldisVec.size() - i) / 10 + i];
		break;
	}

	vp.aveCurveLength = aveLen;
	vp.varCurveLength = varLen;
	vp.aveCurveBlending = blending;
	vp.minCurveDistance = minCurveDistance;
}

//读取地址在address的矢量图，并存储在vp内
void ReadVectorPic(VectorPicture& vp, string address)
{
	vp.lines.clear();
	ifstream fin(address);
	int lineNum, pointNum;
	fin >> vp.rows >> vp.cols;
	fin >> lineNum;
	while(lineNum--)
	{
		vector<Vec2f> line;
		fin >> pointNum;
        qDebug()<<pointNum;
		if (pointNum == 0)
			continue;
		while (pointNum--)
		{
			Vec2f pt;
			fin >> pt[0] >> pt[1];
            qDebug()<<pt[0]<<","<<pt[1];
			line.push_back(pt);
		}
		vp.lines.push_back(line);
	}
	fin.close();
	CalculateVectorPictureCharacteristics(vp);
}

//将矢量图vp，输出到地址address
void WriteVectorPic(const VectorPicture& vp, string address)
{
	ofstream fout(address);
	fout << vp.rows << " " << vp.cols << endl;
	fout << vp.lines.size() << endl;
	for (int lineId = 0; lineId < vp.lines.size(); lineId++)
	{
		fout << vp.lines[lineId].size() << endl;
		for (int pointId = 0; pointId < vp.lines[lineId].size(); pointId++)
		{
			Vec2f p = vp.lines[lineId][pointId];
			fout << p[0] << " " << p[1] << endl;
		}
	}
	fout.close();
}

//从矢量图vp生成位图out
void GenerateImageFromVector(const VectorPicture& vp, Mat& out)
{
	out = Mat(vp.rows, vp.cols, CV_8UC3, Scalar(255, 255, 255));
	for (int lineId = 0; lineId < vp.lines.size(); lineId++)
	{
		for (int pointId = 0; pointId < vp.lines[lineId].size() - 1; pointId++)
		{
			Point st = Point(vp.lines[lineId][pointId][1], vp.lines[lineId][pointId][0]);
			Point ed = Point(vp.lines[lineId][pointId + 1][1], vp.lines[lineId][pointId + 1][0]);
            line(out, st, ed, Scalar(0, 0, 0));
		}
	}
	
}

Mat draw_board;
vector<Vec2f> draw_lineVec;
Point draw_prePoint;
bool draw_buttonDownFlag = 0;
int draw_delay = 0;

//绘制矢量图时的鼠标响应函数
void OnMouseDrawingVectorPic(int event, int x, int y, int flag, void *param)
{
	VectorPicture& vp = *(VectorPicture*) param;
	if (event == EVENT_LBUTTONDOWN)
	{
		draw_buttonDownFlag = 1;
		draw_prePoint = Point(x, y);
		draw_lineVec.clear();
		draw_lineVec.push_back(Vec2f(y, x));
	}
	else if (event == EVENT_MOUSEMOVE)
	{
		if (draw_buttonDownFlag)
		{
			draw_delay = (draw_delay + 1) % 5;
			if (draw_delay == 0)
			{
				line(draw_board, draw_prePoint, Point(x, y), Scalar(0, 0, 0));
				draw_prePoint = Point(x, y);
				draw_lineVec.push_back(Vec2f(y, x));
				imshow("board", draw_board);
			}
			
		}
	}
	else if (event == EVENT_LBUTTONUP)
	{
		vp.lines.push_back(draw_lineVec);
		draw_lineVec.clear();
		draw_buttonDownFlag = 0;
	}
}

//绘制矢量图，并以矢量图.txt和位图.png格式分别保存在address内
void TestDrawVectorPicture(string address, int rows, int cols, Mat baseMap = Mat(0, 0, CV_8UC3))
{
	VectorPicture vp;
	vp.rows = rows;
	vp.cols = cols;
	if (baseMap.empty())
		draw_board = Mat(vp.rows, vp.cols, CV_8UC3, Scalar(255, 255, 255));
	else
		draw_board = baseMap.clone();
	imshow("board", draw_board);
	vector<Vec2f> lineVec;
	setMouseCallback("board", OnMouseDrawingVectorPic, &vp);
	waitKey();
	WriteVectorPic(vp, address + ".txt");
	imwrite(address + ".png", draw_board);
}
