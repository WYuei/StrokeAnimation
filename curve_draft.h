#pragma once
#include<iostream>
#include<cstdio>
#include<vector>
#include<queue>
#include<opencv.hpp>
#include"my_math.h"
#include"geometry_2d.h"
#include"line_draft.h"
#include"my_vector_picture.h"
using namespace std;
using namespace cv;

int cont_field_r = 50;
int cont_self_k = 100;
int cont_derivative_k = 25;
int cont_binary_threshold = 60;
int cont_draw_deflection = 100;
int cont_draw_attenuate_r = 10;
int cont_draw_attenuate_k = 70;
int cont_draw_line_num = 100;
int cont_draw_line_length = 100;
int cont_color_channel = 4;

vector<g_Point> GetTrajectory(Mat& src)
{
	Mat in = src.clone();
	vector<g_Point> ret;
	g_Point st, now;
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			if (in.at<uchar>(i, j) == 0)
			{
				ret.push_back(g_Point(i, j));
			}
		}
	}
	return ret;
}

void GenerateLinearField(Mat& src, Mat &out)
{
	Mat in = src.clone();
	out = src.clone();
	vector<g_Point> blackP;
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			if (in.at<uchar>(i, j) == 0)
			{
				blackP.push_back(g_Point(i, j));
			}
		}
	}
	int r = 100;
	for (int cnt = 0; cnt < blackP.size(); cnt++)
	{
		for (int i = -r; i <= r; i++)
		{
			int max_j = sqrt(r * r - i * i);
			for (int j = -max_j; j <= max_j; j++)
			{
				//int tmp_color = min(sqrt(i*i + j * j) * 255 / r, 255.0);
				int tmp_color = 255 - GaussianFunction(255, 0, 0.3, sqrt(i*i + j * j) / r);
				int new_x = blackP[cnt].x + i;
				int new_y = blackP[cnt].y + j;
				if (new_x >= 0 && new_x < in.rows && new_y >= 0 && new_y < in.cols)
				{
					out.at<uchar>(new_x, new_y) = min((int)out.at<uchar>(new_x, new_y), tmp_color);
				}
			}
		}
	}
}

void MixLinearField(Mat& m1, Mat& m2, double alpha, Mat& out)
{
	out = m1.clone();
	for (int i = 0; i < m1.rows; i++)
	{
		for (int j = 0; j < m1.cols; j++)
		{
			out.at<uchar>(i, j) = 255 - min(pow(255 - m1.at<uchar>(i, j) + 1, alpha)*pow(255 - m2.at<uchar>(i, j) + 1, 1 - alpha), 255.0);
			//out.at<uchar>(i, j) = m1.at<uchar>(i, j)*alpha + m2.at<uchar>(i, j)*(1 - alpha);
		}
	}
}

void GetFocusLine(Mat& m, Mat& out, int coreSize)
{
	int r = coreSize / 2;
	if (coreSize % 2 == 0)
		return;
	Mat in = m.clone();
	out = m.clone();
	for (int i = r; i < m.rows - r; i++)
	{
		for (int j = r; j < m.cols - r; j++)
		{
			uchar center = m.at<uchar>(i, j);
			bool flag = 1;
			for (int ii = i - r; ii <= i + r && flag; ii++)
			{
				for (int jj = j - r; jj <= j + r && flag; jj++)
				{
					if (center > in.at<uchar>(ii, jj))
						flag = 0;
				}
			}
			if (flag)
				out.at<uchar>(i, j) = 0;
			else
				out.at<uchar>(i, j) = 255;
		}
	}
}

void TestLinearField()
{
	const int IMAGE_NUM = 2;
	Mat src[IMAGE_NUM], fimg, outimg;
	//src[0] = imread("shape1.png");
	//src[1] = imread("shape2.png");
	src[0] = imread("test0.png");
	src[1] = imread("test1.png");
	cvtColor(src[0], src[0], COLOR_BGR2GRAY);
	cvtColor(src[1], src[1], COLOR_BGR2GRAY);
	imshow("src0", src[0]);
	imshow("src1", src[1]);
	GenerateLinearField(src[0], src[0]);
	GenerateLinearField(src[1], src[1]);
	imshow("new0", src[0]);
	imshow("new1", src[1]);
	int f_num = 20;
	int f_now = 0;
	while (1)
	{
		double alpha = f_now * 1.0 / f_num;
		MixLinearField(src[0], src[1], alpha, fimg);
		imshow("test", fimg);
		f_now = (f_now + 1) % (f_num + 1);
		waitKey(200);
	}
	waitKey();
}

