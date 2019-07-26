#pragma once
#include<iostream>
#include<cstdio>
#include<vector>
#include<opencv.hpp>
#include"geometry_2d.h"
using namespace std;
using namespace cv;

const int OverlapNum = 3;
const int MAX_DR = 50;
const int MAX_LineMidL = 500;
const int MAX_PlaySpeed = 20;
const int MAX_RAND_NUM = 10000;
const int Line_Rand_Num = 100;
int DR = 30;
int LineNum = 50;
int LineMidL = 100;
int PlaySpeed = 5;

void GenerateStraightLineDraftFromThin(Mat& src, Mat& output)
{
	Mat in = src.clone();
	vector<g_Point> srcP;
	vector<int> srcV;
	int pnum, lineMaxL, lineMinL;
	vector<g_Line> randL;
	vector<int> randV;
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			uchar pt = in.at<uchar>(i, j);
			if (pt == 255)
			{
				srcP.push_back(g_Point(i, j));
				srcV.push_back(1 << OverlapNum);
			}
		}
	}
	pnum = srcP.size();
	lineMaxL = LineMidL * 1.5;
	lineMinL = LineMidL * 0.5;
	LineNum = pnum * OverlapNum * 2 / (LineMidL + 1);
	output = Mat(in.rows, in.cols, CV_8UC1, Scalar(0));
	for (int cnt = 0; cnt < LineNum; cnt++)
	{
		//lineRandNum = min(100, cnt * 5 + 20);
		randL.clear();
		randV.clear();
		for (int rid = 0; rid < Line_Rand_Num; rid++)
		{
			int randCnt = 0;
			int sid, eid, sx, sy, ex, ey, dis, pnx, pny;
			for (; randCnt < MAX_RAND_NUM; randCnt++)
			{
				sid = rand() % pnum;
				eid = rand() % pnum;
				pnx = rand() % 2 * 2 - 1;
				pny = rand() % 2 * 2 - 1;
				sx = srcP[sid].x + pnx * ((rand() % (DR + 1))*(rand() % (DR + 1)) / (DR + 1));
				sy = srcP[sid].y + pny * ((rand() % (DR + 1))*(rand() % (DR + 1)) / (DR + 1));
				ex = srcP[eid].x - pnx * ((rand() % (DR + 1))*(rand() % (DR + 1)) / (DR + 1));
				ey = srcP[eid].y - pny * ((rand() % (DR + 1))*(rand() % (DR + 1)) / (DR + 1));
				dis = dist(g_Point(sx, sy), g_Point(ex, ey));
				if (dis >= lineMinL && dis <= lineMaxL)
					break;
			}
			if (randCnt >= MAX_RAND_NUM)
				break;
			g_Line tmpL = g_Line(g_Point(sx, sy), g_Point(ex, ey));
			int tmpV = 0;
			for (int i = 0; i < pnum; i++)
			{
				int dis = dist(srcP[i], NearestPointToLineSeg(srcP[i], tmpL));
				if (dis < DR / 2)
				{
					//tmpV += srcV[i] / (dis + 1);
					tmpV += srcV[i] / 2;
				}
			}
			randL.push_back(tmpL);
			randV.push_back(tmpV);
		}
		if (randV.size() == 0)
			break;
		int mxId = 0;
		for (int i = 0; i < randV.size(); i++)
		{
			if (randV[mxId] < randV[i])
				mxId = i;
		}
		for (int i = 0; i < pnum; i++)
		{
			if (dist(srcP[i], NearestPointToLineSeg(srcP[i], randL[mxId])) < DR / 2)
			{
				//srcV[i] -= srcV[i] / (dist(srcP[i], NearestPointToLineSeg(srcP[i], randL[mxId])) + 1);
				srcV[i] -= srcV[i] / 2;
			}
		}
		line(output, Point(randL[mxId].s.y, randL[mxId].s.x), Point(randL[mxId].e.y, randL[mxId].e.x), Scalar(255), 1);
	}
}

void LineTest()
{
	srand(time(NULL));
	Mat img = imread("test.png");
	//	cvtColor(img, img, COLOR_BGR2GRAY);
	//	threshold(img,img,220,255,THRESH_BINARY);
	Mat output;
	namedWindow("source");
	namedWindow("output");
	createTrackbar("疏密度", "output", &DR, MAX_DR, NULL);
	createTrackbar("线段长度", "output", &LineMidL, MAX_LineMidL, NULL);
	createTrackbar("最大帧率", "output", &PlaySpeed, MAX_PlaySpeed, NULL);
	int inputNum = 0;
	while (1)
	{
		img = imread("test" + to_string(inputNum) + ".png");
		cvtColor(img, img, COLOR_BGR2GRAY);
		threshold(img, img, 1, 255, THRESH_BINARY_INV);
		//img = imread("acg1.png");
		imshow("source", img);
		//inputNum = (inputNum + 1) % 10;
		GenerateStraightLineDraftFromThin(img, output);
		imshow("output", output);
		if (PlaySpeed)
			waitKey(1000 / PlaySpeed);
		else
			waitKey();
	}
}