void GenerateDirectedField_OldFunction(Mat& src, Mat &out)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_32FC4, Scalar(0, 0, 0, 0));
	vector<g_Point> blackP;
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			if (in.at<uchar>(i, j) == 0)
			{
				blackP.push_back(g_Point(i, j));
			}
		}
	}
	int r = 50;
	for (int cnt = 0; cnt < blackP.size(); cnt++)
	{
		for (int i = -r; i <= r; i++)
		{
			int max_j = sqrt(r * r - i * i);
			for (int j = -max_j; j <= max_j; j++)
			{
				int new_x = blackP[cnt].x + i;
				int new_y = blackP[cnt].y + j;
				if (new_x >= 0 && new_x < in.rows && new_y >= 0 && new_y < in.cols)
				{
					float dis = sqrt(i*i + j * j);
					float dx = j * 1.0 / r;
					float dy = -i * 1.0 / r;
					Vec2f v = normalize(Vec2f(dx, dy))*GaussianFunction(1, 0, 0.3, dis*1.0 / r);
					dx = v[0];
					dy = v[1];
					Vec4f bef = out.at<Vec4f>(new_x, new_y);
					if (fabs(dx*dx + dy * dy - bef[0] - bef[3]) < eps)
					{
						out.at<Vec4f>(new_x, new_y) = (Vec4f(dx*dx, dx*dy, dx*dy, dy*dy) + bef)*0.5;
					}
					else if (dx*dx + dy * dy > bef[0] + bef[3])
					{
						out.at<Vec4f>(new_x, new_y) = Vec4f(dx*dx, dx*dy, dx*dy, dy*dy);
					}
				}
			}
		}
	}
	for (int cnt = 0; cnt < blackP.size(); cnt++)
	{
		Vec4f center_color = out.at<Vec4f>(blackP[cnt].x, blackP[cnt].y);
		swap(center_color[0], center_color[3]);
		center_color[1] = center_color[2] = -center_color[1];
		out.at<Vec4f>(blackP[cnt].x, blackP[cnt].y) = center_color;
	}
	for (int cnt = 0; cnt < blackP.size(); cnt++)
	{
		Vec4f center_color = Vec4f(0, 0, 0, 0);
		int sum = 0;
		for (int i = -5; i <= 5; i++)
		{
			for (int j = -5; j <= 5; j++)
			{
				int nxt_x = blackP[cnt].x + i;
				int nxt_y = blackP[cnt].y + j;
				if (nxt_x<0 || nxt_x>in.rows || nxt_y<0 || nxt_y>in.cols || in.at<uchar>(nxt_x, nxt_y) == 0)
					continue;
				sum++;
				center_color += out.at<Vec4f>(nxt_x, nxt_y);
				//cout << blackP[cnt].x << "," << blackP[cnt].y << ",  " << nxt_x << "," << nxt_y << endl;
				//cout << center_color << ", " << out.at<Vec4f>(nxt_x, nxt_y) << endl;
			}
		}
		if (sum)
		{
			center_color /= (center_color[0] + center_color[3]);
			if (center_color[2] >= 0)
				center_color[1] = center_color[2] = sqrt(center_color[0] * center_color[3]);
			else
				center_color[1] = center_color[2] = -sqrt(center_color[0] * center_color[3]);
			out.at<Vec4f>(blackP[cnt].x, blackP[cnt].y) = center_color;
		}
	}
}

void GenerateDirectedField(Mat& src, Mat &out)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_32FC4, Scalar(0, 0, 0, 0));
	vector<g_Point> blackP = GetTrajectory(in);
	int g[2][8] = { {-1,-1,-1,0,0,1,1,1},{-1,0,1,-1,1,-1,0,1} };
	int r = cont_field_r;
	for (int cnt = 0; cnt < blackP.size(); cnt++)
	{
		int nxt_x, nxt_y, adjacentNum = 0;
		Vec4f cont = Vec4f(0, 0, 0, 0);
		for (int gid = 0; gid < 8; gid++)
		{
			nxt_x = blackP[cnt].x + g[0][gid];
			nxt_y = blackP[cnt].y + g[1][gid];
			if (nxt_x >= 0 && nxt_x < in.rows && nxt_y >= 0 && nxt_y < in.cols&&in.at<uchar>(nxt_x, nxt_y) == 0)
			{
				cont += Vec4f(g[0][gid] * g[0][gid], g[0][gid] * g[1][gid], g[1][gid] * g[1][gid], 0)*sqrt(g[0][gid] * g[0][gid] + g[1][gid] * g[1][gid]);
				adjacentNum++;
			}
		}
		if (adjacentNum <= 1)
			continue;
		for (int i = -r * 2; i <= r * 2; i++)
		{
			int max_j = sqrt(r * r * 4 - i * i);
			for (int j = -max_j; j <= max_j; j++)
			{
				int new_x = blackP[cnt].x + i;
				int new_y = blackP[cnt].y + j;
				if (new_x >= 0 && new_x < in.rows && new_y >= 0 && new_y < in.cols)
				{
					float dis = sqrt(i*i + j * j);
					Vec4f newP = out.at<Vec4f>(new_x, new_y);
					newP += cont;
					newP[3] = max(newP[3], (float)GaussianFunction(1, 0, 0.3, dis*1.0 / r));
					//newP[3] = max(newP[3], 1 - dis * 1.0f / r);
					out.at<Vec4f>(new_x, new_y) = newP;
				}
			}
		}
	}
	for (int i = 0; i < out.rows; i++)
	{
		for (int j = 0; j < out.cols; j++)
		{
			Vec4f befP = out.at<Vec4f>(i, j);
			if (befP[3] < eps)
			{
				out.at<Vec4f>(i, j) = Vec4f(0, 0, 0, 0);
				continue;
			}
			//double exponent_value = 1;
			double exponent_value = max(log(r*2.0 / max(fabs(befP[1]), 1.0f)), 1.0);
			Vec2f newV = normalize(Vec2f(pow(befP[0], exponent_value), pow(befP[2], exponent_value)))*befP[3];
			if (befP[1] >= 0)
				out.at<Vec4f>(i, j) = Vec4f(newV[0] * newV[0], newV[0] * newV[1], newV[0] * newV[1], newV[1] * newV[1]);
			else
				out.at<Vec4f>(i, j) = Vec4f(newV[0] * newV[0], -newV[0] * newV[1], -newV[0] * newV[1], newV[1] * newV[1]);

		}
	}
}

void GenerateDirectedFieldFromVector(VectorPicture& vp, Mat &out)
{
	out = Mat(vp.rows, vp.cols, CV_32FC4, Scalar(0, 0, 0, 0));
	int r = cont_field_r;
	for (int lineId = 0; lineId < vp.lines.size(); lineId++)
	{
		for (int pointId = 0; pointId < vp.lines[lineId].size() - 1; pointId++)
		{
			Vec2f stPoint = vp.lines[lineId][pointId];
			Vec2f edPoint = vp.lines[lineId][pointId + 1];
			int mni = max(0, (int)min(stPoint[0], edPoint[0]) - r);
			int mxi = min(vp.rows - 1, (int)max(stPoint[0], edPoint[0]) + r);
			int mnj = max(0, (int)min(stPoint[1], edPoint[1]) - r);
			int mxj = min(vp.cols - 1, (int)max(stPoint[1], edPoint[1]) + r);
			g_Line nowLine = g_Line(g_Point(stPoint[0], stPoint[1]), g_Point(edPoint[0], edPoint[1]));
			Vec4f nowTensor = Vec4f(
				(nowLine.e.x - nowLine.s.x)*(nowLine.e.x - nowLine.s.x),
				(nowLine.e.x - nowLine.s.x)*(nowLine.e.y - nowLine.s.y),
				(nowLine.e.y - nowLine.s.y)*(nowLine.e.y - nowLine.s.y), 0);
			nowTensor /= (nowTensor[0] + nowTensor[2]);
			for (int i = mni; i <= mxi; i++)
			{
				for (int j = mnj; j <= mxj; j++)
				{
					g_Point nowPoint = g_Point(i, j);
					double dis = dist(nowPoint, NearestPointToLineSeg(nowPoint, nowLine));
					if ((nowPoint - nowLine.s)*(nowLine.e - nowLine.s) < 0 ||
						(nowPoint - nowLine.e)*(nowLine.s - nowLine.e) < 0 || dis > r)
						continue;
					Vec4f newP = nowTensor;
					double value=
					newP[3] = GaussianFunction(1, 0, 0.3, dis*1.0 / r);
					if (newP[3] > out.at<Vec4f>(i, j)[3] - eps)
					{
						out.at<Vec4f>(i, j) = newP;
					}
				}
			}
		}
	}
	for (int lineId = 0; lineId < vp.lines.size(); lineId++)
	{
		for (int pointId = 0; pointId < vp.lines[lineId].size(); pointId++)
		{
			g_Point midPoint, befPoint, aftPoint;
			midPoint = vp.lines[lineId][pointId];
			if(pointId>0)
				befPoint = vp.lines[lineId][pointId - 1];
			else
				befPoint = vp.lines[lineId][pointId + 1];
			if(pointId< vp.lines[lineId].size()-1)
				aftPoint = vp.lines[lineId][pointId + 1];
			else
				aftPoint = vp.lines[lineId][pointId - 1];
			Vec2f v1 = normalize(Vec2f(midPoint.x - befPoint.x, midPoint.y - befPoint.y));
			Vec2f v2 = normalize(Vec2f(midPoint.x - aftPoint.x, midPoint.y - aftPoint.y));
			Vec4f tensor1 = Vec4f(v1[0], 0);
			tensor1 /= tensor1[0] + tensor1[2];
			Vec4f tensor2 = Vec4f((midPoint.x - aftPoint.x)*(midPoint.x - aftPoint.x),
				(midPoint.x - aftPoint.x)*(midPoint.y - aftPoint.y),
				(midPoint.y - aftPoint.y)*(midPoint.y - aftPoint.y), 0);
			tensor2 /= tensor2[0] + tensor2[2];
			for (int i = midPoint.x - r; i <= midPoint.x + r; i++)
			{
				for (int j = midPoint.y - r; j <= midPoint.y + r; j++)
				{
					if (i == midPoint.x&&j == midPoint.y)
						continue;
					g_Point nowPoint = g_Point(i, j);
					double dis = dist(nowPoint, midPoint);
					if (dis > r)
						continue;
					if ((befPoint - midPoint)*(aftPoint - midPoint) > 0)
					{
						Vec2f v = v1 * ((nowPoint - midPoint)* g_Point(v2)) + v2 * ((nowPoint - midPoint)* g_Point(v1));
						v = normalize(v);
						Vec4f newP = Vec4f(v[0] * v[0], v[0] * v[1], v[1] * v[1], GaussianFunction(1, 0, 0.3, dis*1.0 / r));
						if (newP[3] > out.at<Vec4f>(i, j)[3])
						{
							out.at<Vec4f>(i, j) = newP;
						}
					}
					else
					{
						Vec2f v = Vec2f(i - midPoint.x, j - midPoint.y);
						v = normalize(Vec2f(v[1], -v[0]));
						Vec4f newP = Vec4f(v[0] * v[0], v[0] * v[1], v[1] * v[1], GaussianFunction(1, 0, 0.3, dis*1.0 / r));
						if (newP[3] > out.at<Vec4f>(i, j)[3] - eps)
						{
							out.at<Vec4f>(i, j) = newP;
						}
					}
					//out.at<Vec4f>(i, j) = Vec4f(1, 0, 0, 1);
				}
			}
		}
	}
	for (int i = 0; i < vp.rows; i++)
	{
		for (int j = 0; j < vp.cols; j++)
		{
			Vec4f newP = out.at<Vec4f>(i, j);
			if (newP[3] < eps)
				continue;
			newP *= newP[3];
			newP[3] = newP[2];
			newP[2] = newP[1];
			out.at<Vec4f>(i, j) = newP;
		}
	}
}

void MixDirectedField(Mat& m1, Mat& m2, double alpha, Mat& out)
{
	out = m1.clone();
	for (int i = 0; i < m1.rows; i++)
	{
		for (int j = 0; j < m1.cols; j++)
		{
			out.at<Vec4f>(i, j) = m1.at<Vec4f>(i, j)*alpha + m2.at<Vec4f>(i, j)*(1 - alpha);
		}
	}
}

void DirectedField2ColorImage(Mat& src, Mat &out, int channel = 4)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_32FC3);
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			if (channel == 4)
			{
				Vec4f now = in.at<Vec4f>(i, j);
				out.at<Vec3f>(i, j) = Vec3f(now[0], fabs(now[1]), now[3]);
			}
			else
			{
				Vec4f now = in.at<Vec4f>(i, j);
				float tmp = fabs(now[channel]);
				//float tmp = max(now[channel], 0.0f);
				out.at<Vec3f>(i, j) = Vec3f(tmp, tmp, tmp);
			}
		}
	}
}


void GenerateIntensityImage(const Mat& src, Mat &out)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_8U);
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			//Vec4f tmp = EigenFromTensor(in.at<Vec4f>(i, j));
			Vec4f tmp = in.at<Vec4f>(i, j);
			out.at<uchar>(i, j) = max(min(tmp[3], 1.0f), 0.0f) * 255;
		}
	}
}

void GenerateIntensityImageOf32F(const Mat& src, Mat &out)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_32FC1);
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			//Vec4f tmp = EigenFromTensor(in.at<Vec4f>(i, j));
			Vec4f tmp = in.at<Vec4f>(i, j);
			out.at<float>(i, j) = max(min(tmp[3], 1.0f), 0.0f);
		}
	}
}

void GenerateEigenImage(Mat& src, Mat& out)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_32FC4);
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			out.at<Vec4f>(i, j) = EigenFromTensor(in.at<Vec4f>(i, j));
		}
	}
}

void GenerateTensorImage(Mat& src, Mat& out)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_32FC4);
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			out.at<Vec4f>(i, j) = TensorFromEigen(in.at<Vec4f>(i, j));
		}
	}
}

void IntensityImage2NormalMap(Mat& src, Mat &out)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_8UC3);
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			if (i == 0 || j == 0 || i == in.rows - 1 || j == in.cols - 1)
			{
				out.at<Vec3b>(i, j) = Vec3f(1.0, 0.5, 0.5) * 255;
				continue;
			}
			Vec3f norm = Vec3f(5.0f / 255,
				(in.at<float>(i + 1, j - 1) + in.at<float>(i + 1, j) + in.at<float>(i + 1, j + 1)) / 3
					- (in.at<float>(i - 1, j - 1) + in.at<float>(i - 1, j) + in.at<float>(i - 1, j + 1)) / 3,
				(in.at<float>(i - 1, j + 1) + in.at<float>(i, j + 1) + in.at<float>(i + 1, j + 1)) / 3
				- (in.at<float>(i - 1, j - 1) + in.at<float>(i, j - 1) + in.at<float>(i + 1, j - 1)) / 3);
			norm = normalize(norm);
			out.at<Vec3b>(i, j) = (norm * 0.5 + Vec3f(0.5, 0.5, 0.5))*255;
		}
	}
}

void FlowBaseDoG(Mat& src, Mat& out)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_8U);
	const int width = 20;
	double f[width + 1];
	for (int i = 0; i <= width; i++)
	{
		//f[i] = GaussianFunction(0.7, 0, 0.1, i*1.0 / width) - GaussianFunction(0.25, 0, 0.3, i*1.0 / width);
		f[i] = GaussianFunction(0.15, i*1.0 / width) - GaussianFunction(0.24, i*1.0 / width);
		//cout << i << ":" << f[i] << endl;
	}
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			Vec4f now = in.at<Vec4f>(i, j);
			if (now[3] < eps)
			{
				out.at<uchar>(i, j) = 0;
				continue;
			}
			Vec2f cut_direction = getRotateLeft(Vec2f(now[0], now[1]));
			double sum = 0;
			for (int step = -width; step <= width; step++)
			{
				int cx = round(i + step * cut_direction[0]);
				int cy = round(j + step * cut_direction[1]);
				if (cx < 0 || cx >= in.rows || cy < 0 || cy >= in.cols)
					continue;
				sum += in.at<Vec4f>(cx, cy)[3] * f[abs(step)];
			}
			//sum /= 10;
			//cout << sum << endl;
			out.at<uchar>(i, j) = min(max(int(sum * 255), 0), 255);
		}
	}
}

void TestDirectedFieldAndFlowBaseDoG()
{
	const int IMAGE_NUM = 2;
	Mat src[IMAGE_NUM], outimg[IMAGE_NUM], fimg;
	//src[0] = imread("shape1.png");
	//src[1] = imread("shape2.png");
	src[0] = imread("test0.png");
	src[1] = imread("test1.png");
	cvtColor(src[0], src[0], COLOR_BGR2GRAY);
	cvtColor(src[1], src[1], COLOR_BGR2GRAY);
	imshow("src0", src[0]);
	imshow("src1", src[1]);
	GenerateDirectedField(src[0], src[0]);
	GenerateDirectedField(src[1], src[1]);
	DirectedField2ColorImage(src[0], outimg[0]);
	DirectedField2ColorImage(src[1], outimg[1]);
	imshow("new0", outimg[0]);
	imshow("new1", outimg[1]);
	int f_num = 20;
	int f_now = 0;
	while (1)
	{
		double alpha = f_now * 1.0 / f_num;
		MixDirectedField(src[0], src[1], alpha, fimg);
		DirectedField2ColorImage(fimg, outimg[0]);
		imshow("dyed vector image", outimg[0]);
		GenerateEigenImage(fimg, outimg[1]);
		FlowBaseDoG(outimg[1], outimg[1]);
		imshow("FDoG", outimg[1]);
		GenerateIntensityImage(fimg, fimg);
		imshow("intensity image", fimg);
		f_now = (f_now + 1) % (f_num + 1);
		waitKey(1000);
	}
	waitKey();
}

void MixExponentialDirectedField(Mat& m1, Mat& m2, double alpha, Mat& out)
{
	Mat eigen1, eigen2;
	GenerateEigenImage(m1, eigen1);
	GenerateEigenImage(m2, eigen2);
	MixDirectedField(m1, m2, alpha, out);
	GenerateEigenImage(out, out);
	for (int i = 0; i < m1.rows; i++)
	{
		for (int j = 0; j < m1.cols; j++)
		{
			Vec4f eigenPoint1 = eigen1.at<Vec4f>(i, j);
			Vec4f eigenPoint2 = eigen2.at<Vec4f>(i, j);
			Vec4f outPoint = out.at<Vec4f>(i, j);
			//double val1 = fabs(outPoint[0] * eigenPoint1[0] + outPoint[1] * eigenPoint1[1])*eigenPoint1[3];
			//double val2 = fabs(outPoint[0] * eigenPoint2[0] + outPoint[1] * eigenPoint2[1])*eigenPoint2[3];
			double val1 = eigenPoint1[3];
			double val2 = eigenPoint2[3];
			//if (val1 + val2 > eps)
			{
				val1 += GaussEps;
				val2 += GaussEps;
			}
			double k = cont_derivative_k * 0.01;
			double sk = cont_self_k * 0.01;
			out.at<Vec4f>(i, j)[3] = sk * pow(val1, alpha)*pow(val2, 1 - alpha) + k * fabs(val2*log(val1 / val2)*pow(val1 / val2, alpha));
			//out.at<Vec4f>(i, j)[3] = val1 * alpha + val2 * (1 - alpha);
		}
	}
}

void GenerateThinImage(Mat& src, Mat& out)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_8U, Scalar(0));
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			Vec4f tensor = in.at<Vec4f>(i, j);
			if (tensor[3] < eps)
				continue;
			Vec4f eigen = EigenFromTensor(tensor);
			pair<int, int> nxt1 = getNextPoint(getRotateLeft(Vec2f(eigen[0], eigen[1])));
			pair<int, int> nxt2 = getNextPoint(getRotateRight(Vec2f(eigen[0], eigen[1])));
			int lefP_x = i + nxt1.first, lefP_y = j + nxt1.second;
			int rigP_x = i + nxt2.first, rigP_y = j + nxt2.second;
			int lefP_x2 = i + nxt1.first * 2, lefP_y2 = j + nxt1.second * 2;
			int rigP_x2 = i + nxt2.first * 2, rigP_y2 = j + nxt2.second * 2;
			if ((lefP_x <0 || lefP_x>in.rows || lefP_y<0 || lefP_y>in.cols || in.at<Vec4f>(lefP_x, lefP_y)[3] <= tensor[3])
				&& (rigP_x <0 || rigP_x>in.rows || rigP_y<0 || rigP_y>in.cols || in.at<Vec4f>(rigP_x, rigP_y)[3] <= tensor[3])
				&& (lefP_x2 <0 || lefP_x2>in.rows || lefP_y2<0 || lefP_y2>in.cols || in.at<Vec4f>(lefP_x2, lefP_y2)[3] <= tensor[3])
				&& (rigP_x2 <0 || rigP_x2>in.rows || rigP_y2<0 || rigP_y2>in.cols || in.at<Vec4f>(rigP_x2, rigP_y2)[3] <= tensor[3]))
				out.at<uchar>(i, j) = 255;
		}
	}
}

void LinearAddImage(Mat& m1, double a1, Mat& m2, double a2, Mat& out)
{
	out = m1.clone();
	for (int i = 0; i < m1.rows; i++)
	{
		for (int j = 0; j < m1.cols; j++)
		{
			int pix = round(m1.at<uchar>(i, j)*a1 + m2.at<uchar>(i, j)*a2);
			pix = min(255, max(0, pix));
			out.at<uchar>(i, j) = pix;
		}
	}
}

void GetDifference(Mat& m1, Mat& m2, double k, Mat& out)
{
	out = m1.clone();
	for (int i = 0; i < m1.rows; i++)
	{
		for (int j = 0; j < m1.cols; j++)
		{
			int pix = abs((int)m1.at<uchar>(i, j) - (int)m2.at<uchar>(i, j))*k;
			pix = min(255, max(0, pix));
			out.at<uchar>(i, j) = pix;
		}
	}
}

Vec2f GetDeflection(pair<int, int> nxt)
{
	return Vec2f(nxt.second, -nxt.first)*0.01*(cont_draw_deflection - 100);
}

double GetValueOfRandLine(const Mat& src, g_Point st, int lineLength, int dir)
{
	double rand_deflection = (1ll * lineLength*lineLength * 233 / 17 % 120 - 20) * 0.01;
	double ret = 0;
	pair<int, int> nxt, now = make_pair(st.x, st.y);
	set<pair<int, int> > his;
	Vec2f lazy = Vec2f(0, 0);
	Vec4f st_tensor = src.at<Vec4f>(st.x, st.y);
	Vec4f st_eigen = EigenFromTensor(st_tensor);
	Vec2f draw_direction = Vec2f(st_eigen[0], st_eigen[1])*dir;
	int tlen = 0;
	for (int draw_id = 0; draw_id < lineLength; draw_id++)
	{
		if (now.first < 0 || now.first >= src.rows || now.second < 0 || now.second >= src.cols)
			break;
		Vec4f tensor = src.at<Vec4f>(now.first, now.second);
		Vec4f eigen = EigenFromTensor(tensor);
		if (eigen[3] < eps)
			break;
		ret += eigen[3] / 2;
		if (draw_direction[0] * eigen[0] + draw_direction[1] * eigen[1] > 0)
		{
			nxt = getNextPoint(Vec2f(eigen[0], eigen[1]) + lazy);
			draw_direction = Vec2f(eigen[0], eigen[1]);
		}
		else
		{
			nxt = getNextPoint(Vec2f(-eigen[0], -eigen[1]) + lazy);
			draw_direction = Vec2f(-eigen[0], -eigen[1]);
		}
		draw_direction = normalize(draw_direction);
		lazy += draw_direction * sqrt(nxt.first*nxt.first + nxt.second*nxt.second) - Vec2f(nxt.first, nxt.second);
		if (lazy[0] * lazy[0] + lazy[1] * lazy[1] > 1)
			lazy = normalize(lazy) * 1;
		lazy += GetDeflection(nxt)*rand_deflection;
		now.first += nxt.first;
		now.second += nxt.second;
		his.insert(now);
		tlen++;
	}
	return ret * tlen*1.0 / lineLength;
}

void DrawRandLine(Mat& src, Mat &out, g_Point st, int lineLength, int dir)
{
	int r = cont_draw_attenuate_r;
	double rand_deflection = (1ll * lineLength*lineLength * 233 / 17 % 120 - 20) * 0.01;
	Mat mask = Mat(src.rows, src.cols, CV_8U, Scalar(0));
	vector<pair<int, int>> vec;
	pair<int, int> nxt = make_pair(0, 0);
	pair<int, int> now = make_pair(st.x, st.y);
	set<pair<int, int> > his;
	Vec2f lazy = Vec2f(0, 0);
	Vec4f st_tensor = src.at<Vec4f>(st.x, st.y);
	Vec4f st_eigen = EigenFromTensor(st_tensor);
	Vec2f draw_direction = Vec2f(st_eigen[0], st_eigen[1])*dir;
	double col = 1.0;
	for (int draw_id = 0; draw_id < lineLength; draw_id++)
	{
		if (now.first < 0 || now.first >= src.rows || now.second < 0 || now.second >= src.cols)
			break;
		Vec4f tensor = src.at<Vec4f>(now.first, now.second);
		Vec4f eigen = EigenFromTensor(tensor);
		if (eigen[3] < GaussEps)
			break;
		for (int i = -r; i <= +r; i++)
		{
			int j_limit = sqrt(r*r - i * i);
			for (int j = -j_limit; j <= j_limit; j++)
			{
				int mx = i + now.first;
				int my = j + now.second;
				double md = sqrt(i*i + j * j);
				if (mx < 0 || mx >= src.rows || my < 0 || my >= src.cols)
					continue;
				mask.at<uchar>(mx, my) = max((int)mask.at<uchar>(mx, my), int(255 * (r - md) / r));
			}
		}
		out.at<uchar>(now.first, now.second) = col * 255;
		//col *= 0.997;
		if (draw_direction[0] * eigen[0] + draw_direction[1] * eigen[1] > 0)
		{
			nxt = getNextPoint(Vec2f(eigen[0], eigen[1]) + lazy);
			draw_direction = Vec2f(eigen[0], eigen[1]);
		}
		else
		{
			nxt = getNextPoint(Vec2f(-eigen[0], -eigen[1]) + lazy);
			draw_direction = Vec2f(-eigen[0], -eigen[1]);
		}
		if (his.find(make_pair(now.first + nxt.first, now.second + nxt.second)) != his.end())
			break;
		draw_direction = normalize(draw_direction);
		lazy += draw_direction * sqrt(nxt.first*nxt.first + nxt.second*nxt.second) - Vec2f(nxt.first, nxt.second);
		if (lazy[0] * lazy[0] + lazy[1] * lazy[1] > 1)
			lazy = normalize(lazy) * 1;
		lazy += GetDeflection(nxt)*rand_deflection;
		now.first += nxt.first;
		now.second += nxt.second;
		his.insert(now);
	}
	for (int i = 0; i < src.rows; i++)
	{
		for (int j = 0; j < src.cols; j++)
		{
			if (mask.at<uchar>(i, j) > 0)
				src.at<Vec4f>(i, j) *= 1 - mask.at<uchar>(i, j)*1.0 / 255 * cont_draw_attenuate_k / 100;
		}
	}
}

void GenerateLineDraftFromTensor(Mat& src, Mat& out, double threshold, int line_num, int line_length)
{
	Mat in = src.clone();
	out = Mat(in.rows, in.cols, CV_8U, Scalar(0));
	vector<g_Point> vec;
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			Vec4f tensor = in.at<Vec4f>(i, j);
			Vec4f eigen = EigenFromTensor(tensor);
			if (eigen[3] >= threshold)
			{
				vec.push_back(g_Point(i, j));
			}
		}
	}
	if (vec.size() == 0)
		return;
	int PointRandNum = 100;
	//int PointRandNum = 1;
	vector<g_Point> randP;
	vector<double> randValueV;
	vector<int> randDirV;
	while (line_num--)
	{
		randP.clear();
		randValueV.clear();
		randDirV.clear();
		int linelength = line_length * (0.7 + 0.3*rand() / RAND_MAX);
		//linelength = 100000;
		for (int rid = 0; rid < PointRandNum; rid++)
		{
			g_Point st = vec[1ll * rand()*rand() % vec.size()];
			int dir = rand() % 2 * 2 - 1;
			randP.push_back(st);
			randValueV.push_back(GetValueOfRandLine(in, st, linelength, dir));
			randDirV.push_back(dir);
		}
		if (randP.size() == 0)
			break;
		int mxId = 0;
		for (int rid = 0; rid < randP.size(); rid++)
		{
			if (randValueV[rid] > randValueV[mxId])
				mxId = rid;
		}
		DrawRandLine(in, out, randP[mxId], linelength, randDirV[mxId]);
	}
}

/*
void debug(Mat &in)
{
	for (int i = 0; i < in.rows; i++)
	{
		for (int j = 0; j < in.cols; j++)
		{
			Vec4f tmp = in.at<Vec4f>(i, j);
			if (tmp[0] != 0)
				cout << tmp << endl;
		}
	}
}
*/

void TestDirectionFieldAndCurveDraft()
{
	const int IMAGE_NUM = 2;
	Mat src[IMAGE_NUM], src_color[IMAGE_NUM], src_instensity[IMAGE_NUM], src_eigen[IMAGE_NUM], src_draft[IMAGE_NUM];
	Mat now_img, ts_img, thin_img, FDoG_img, straight_line_img;
	Mat intensity_img, threshold_img, draft_img, color_img, eigen_img;
	//src[0] = imread("draft1.bmp");
	//src[1] = imread("draft2.bmp");
	src[0] = imread("shape1.png");
	src[1] = imread("shape2.png");

	namedWindow("threshold intensity image");
	imshow("threshold intensity image", src[0]);
	createTrackbar("本体系数", "threshold intensity image", &cont_self_k, 100, NULL);
	createTrackbar("导数系数", "threshold intensity image", &cont_derivative_k, 50, NULL);
	createTrackbar("二值化阈值", "threshold intensity image", &cont_binary_threshold, 255, NULL);

	namedWindow("staight line image");
	imshow("staight line imag", src[0]);
	createTrackbar("疏密度", "staight line image", &DR, MAX_DR, NULL);
	createTrackbar("线段长度", "staight line image", &LineMidL, MAX_LineMidL, NULL);

	namedWindow("tensor image");
	imshow("tensor image", src[0]);
	createTrackbar("颜色通道", "tensor image", &cont_color_channel, 4, NULL);

	namedWindow("line draft");
	imshow("line draft", src[0]);
	createTrackbar("线段数量", "line draft", &cont_draw_line_num, 200, NULL);
	createTrackbar("线段长度", "line draft", &cont_draw_line_length, 500, NULL);
	createTrackbar("偏转值", "line draft", &cont_draw_deflection, 200, NULL);
	//createTrackbar("衰减系数", "line draft", &draw_attenuate_k, 100, NULL);
	createTrackbar("分散程度", "line draft", &cont_draw_attenuate_r, 50, NULL);

	cvtColor(src[0], src[0], COLOR_BGR2GRAY);
	cvtColor(src[1], src[1], COLOR_BGR2GRAY);
	imshow("src0 in Source Image", src[0]);
	imshow("src1 in Source Image", src[1]);

	GenerateDirectedField(src[0], src[0]);
	GenerateDirectedField(src[1], src[1]);

	GenerateLineDraftFromTensor(src[0], src_draft[0], cont_binary_threshold / 255.0, 100, 1000);
	GenerateLineDraftFromTensor(src[1], src_draft[1], cont_binary_threshold / 255.0, 100, 1000);
	imshow("src0 in Draft Image", src_draft[0]);
	imshow("src1 in Draft Image", src_draft[1]);

	GenerateEigenImage(src[0], src_eigen[0]);
	GenerateEigenImage(src[1], src_eigen[1]);
	GenerateIntensityImage(src_eigen[0], src_instensity[0]);
	GenerateIntensityImage(src_eigen[1], src_instensity[1]);
	imshow("src0 in Intensity Image", src_instensity[0]);
	imshow("src1 in Intensity Image", src_instensity[1]);

	DirectedField2ColorImage(src[0], src_color[0]);
	DirectedField2ColorImage(src[1], src_color[1]);
	imshow("src0 in Color Image", src_color[0]);
	imshow("src1 in Color Image", src_color[1]);

	//waitKey();
	//return 0;

	int f_num = 40;
	int f_now = 0;
	while (1)
	{
		double alpha = min(f_now, f_num - f_now) * 1.0 / (f_num*0.5);
		cout << alpha << endl;
		MixExponentialDirectedField(src[0], src[1], alpha, eigen_img);
		GenerateTensorImage(eigen_img, ts_img);
		//dilate(ts_img, ts_img, getStructuringElement(MORPH_RECT, Size(11, 11)));
		GaussianBlur(ts_img, ts_img, Size(3, 3), 0, 0);
		DirectedField2ColorImage(ts_img, color_img, cont_color_channel);
		imshow("tensor image", color_img);
		GenerateIntensityImage(eigen_img, intensity_img);
		imshow("intensity image", intensity_img);
		threshold(intensity_img, threshold_img, cont_binary_threshold, 255, THRESH_BINARY);
		imshow("threshold intensity image", threshold_img);
		GenerateLineDraftFromTensor(ts_img, draft_img, cont_binary_threshold / 255.0, cont_draw_line_num, cont_draw_line_length);
		threshold(draft_img, draft_img, 1, 255, THRESH_BINARY_INV);
		imshow("line draft", draft_img);
		/*
		thin_img = thinImage(threshold_img);
		imshow("thin image", thin_img);
		GenerateStraightLineDraftFromThin(thin_img, straight_line_img);
		imshow("staight line image", straight_line_img);
		*/
		f_now = (f_now + 1) % (f_num + 1);
		waitKey(200);
	}
	waitKey();
}
